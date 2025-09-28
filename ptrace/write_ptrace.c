#include <unistd.h>
#include <sys/ptrace.h>
#include <sys/reg.h>
#include <stdio.h>
#include <sys/wait.h>
#include <sys/syscall.h>

int main(int argc, char **argv)
{
    pid_t child = fork();
    int status = 0;
    long orig_rax = 0;
    long rax = 0;
    int insyscall = 0;
    long params[3];
    if (child == 0)
    {
        printf("Child process id: %d\n", getpid());
        sleep(1);
        ptrace(PTRACE_TRACEME, 0, NULL, NULL);
        execl("/bin/ls", "ls", "-l", "-h", NULL);
    }
    else
    {
        printf("Parent process %d wait child %d\n", getpid(), child);
        wait(&status);
        ptrace(PTRACE_SYSCALL, child, NULL, NULL);
        printf("ignore the first SIGTRAP signal:execve\n");
        // enter loop
        while (1)
        {
            // printf("Child process %d wait child %d\n", getpid(), child);
            wait(&status);
            // printf("Got signal %d\n", WSTOPSIG(status));
            if (WIFEXITED(status))
            {
                printf("Parent process %d: child %d exited\n", getpid(), child);
                break;
            }
            orig_rax = ptrace(PTRACE_PEEKUSER, child, 8 * ORIG_RAX, NULL);

            // ** PART 1 **
            if (orig_rax != SYS_write)
            {
                ptrace(PTRACE_SYSCALL, child, NULL, NULL);
                continue;
            }
            // ** PART 1 END **

            printf("Got signal SIGTRAP for write system call %d\n", WSTOPSIG(status));

            // ** PART 2 **
            /* Syscall entry */
            if (insyscall == 0)
            {
                insyscall = 1;
                params[0] = ptrace(PTRACE_PEEKUSER, child, 8 * RDI, NULL);
                params[1] = ptrace(PTRACE_PEEKUSER, child, 8 * RSI, NULL);
                params[2] = ptrace(PTRACE_PEEKUSER, child, 8 * RDX, NULL);
                printf(">>> Write called with %ld, %ld, %ld\n", params[0], params[1], params[2]);
            }
            else
            {
                /* Syscall exit */
                rax = ptrace(PTRACE_PEEKUSER, child, 8 * RAX, NULL);
                printf("<<< Write returned with %ld\n", rax);
                insyscall = 0;
            }
            // ** PART 2 END **

            ptrace(PTRACE_SYSCALL, child, NULL, NULL);
        }
    }

    return 0;
}