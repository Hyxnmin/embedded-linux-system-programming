# 🐧 Embedded Linux System Programming

> **Exploring the interaction between Hardware and Kernel Space.**
> 리눅스 커널 모듈(LKM) 개발을 통해 운영체제의 리소스 관리, 인터럽트 처리, 그리고 User/Kernel Space 간의 통신 메커니즘을 깊이 있게 연구한 프로젝트 모음입니다.

![C](https://img.shields.io/badge/Language-C-blue?style=flat-square) ![Linux](https://img.shields.io/badge/Platform-Linux%20Kernel-black?style=flat-square&logo=linux) ![License](https://img.shields.io/badge/License-GPLv2-green?style=flat-square)

## 📂 Project Overview

이 저장소는 임베디드 리눅스 환경(Target: ARM Based Board)에서 **직접 디바이스 드라이버를 구현**하며 마주친 기술적 문제들과 해결 과정을 기록했습니다. 단순한 기능 구현을 넘어, **시스템의 안정성(Stability)과 효율성(Efficiency)**을 고려한 설계를 지향했습니다.

| Directory | Project | Key Concepts |
|:--- |:--- |:--- |
| `01_gpio_interrupt_timer` | **GPIO Control Module** | Interrupt Handling, Kernel Timer, Software Debouncing |
| `02_char_device_driver` | **Character Device Driver** | VFS(Virtual File System), System Call, User-Kernel Data Transfer |
| `03_security_system_pir` | **PIR Security System** | Event-Driven Architecture, Sensor Integration, State Machine |

---

## 🚀 Key Technical Challenges & Solutions

프로젝트 진행 중 발생한 주요 이슈와 이를 해결하기 위해 적용한 엔지니어링 접근 방식입니다.

### 1. Software Debouncing (in `01_gpio_interrupt_timer`)
* **Issue:** 기계식 스위치 조작 시 물리적 진동(Chattering)으로 인해 한 번의 입력에 수십 번의 인터럽트가 발생하는 현상 확인.
* **Solution:** 리눅스 커널의 시간 단위인 `jiffies`를 활용하여 디바운싱 로직을 구현. 마지막 인터럽트 발생 시점과 현재 시점의 차이가 **200ms 미만일 경우 노이즈로 간주하고 무시**하여 입력 신뢰성을 확보했습니다.
    ```c
    // Code Snippet: Debouncing Logic
    if (jiffies - last_irq_time < msecs_to_jiffies(200)) {
        return IRQ_HANDLED; // Ignore noise
    }
    last_irq_time = jiffies;
    ```

### 2. Concurrency Management with Kernel Timers
* **Approach:** `sleep()` 함수는 CPU를 점유하거나 프로세스를 차단(Block)할 위험이 있어 인터럽트 컨텍스트 내 사용이 부적절함.
* **Implementation:** 대신 비동기적으로 동작하는 `struct timer_list`를 사용하여, 메인 시스템의 흐름을 방해하지 않고 LED 점멸 패턴(Blink, Shift)을 제어하는 **Non-blocking 아키텍처**를 구현했습니다.

### 3. Safe User-Kernel Communication (in `02_char_device_driver`)
* **Principle:** User Space의 메모리 포인터를 Kernel Space에서 직접 참조할 경우, 잘못된 주소 접근으로 인한 **Kernel Panic** 위험이 있음.
* **Implementation:** `copy_from_user()`와 `copy_to_user()` 커널 함수를 사용하여 데이터 유효성을 검증한 뒤 안전하게 메모리를 복사하도록 구현했습니다.

---

## 🛠️ Build & Usage

표준 리눅스 커널 빌드 시스템(Kbuild)을 준수하여 `Makefile`을 구성했습니다.

### Prerequisites
* Linux Kernel Headers (`sudo apt install linux-headers-$(uname -r)`)
* GCC Compiler, Make

### How to Build
각 프로젝트 폴더로 이동하여 `make` 명령어를 실행하면, 현재 커널 버전을 자동으로 감지하여 모듈을 빌드합니다.

```bash
# Example: Build GPIO Module
cd 01_gpio_interrupt_timer
make

# Load Module
sudo insmod assign1.ko

# Check Kernel Log
dmesg | tail
```
## 📝 Learning Outcomes

1.  **Kernel Mechanics:** 커널 모듈의 생명주기(`init`, `exit`)와 커널 심볼 테이블에 대한 이해.
2.  **Resource Management:** `request_irq`, `gpio_request` 등을 통한 하드웨어 리소스 할당 및 해제와 메모리 누수 방지.
3.  **Low-Level Debugging:** `dmesg`와 커널 로그(`printk`)를 활용한 트러블 슈팅 능력.
