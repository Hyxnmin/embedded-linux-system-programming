#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

int main(void) {
    int dev;
    char r1, r2;

    // 디바이스 드라이버 파일 열기
    dev = open("/dev/assign2", O_RDWR);
    if (dev < 0) {
        printf("driver open failed!\n");
        return -1;
    }

    // 메뉴 출력
    printf("Mode 1 : 1\nMode 2 : 2\nMode 3 : 3\nMode 4 : 4\n");

    // 무한 루프를 돌며 사용자 입력 대기
    while (1) {
        printf("Type a Mode: ");
        // 사용자로부터 모드 입력 받기
        scanf(" %c", &r1);

        // 입력받은 모드 값을 드라이버에 전달
        write(dev, &r1, 1);

        // 수동 모드일 경우
        if (r1 == '3') {
            while (1) {
                printf("LED to enable: ");
                scanf(" %c", &r2);
                // LED 제어 명령 전달
                write(dev, &r2, 1);
                if (r2 == '4') break;
            }
        }
        sleep(1);
    }
    // 디바이스 드라이버 닫기
    close(dev);

    return 0;
}