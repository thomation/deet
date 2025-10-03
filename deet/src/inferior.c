#include "inferior.h"
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <sys/wait.h>
#include <sys/ptrace.h>
#include <signal.h>
#include <sys/reg.h>
#include <sys/user.h>
#include "dwarf.h"

struct _inferior
{
    const char *prog_path;
    pid_t child_pid;
    int child_execed;
    breakpoints *bp_ref;      // 引用的断点结构体，不要free它
    int stoped_at_breakpoint; // 是否停在断点处
};
static long set_breakpoint(inferior *inf, unsigned long addr)
{
    long data = ptrace(PTRACE_PEEKDATA, inf->child_pid, (void *)addr, NULL);
    if (data == -1)
    {
        perror("ptrace(PEEKDATA)");
        return -1;
    }
    long data_with_int3 = (data & ~0xff) | 0xcc; // 将最低字节替换为0xcc
    if (ptrace(PTRACE_POKEDATA, inf->child_pid, (void *)addr, (void *)data_with_int3) == -1)
    {
        perror("ptrace(POKEDATA)");
        return -1;
    }
    printf("Set breakpoint at 0x%lx, data: 0x%lx  -> 0x%lx\n", addr, data, data_with_int3);
    return data;
}

static void set_breakpoints(inferior *inf)
{
    if (inf == NULL || inf->bp_ref == NULL)
        return;
    for (int i = 0; i < breakpoints_count(inf->bp_ref); ++i)
    {
        unsigned long addr = breakpoints_get_address(inf->bp_ref, i);
        // 在子进程中设置断点
        long original_data = set_breakpoint(inf, addr);
        if (original_data == -1)
        {
            printf("Failed to set breakpoint at 0x%lx\n", addr);
        }
        else
        {
            // 保存 original_data 以便恢复
            breakpoints_set_original_data(inf->bp_ref, i, (unsigned char)(original_data & 0xff));
        }
    }
}
void wait_child(inferior *inf)
{
    int status = 0;
    pid_t pid = inf->child_pid;
    waitpid(pid, &status, 0);
    if (WIFSTOPPED(status) && WSTOPSIG(status) == SIGTRAP)
    {
        // exec之前会触发一次SIGTRAP，必须Continue，让exec执行，然后停下，等待命令
        printf("Child stopped by SIGTRAP child running = %d, stoped at breakpoint = %d\n", inf->child_execed, inf->stoped_at_breakpoint);
        // 第一次是exec之前的，继续执行到断点处
        if (!inf->child_execed)
        {
            printf("Child execed, now set breakpoints.\n");
            set_breakpoints(inf); // 设置断点
            inf->child_execed = 1;
            ptrace(PTRACE_CONT, pid, NULL, NULL);
            wait_child(inf);
        }
        else if(!inf->stoped_at_breakpoint)
        {
            // 继续执行到断点处
            printf("Child hit a breakpoint!\n");
            inf->stoped_at_breakpoint = 1;
        }
    }
    else if (WIFEXITED(status))
    {
        printf("Child exited (status %d)\n", WEXITSTATUS(status));
    }
    else if (WIFSIGNALED(status))
    {
        printf("Child killed by signal %d\n", WTERMSIG(status));
    }
    else if (WIFSTOPPED(status))
    {
        printf("Child stopped by signal %d\n", WSTOPSIG(status));
        inferior_backtrace(inf);
    }
}
inferior *inferior_new(const char *prog_path, int argc, const char **argv, breakpoints *bp)
{
    inferior *inf = (inferior *)malloc(sizeof(inferior));
    if (!inf)
        return NULL;

    inf->prog_path = prog_path;
    inf->child_pid = -1;
    inf->child_execed = 0;
    inf->bp_ref = bp; // 保存断点引用
    inf->stoped_at_breakpoint = 0;

    // 构造execvp参数数组
    char **exec_argv = (char **)malloc(sizeof(char *) * (argc + 2));
    if (!exec_argv)
    {
        free(inf);
        return NULL;
    }
    exec_argv[0] = strdup(prog_path);
    for (int i = 0; i < argc; ++i)
    {
        exec_argv[i + 1] = argv[i] ? strdup(argv[i]) : NULL;
    }
    exec_argv[argc + 1] = NULL;

    pid_t pid = fork();
    if (pid == 0)
    {
        ptrace(PTRACE_TRACEME, 0, NULL, NULL);
        // 子进程
        printf("Starting child process %s\n", prog_path);
        execvp(prog_path, exec_argv);
        // 代码走不到这里，除非execvp失败
        perror("execvp");
        exit(1);
    }
    else if (pid > 0)
    {
        // 父进程
        inf->child_pid = pid;
        wait_child(inf);
    }
    else
    {
        perror("fork");
        for (int i = 0; i < argc + 1; ++i)
            free(exec_argv[i]);
        free(exec_argv);
        free(inf);
        return NULL;
    }

    // 父进程清理参数数组
    for (int i = 0; i < argc + 1; ++i)
        free(exec_argv[i]);
    free(exec_argv);

    return inf;
}

