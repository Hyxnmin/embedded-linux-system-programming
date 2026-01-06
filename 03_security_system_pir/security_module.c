#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/interrupt.h>
#include <linux/gpio.h>
#include <linux/timer.h>

#define HIGH 1
#define LOW 0
#define GPIO 7  

// 핀 설정
int led[4] = { 23, 24, 25, 1 };
int sw[4] = { 4, 17, 27, 22 };

// 타이머 및 상태 전역 변수 
static struct timer_list timer;
static int alarm_status = LOW; // 알람 활성화 상태
static int led_status = LOW;   // LED 점멸 상태

// 타이머 콜백 2초 간격으로 호출
static void timer_cb(struct timer_list *timer) {
    int i;
    // 알람이 활성화된 경우에만 동작
    if (alarm_status == HIGH) {
        led_status = !led_status;

        for(i = 0; i < 4; i++) {
            gpio_direction_output(led[i], led_status);
        }

        timer->expires = jiffies + HZ * 2;
        add_timer(timer);
    }
}

// PIR 인터럽트 핸들러: 물체 감지 시 동작
irqreturn_t pir_irq_handler(int irq, void *dev_id) {
    int i;
    // 알람이 꺼져있을 때만 동작 시작
    if (alarm_status == LOW) {
        printk(KERN_INFO "PIR Detected! Alarm Start.\n");
        alarm_status = HIGH;
        led_status = HIGH;
        
        // 즉시 모든 LED 켜기 
        for(i = 0; i < 4; i++) {
            gpio_direction_output(led[i], led_status);
        }
        
        timer.expires = jiffies + HZ * 2;
        add_timer(&timer);
    }
    return IRQ_HANDLED;
}

// 스위치 인터럽트 핸들러: 스위치 누르면 동작 
irqreturn_t sw_irq_handler(int irq, void *dev_id) {
    int i;
    // 알람이 켜져있을 때만 동작 (알람 종료)
    if (alarm_status == HIGH) {
        printk(KERN_INFO "Switch Pressed! Alarm Stop.\n");
        alarm_status = LOW;
        
        del_timer(&timer);

        // 모든 LED 끄기 
        for(i = 0; i < 4; i++) {
            gpio_direction_output(led[i], LOW);
        }
    }
    return IRQ_HANDLED;
}

static int assign3_init(void) {
    int res, i;
    printk(KERN_INFO "Assign3 Init!\n");

    // LED GPIO 요청 및 초기화 
    for(i = 0; i < 4; i++) {
        res = gpio_request(led[i], "LED");
        gpio_direction_output(led[i], LOW); // 초기 상태 OFF
        if (res < 0) 
            printk(KERN_INFO "LED gpio_request failed\n");
    }

    // PIR GPIO 요청 및 인터럽트 등록 
    res = gpio_request(GPIO, "PIR");
    res = request_irq(gpio_to_irq(GPIO), (irq_handler_t)pir_irq_handler,
        IRQF_TRIGGER_FALLING, "PIR_IRQ", (void *)(pir_irq_handler));
    if (res < 0)
        printk(KERN_INFO "PIR gpio_request failed\n");
    
    // Switch GPIO 요청 및 인터럽트 등록 
    for(i = 0; i < 4; i++) {
        res = gpio_request(sw[i], "SW");
        res = request_irq(gpio_to_irq(sw[i]), (irq_handler_t)sw_irq_handler, 
            IRQF_TRIGGER_RISING, "SW_IRQ", (void *)(sw_irq_handler));
        if (res < 0) {
            printk(KERN_INFO "Switch gpio_request failed\n");
        }
    }

    // 타이머 초기화 
    timer_setup(&timer, timer_cb, 0);

    return 0;
}

static void assign3_exit(void) {
    int i;
    printk(KERN_INFO "Assign3 Exit!\n");
    // 타이머 제거 
    del_timer(&timer);

    // LED 끄기 및 GPIO 해제 
    for(i = 0; i < 4; i++) {
        gpio_direction_output(led[i], LOW);
        gpio_free(led[i]);
    }

    // PIR 인터럽트 및 GPIO 해제 
    free_irq(gpio_to_irq(GPIO), (void *)(pir_irq_handler));
    gpio_free(GPIO);

    // Switch 인터럽트 및 GPIO 해제 
    for(i = 0; i < 4; i++) {
        free_irq(gpio_to_irq(sw[i]), (void *)(sw_irq_handler));
        gpio_free(sw[i]);
    }
}

module_init(assign3_init);
module_exit(assign3_exit);
MODULE_LICENSE("GPL");