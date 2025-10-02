#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void call_addr2line(const char *executable, const char *address) {
    char command[1024];
    FILE *fp;
    char result[1024];
    
    // 构建命令
    snprintf(command, sizeof(command), 
             "addr2line -e %s -f -C -p %s", 
             executable, address);
    
    // 执行命令并读取输出
    fp = popen(command, "r");
    if (fp == NULL) {
        perror("popen failed");
        return;
    }
    
    // 读取并显示结果
    printf("addr2line result: ");
    while (fgets(result, sizeof(result), fp) != NULL) {
        printf("%s", result);
    }
    
    pclose(fp);
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: %s <executable> <address>\n", argv[0]);
        printf("Example: %s samples/segfault 0x401186\n", argv[0]);
        return 1;
    }
    
    call_addr2line(argv[1], argv[2]);
    return 0;
}