void inferior_continue(inferior *inf)
{
    if (inf && inf->child_pid > 0)
    {
        if(inf->stoped_at_breakpoint)
        {
            // 获取断点地址
            struct user_regs_struct regs;
            if (ptrace(PTRACE_GETREGS, inf->child_pid, NULL, &regs) == -1)
            {
                perror("ptrace(GETREGS)");
                return;
            }
            unsigned long rip = regs.rip;
            unsigned long bp_addr = rip - 1; // 断点地址是当前指令地址减1
            printf("Continuing from breakpoint at 0x%lx\n", bp_addr);
            // 将RIP指向断点地址
            regs.rip = bp_addr;
            if (ptrace(PTRACE_SETREGS, inf->child_pid, NULL, &regs) == -1)
            {
                perror("ptrace(SETREGS)");
                return;
            }
            // 这里需要恢复断点处的原始指令字节，然后单步执行，再次设置断点
            long data = ptrace(PTRACE_PEEKDATA, inf->child_pid, (void *)bp_addr, NULL);
            if (data == -1)
            {
                perror("ptrace(PEEKDATA)");
                return;
            }
            unsigned char original_data = breakpoints_get_original_data(inf->bp_ref, bp_addr);
            long restored_data = (data & ~0xff) | original_data; // 恢复原始字节
            printf("Restore from breakpoint at 0x%lx, data: 0x%lx  -> 0x%lx\n", bp_addr, data, restored_data);
            if (ptrace(PTRACE_POKEDATA, inf->child_pid, (void *)bp_addr, (void *)restored_data) == -1)
            {
                perror("ptrace(POKEDATA)");
                return;
            }
            // 单步执行
            printf("Single stepping over breakpoint at 0x%lx\n", bp_addr);
            if (ptrace(PTRACE_SINGLESTEP, inf->child_pid, NULL, NULL) == -1)
            {
                perror("ptrace(SINGLESTEP)");
                return;
            }
            wait_child(inf);
            // 重新设置断点
            set_breakpoint(inf, bp_addr);
            inf->stoped_at_breakpoint = 0; // 重置状态
        }
        ptrace(PTRACE_CONT, inf->child_pid, NULL, NULL);
        wait_child(inf);
    }
}
static int print_function_name(inferior *inf, unsigned long addr)
{
    char output[256];
    char addr_str[32];
    snprintf(addr_str, sizeof(addr_str), "0x%lx", addr);
    int len = get_function_from_address(inf->prog_path, addr_str, output, sizeof(output));
    if (len > 0)
    {
        printf("Function at address 0x%lx: %s", addr, output);
        // 判断是否为 main 函数
        if (strncmp(output, "main ", 5) == 0 || strncmp(output, "main@", 5) == 0 || strstr(output, "main ") == output)
        {
            return 1;
        }
    }
    else
    {
        printf("Function name not found for address 0x%lx\n", addr);
    }
    return 0;
}
void inferior_backtrace(inferior *inf)
{
    if (inf && inf->child_pid > 0)
    {
        struct user_regs_struct regs;
        if (ptrace(PTRACE_GETREGS, inf->child_pid, NULL, &regs) == -1)
        {
            perror("ptrace(GETREGS)");
            return;
        }
        // RIP寄存器包含当前指令指针，即当前执行的指令地址，崩溃时候也就是崩溃地址
        unsigned long long int instr_ptr = regs.rip;
        unsigned long long int base_ptr = regs.rbp;
        while (1)
        {
            printf("Instr_ptr: 0x%llx\n", instr_ptr);
            if (base_ptr == 0)
                break;
            if (print_function_name(inf, instr_ptr))
            {
                // 如果是 main 函数，停止打印
                break;
            }
            // 读取栈帧中的返回地址
            instr_ptr = ptrace(PTRACE_PEEKDATA, inf->child_pid, base_ptr + 8, NULL);
            // 读取上一个栈帧的基址
            base_ptr = ptrace(PTRACE_PEEKDATA, inf->child_pid, base_ptr, NULL);
        }
    }
}
void inferior_free(inferior *inf)
{
    if (inf)
    {
        if (inf->child_pid > 0)
        {
            printf("Freeing inferior with pid %d\n", inf->child_pid);
            kill(inf->child_pid, SIGKILL);
            int status;
            waitpid(inf->child_pid, &status, 0);
        }
        free(inf);
    }
}