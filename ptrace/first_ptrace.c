#include <unistd.h>
#include <sys/ptrace.h>
#include <sys/wait.h>
#include <sys/user.h>
#include <sys/types.h>
#include <stdio.h>
#include <sys/reg.h>

int main(void){
    pid_t child;
    long orig_rax;
    child = fork();
    if (child == 0){
    	printf("Child process id: %d\n", getpid());
	sleep(5);
        ptrace(PTRACE_TRACEME, 0, NULL, NULL);
	sleep(5);
        execl("/bin/ls", "ls", "-l", "-h", NULL);
    } else {
    	printf("Parent  process id: %d\n", getpid());
    	printf("Parent wait id: %d\n", child);
        wait(NULL);
        printf("Child process called a system call\n");
        orig_rax = ptrace(PTRACE_PEEKUSER, child, 8 * ORIG_RAX, NULL);
        printf("Child process called a system call, id is %ld\n", orig_rax);
	sleep(5);
        ptrace(PTRACE_CONT, child, NULL, NULL);
        printf("ptrace cont %d\n", child);
    }
    printf("all done pid = %d\n", getpid());
    return 0;
}
