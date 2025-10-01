#include <stdlib.h>
#include <string.h>
#include "debuger_command.h"

debuger_command *debuger_command_new(const char *input)
{
    debuger_command *dbg_cmd = (debuger_command *)malloc(sizeof(debuger_command));
    if (dbg_cmd == NULL)
    {
        return NULL;
    }

    // 复制输入，避免修改原字符串
    char *input_copy = strdup(input);
    if (!input_copy)
    {
        free(dbg_cmd);
        return NULL;
    }

    // 解析命令名和参数
    char *token = strtok(input_copy, " ");
    dbg_cmd->cmd = token ? strdup(token) : NULL;

    if (token && strcmp(token, "run") == 0)
    {
        dbg_cmd->cmd_type = CMD_RUN;
    }
    else if (token && strcmp(token, "continue") == 0)
    {
        dbg_cmd->cmd_type = CMD_CONTINUE;
    }
    else if (token && strcmp(token, "step") == 0)
    {
        dbg_cmd->cmd_type = CMD_STEP;
    }
    else if (token && strcmp(token, "quit") == 0)
    {
        dbg_cmd->cmd_type = CMD_QUIT;
    }
    else
    {
        dbg_cmd->cmd_type = CMD_INVALID;
    }

    // 解析参数
    dbg_cmd->argc = 0;
    dbg_cmd->argv = NULL;
    int max_args = 10;
    dbg_cmd->argv = malloc(sizeof(char *) * max_args);
    char *arg;
    while ((arg = strtok(NULL, " ")) != NULL && dbg_cmd->argc < max_args)
    {
        dbg_cmd->argv[dbg_cmd->argc++] = strdup(arg);
    }

    free(input_copy);
    return dbg_cmd;
}
void *debuger_command_free(debuger_command *cmd)
{
    if (cmd)
    {
        if (cmd->cmd)
            free((void *)cmd->cmd);
        for (int i = 0; i < cmd->argc; i++)
        {
            if (cmd->argv[i])
                free(cmd->argv[i]);
        }
        free(cmd->argv);
        free(cmd);
    }
    return NULL;
}