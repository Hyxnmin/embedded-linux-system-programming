CC = gcc
obj-m := kernel_module.o
KDIR := /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)
default:
	$(MAKE) -C $(KDIR) M=$(PWD) modules

clean:
	# [Standard Way] 커널 빌드 시스템을 이용한 깔끔한 정리
	$(MAKE) -C $(KDIR) M=$(PWD) clean
	
	# [Legacy Way] 수업 시간에 사용했던 방식 (참고용 기록)
	# rm -f *.ko
	# rm -f *.o
	# rm -f *.mod.*
	# rm -f .*.cmd