#include <unistd.h>
#include <sys/ptrace.h>
#include <sys/reg.h>
#include <stdio.h>
#include <sys/wait.h>

int main(int argc, char **argv)
{
    pid_t child = fork();
    int status = 0;
    long orig_rax = 0;
    if (child == 0)
    {
        printf("Child process id: %d\n", getpid());
        sleep(1);
        ptrace(PTRACE_TRACEME, 0, NULL, NULL);
        execl("/bin/ls", "ls", "-l", "-h", NULL);
    }
    else
    {
        while (1)
        {
            printf("Child process %d wait child %d\n", getpid(), child);
            wait(&status);
            printf("Got signal %d\n", WSTOPSIG(status));
            if (WIFEXITED(status))
            {
                printf("Parent process %d: child %d exited\n", getpid(), child);
                break;
            }

            orig_rax = ptrace(PTRACE_PEEKUSER, child, 8 * ORIG_RAX, NULL);
            printf("Program called system call: %ld\n", orig_rax);
            ptrace(PTRACE_SYSCALL, child, NULL, NULL);
            // ptrace(PTRACE_CONT, child, NULL, NULL);
        }
    }
    printf("all done pid = %d\n", getpid());
    return 0;
}