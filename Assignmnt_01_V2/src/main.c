/*
 * main.c
 * Ví dụ sử dụng driver_gpio / driver_port
 *
 * Mục tiêu:
 *  - Cấu hình BUTTON1, BUTTON2 là input với pull-up và interrupt on falling edge
 *  - Cấu hình LED_RED, LED_GREEN là output
 *  - Đăng ký callback Button_Event để xử lý khi nút được nhấn
 *
 * Lưu ý:
 *  - Driver_GPIO0, Button_Event, và các định nghĩa BUTTON1/LED_RED/... nằm trong
 *    driver_gpio.h / driver_gpio.c như bạn đã cung cấp.
 *  - Nếu cần khởi tạo hệ thống (SystemInit) hoặc clock, thêm các gọi thích hợp
 *    trước khi sử dụng driver.
 */

#include <stdint.h>
#include <stdbool.h>
#include "driver_gpio.h"
#include "driver_port.h" /* chỉ để chắc có các kiểu/macro nếu cần */

/* Độ trễ đơn giản (busy-wait). Thay bằng HAL/Systick nếu có. */
static void delay_ms(uint32_t ms) {
    volatile uint32_t outer, inner;
    while (ms--) {
        for (outer = 0; outer < 1000u; ++outer) {
            /* vòng NOP để tiêu tốn thời gian; tùy CPU tốc độ, giá trị này cần hiệu chỉnh */
            for (inner = 0; inner < 50u; ++inner) {
                __asm volatile ("nop");
            }
        }
    }
}

int main(void) {
    int32_t res;

    /* --- Cấu hình LED làm output --- */
    res = Driver_GPIO0.SetDirection(LED_RED, ARM_GPIO_OUTPUT);
    if (res != ARM_DRIVER_OK) {
        /* Nếu lỗi, dừng ở đây (có thể thay bằng báo lỗi khác) */
        while (1) { __asm volatile("wfi"); }
    }

    res = Driver_GPIO0.SetDirection(LED_GREEN, ARM_GPIO_OUTPUT);
    if (res != ARM_DRIVER_OK) {
        while (1) { __asm volatile("wfi"); }
    }

    /* Tắt LED ban đầu (giả sử active high) */
    Driver_GPIO0.SetOutput(LED_RED, 0);
    Driver_GPIO0.SetOutput(LED_GREEN, 0);

    /* --- Cấu hình nút làm input + pull-up --- */
    /* Đăng ký callback khi setup: Driver_GPIO0.Setup sẽ đăng ký callback vào PORT driver */
    res = Driver_GPIO0.SetPullResistor(BUTTON1, ARM_GPIO_PULL_UP);
    if (res != ARM_DRIVER_OK) { while (1) { __asm volatile("wfi"); } }

    res = Driver_GPIO0.SetPullResistor(BUTTON2, ARM_GPIO_PULL_UP);
    if (res != ARM_DRIVER_OK) { while (1) { __asm volatile("wfi"); } }

    /* Setup pin: truyền callback Button_Event (được định nghĩa trong driver_gpio.c) */
    res = Driver_GPIO0.Setup(BUTTON1, Button_Event);
    if (res != ARM_DRIVER_OK) { while (1) { __asm volatile("wfi"); } }

    res = Driver_GPIO0.Setup(BUTTON2, Button_Event);
    if (res != ARM_DRIVER_OK) { while (1) { __asm volatile("wfi"); } }

    /* Chọn trigger interrupt: falling edge (khi dùng pull-up, nhấn nút kéo xuống 0) */
    res = Driver_GPIO0.SetEventTrigger(BUTTON1, ARM_GPIO_TRIGGER_FALLING_EDGE);
    if (res != ARM_DRIVER_OK) { while (1) { __asm volatile("wfi"); } }

    res = Driver_GPIO0.SetEventTrigger(BUTTON2, ARM_GPIO_TRIGGER_FALLING_EDGE);
    if (res != ARM_DRIVER_OK) { while (1) { __asm volatile("wfi"); } }

    /* --- Vòng lặp chính --- */
    for (;;) {
        /* Ở đây không cần làm gì vì callback xử lý nhấn nút.
           Thêm một hoạt động nền nhỏ để dễ debug (nháy LED đôi làm heartbeat) */

        /* Nháy LED_RED nhanh để biết chương trình chạy (không can thiệp quá nhiều vào trạng thái do nút điều khiển) */
        Driver_GPIO0.SetOutput(LED_RED, 1);
        delay_ms(100);
        Driver_GPIO0.SetOutput(LED_RED, 0);
        delay_ms(900);
    }

    /* không bao giờ tới đây */
    // return 0;
}
