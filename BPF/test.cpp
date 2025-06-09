#include <stdio.h>
#include <sys/prctl.h>
#include <string.h>

int main() {
    const char *new_name = "myproc";
    
    if (prctl(PR_SET_NAME, (unsigned long)new_name, 0, 0, 0) == -1) {
        perror("prctl");
        return 1;
    }

    // 변경된 이름 확인 (ps -o comm -p <pid>)
    while(1) {
        sleep(10);
    }

    return 0;
}