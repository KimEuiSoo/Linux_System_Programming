#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
int main(void)
{
    char *argv[] = {
    "ssu_execl_test_1", "param1", "param2", (char *)0
    };
    printf("this is the original program\n");
    //ssu_execl_test_1을 실행시킨다.
    execv("./ssu_execl_test_1", argv);
    printf("%s\n", "This line should never get printed\n");
    exit(0);
}