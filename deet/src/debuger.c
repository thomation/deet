#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <string.h>
#include "debuger.h"
#include "debuger_command.h"
#include "inferior.h"

struct _debuger
{
    const char *prog_path;
    inferior *inf;
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
    dbg->inf = NULL;
    return dbg;
}
static int check_inferior_running(debuger *dbg)
{
    if (dbg->inf == NULL)
    {
        printf("No program is being debugged. Use 'run' to start.\n");
        return 0;
    }
    return 1;
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
        int argc;
        const char **argv;
        argc = debuger_command_get_args(cmd, &argv);
        switch (debuger_command_get_type(cmd))
        {
        case CMD_RUN:
            if (dbg->inf != NULL)
            {
                inferior_free(dbg->inf);
            }
            dbg->inf = inferior_new(dbg->prog_path, argc, argv);
            break;
        case CMD_CONTINUE:
            if(check_inferior_running(dbg))
                inferior_continue(dbg->inf);
            break;
        case CMD_BACKTRACE:
            if(check_inferior_running(dbg))
                inferior_backtrace(dbg->inf);
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
        if (dbg->inf)
            inferior_free(dbg->inf);
        free(dbg);
    }
}