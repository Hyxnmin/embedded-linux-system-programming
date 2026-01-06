CC = gcc
obj-m := driver.o

KDIR := /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)

default:
	$(MAKE) -C $(KDIR) M=$(PWD) modules
	$(CC) -o app_test app_test.c

clean:
	# [Standard Way] 1. 커널 빌드 시스템을 이용해 모듈 관련 파일(*.ko, *.o 등) 자동 정리
	$(MAKE) -C $(KDIR) M=$(PWD) clean

	# [User App] 2. 커널이 모르는 'app_test' 실행 파일은 직접 삭제
	rm -f app_test

	# [Legacy Way] 수업 시간에 사용했던 방식 (참고용 기록)
	# rm -f *.ko
	# rm -f *.o
	# rm -f *.mod.*
	# rm -f .*.cmd