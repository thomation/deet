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

struct _inferior
{
    const char *prog_path;
    pid_t child_pid;
};
void wait_child(inferior * inf)
{
    int status = 0;
    pid_t pid = inf->child_pid;
    waitpid(pid, &status, 0);
    if (WIFSTOPPED(status) && WSTOPSIG(status) == SIGTRAP) {
        // printf("Child stopped by SIGTRAP\n");
        ptrace(PTRACE_CONT, pid, NULL, NULL);
        wait_child(inf);
    }
    else if (WIFEXITED(status)) {
        printf("Child exited (status %d)\n", WEXITSTATUS(status));
    } else if (WIFSIGNALED(status)) {
        printf("Child killed by signal %d\n", WTERMSIG(status));
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
    if (!exec_argv) {
        free(inf);
        return NULL;
    }
    exec_argv[0] = strdup(prog_path);
    for (int i = 0; i < argc; ++i) {
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

void inferior_free(inferior *inf)
{
    if (inf)
        free(inf);
}