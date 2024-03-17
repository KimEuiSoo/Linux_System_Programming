#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
int main(void) {
    printf("this is the original program\n");
    //리스트로 인자를 넘겨줘서 ssu_execl_test_1을 실행시킨다.
    execl("./ssu_execl_test_1", "ssu_execl_test_1","param1", "param2", "param3", (char *)0);
    printf("%s\n", "this line should never get printed\n");
    exit(0);
}