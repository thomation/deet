#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <string.h>
#include "debuger.h"
struct _debuger
{
    const char *prog_path;
    pid_t child_pid;
};
enum command
{
    CMD_RUN,
    CMD_CONTINUE,
    CMD_STEP,
    CMD_QUIT,
    CMD_INVALID
};
static int get_next_command(void);

debuger *debuger_new(const char *prog_path)
{
    debuger *dbg = (debuger *)malloc(sizeof(debuger));
    if (dbg == NULL)
    {
        return NULL;
    }
    dbg->prog_path = prog_path;
    dbg->child_pid = -1; // Initialize child_pid
    return dbg;
}
void debuger_run(debuger *dbg)
{
    while(1)
    {
        enum command cmd = get_next_command();
        switch (cmd)
        {
        case CMD_RUN:
            printf("Running program: %s\n", dbg->prog_path);
            // 在这里添加运行程序的代码
            break;
        case CMD_CONTINUE:
            printf("Continuing execution...\n");
            // 在这里添加继续执行的代码
            break;
        case CMD_STEP:
            printf("Stepping through...\n");
            // 在这里添加单步执行的代码
            break;
        case CMD_QUIT:
            printf("Quitting debugger.\n");
            return; // 退出调试器
        case CMD_INVALID:
        default:
            printf("Invalid command. Available commands: run, continue, step, quit\n");
            break;
        }
    }
}
static int get_next_command()
{
    // 显示(deet)提示，并且读入一行用户输入,然后解析命令
    char input[128];
    printf("(deet) ");
    if (fgets(input, sizeof(input), stdin) == NULL) {
        return CMD_INVALID;
    }

    // 去除换行符
    input[strcspn(input, "\n")] = 0;

    if (strcmp(input, "run") == 0) {
        return CMD_RUN;
    } else if (strcmp(input, "continue") == 0) {
        return CMD_CONTINUE;
    } else if (strcmp(input, "step") == 0) {
        return CMD_STEP;
    } else if (strcmp(input, "quit") == 0) {
        return CMD_QUIT;
    } else {
        return CMD_INVALID;
    }
    return CMD_INVALID;
}
void debuger_free(debuger *dbg)
{
    if (dbg)
    {
        free(dbg);
    }
}