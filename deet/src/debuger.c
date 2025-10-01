#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <string.h>
#include "debuger.h"
#include "debuger_command.h"

struct _debuger
{
    const char *prog_path;
    pid_t child_pid;
};
static debuger_command *get_next_command(void);

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
    while (1)
    {
        debuger_command *cmd = get_next_command();
        if (cmd == NULL)
        {
            printf("Invalid command.\n");
            continue;
        }
        switch (cmd->cmd_type)
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
            debuger_command_free(cmd);
            return; // 退出调试器
        case CMD_INVALID:
        default:
            printf("Invalid command. Available commands: run, continue, step, quit\n");
            break;
        }
        debuger_command_free(cmd);
    }
}
static debuger_command *get_next_command()
{
    // 显示(deet)提示，并且读入一行用户输入,然后解析命令
    char input[128];
    printf("(deet) ");
    if (fgets(input, sizeof(input), stdin) == NULL)
    {
        return NULL;
    }

    // 去除换行符
    input[strcspn(input, "\n")] = 0;
    return debuger_command_new(input);
}
void debuger_free(debuger *dbg)
{
    if (dbg)
    {
        free(dbg);
    }
}