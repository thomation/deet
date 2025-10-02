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
};
void wait_child(inferior *inf)
{
    int status = 0;
    pid_t pid = inf->child_pid;
    waitpid(pid, &status, 0);
    if (WIFSTOPPED(status) && WSTOPSIG(status) == SIGTRAP)
    {
        // printf("Child stopped by SIGTRAP\n");
        ptrace(PTRACE_CONT, pid, NULL, NULL);
        wait_child(inf);
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
    }
}
inferior *inferior_new(const char *prog_path, int argc, const char **argv)
{
    inferior *inf = (inferior *)malloc(sizeof(inferior));
    if (!inf)
        return NULL;

    inf->prog_path = prog_path;
    inf->child_pid = -1;

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
            if(print_function_name(inf, instr_ptr))
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