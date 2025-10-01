#include <stdio.h>

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        fprintf(stderr, "Usage: %s <program to debug>\n", argv[0]);
        return 1;
    }
    printf("Debugging program: %s\n", argv[1]);
    return 0;
}