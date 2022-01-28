/**
 * @file    main.c
 * @brief   4 digits 7 segments display for EFM32GG_STK3700
 * @version 1.0
 *
 * @note    7 segments display with 4 digits
 *
 * @note    LEDs are on pins 0 to 8 of GPIO Port D
 *
 * @note    It uses a primitive delay mechanism.
 *
 * @author  Henrique Faria Ribeiro / Henrique Paulino Cruz
 * @date    28/01/2022
 */

#include <stdint.h>
#include "em_device.h"

/**
 * @brief       BIT macro
 *              Defines a constant (hopefully) with a bit 1 in the N position
 * @param       N : Bit index with bit 0 the Least Significant Bit
 */
#define BIT(N) (1U << (N))
#define MASK_ZERO_TO_SEVEN_PIN_PUSHPULL 0x04444444
#define MASK_ZERO_TO_THREE_PIN_PUSHPULL 0x00004444
#define MASK_INTERNAL_BUTTONS_INPUT 0x00000110

#define LED1 (BIT(2))
#define LED2 (BIT(3))

/// Default delay value.
#define DELAYVAL 3
/**
 * @brief  Quick and dirty delay function
 * @note   Do not use it in production code
 */
void delay(uint32_t delay) {
    volatile uint32_t counter;
    int i;

    for (i = 0; i < delay; i++) {
        counter = 100000;
        while (counter)
            counter--;
    }
}

//typedef GPIO_t (GPIO_P_TypeDef *);

static GPIO_P_TypeDef * const GPIOB = &(GPIO->P[1]);    // GPIOB
static GPIO_P_TypeDef * const GPIOC = &(GPIO->P[2]);    // GPIOC
static GPIO_P_TypeDef * const GPIOD = &(GPIO->P[3]);    // GPIOD
static GPIO_P_TypeDef * const GPIOE = &(GPIO->P[4]);    // GPIOE


/*
1 -> bc         -> 00000110
2 -> abdeg      -> 01011011
3 -> abcdg      -> 01001111
4 -> bcfg       -> 01100110
5 -> acdfg      -> 01101101
6 -> acdefg     -> 01111101
7 -> abc        -> 00000111
8 -> abcdefg    -> 01111111
9 -> abcdfg     -> 01101111
0 -> abcdef     -> 00111111
*/
static const int hex[10] = {
    0x00111111, // 0
    0x00000110, // 1
    0x01011011, // 2
    0x01001111, // 3
    0x01100110, // 4
    0x01101101, // 5
    0x01111101, // 6
    0x00000111, // 7
    0x01111111, // 8
    0x01101111  // 9
};

#define INT_TO_SEG(N) (hex[N])

void show_segments(int val_delay, int *seg) {
    int num_digits = 0;
    
    for(int i = 0; i < 4; i++) {
        if(seg[num_digits] < 0) break;
        num_digits++;
    }
    
    for(int i = 0; i < val_delay*30000; i++) {
        for(int j = 0; j < num_digits; j++) {
            // display logics
            GPIOC->DOUT = BIT(j);               // choose the digit
            GPIOD->DOUT = INT_TO_SEG(seg[j]);   // diplay the number
        }
    }
}

void vet_segments(int num, int *seg) {
    int aux = num;
    int seg_it = 0;
    
    while (aux > 0) {
        seg[seg_it] = aux % 10;
        aux /= 10;
        seg_it++;
    }
    
    while (seg_it < 4) {
        seg[seg_it] = 0;
        seg_it++;
    }
}

void stopwatch(int val_delay, int *seg) {
    int count = 0;
    while(count <= 9999) {
        vet_segments(count, seg);
        show_segments(val_delay, seg);
        count++;
    }
}

void hour_format_24(int val_delay, int *seg){
    for(int hour = 0; hour <= 24; hour++) {
        for(int min = 0; min <= 60; min++) {
            int num = hour*100;
            num += min;
            vet_segments(num, seg);
            show_segments(val_delay, seg);
        }
    }
}

void hour_format_AMPM(int val_delay, int *seg){
    for(int hour = 0; hour <= 24; hour++) {
        for(int min = 0; min <= 60; min++) {
            int aux = (hour == 12 ? hour : (hour%12));
            int num = aux*100;
            num += min;
            vet_segments(num, seg);
            show_segments(val_delay, seg);
            if(hour > 12)
                GPIOE->DOUT = (LED2 & ~LED1);   // turn on PM LD
            else
                GPIOE->DOUT = (LED1 & ~LED2);   // turn on PM LD
        }
    }
}

/**
 * @brief  Main function
 *
 * @note   Using default clock configuration
 * @note   HFCLK     = HFRCO 14 MHz
 * @note   HFCORECLK = HFCLK
 * @note   HFPERCLK  = HFCLK

 */

int main(void) {
    /* Enable Clock for GPIO */
    CMU->HFPERCLKDIV |= CMU_HFPERCLKDIV_HFPERCLKEN; // Enable HFPERCLK
    CMU->HFPERCLKEN0 |= CMU_HFPERCLKEN0_GPIO;       // Enable HFPERCKL for GPIO

    /* Configure Pins in GPIOD */
    GPIOB->MODEH = MASK_INTERNAL_BUTTONS_INPUT; // Set bits
    GPIOC->MODEL = MASK_ZERO_TO_THREE_PIN_PUSHPULL; // Set bits
    GPIOD->MODEL = MASK_ZERO_TO_SEVEN_PIN_PUSHPULL; // Set bits
    GPIOE->MODEL = (GPIO_P_MODEL_MODE2_PUSHPULL|GPIO_P_MODEL_MODE3_PUSHPULL);

    /* Initial values for LEDs */
    GPIOD->DOUT = 0U; // Turn Off LEDs
    GPIOC->DOUT = 0U; // Turn Off LEDs

    int seg[4];
    int val_delay = 2;
    /* loop */
    while (1) {
        //stopwatch(val_delay, seg);
        //hour_format_24(val_delay, seg);
        //format_AMPM(val_delay, seg);
    }
}