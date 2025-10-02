
#include <stdio.h>
#include <string.h>
#include "dwarf.h"

size_t get_function_from_address(const char *executable, const char *address, char *output, size_t output_size)
{
    char command[1024];
    FILE *fp;

    // 构建命令
    snprintf(command, sizeof(command),
             "addr2line -e %s -f -C -p %s",
             executable, address);

    // 执行命令并读取输出
    fp = popen(command, "r");
    if (fp == NULL)
    {
        perror("popen failed");
        return 0;
    }

    // 清空 output
    output[0] = '\0';
    char tmp[1024];
    size_t written = 0;
    while (fgets(tmp, sizeof(tmp), fp) != NULL)
    {
        size_t to_write = strlen(tmp);
        if (written + to_write >= output_size)
        {
            to_write = output_size - written - 1;
        }
        if (to_write > 0)
        {
            strncat(output, tmp, to_write);
            written += to_write;
        }
        if (written >= output_size - 1)
        {
            break;
        }
    }

    pclose(fp);
    return written;
}