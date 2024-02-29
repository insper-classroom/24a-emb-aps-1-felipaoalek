/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */


#include "hardware/gpio.h"
#include "pico/stdlib.h"
#include <stdio.h>
#include <math.h>

const int BTN_PIN_RED = 13;
const int BTN_PIN_GREEN = 12;
const int BTN_PIN_BLUE = 10;
const int BTN_PIN_YELLOW = 11;
const int BUZZ_PIN = 18;
volatile int foi_red = 0;
volatile int foi_green = 0;
volatile int foi_blue = 0;
volatile int foi_yellow = 0;
volatile int freq_green = 1000;
volatile int freq_red = 6000;
volatile int freq_blue = 4000;
volatile int freq_yellow = 2000;

void btn_callback(uint gpio, uint32_t events) {
   
  if (events == 0x8){
    if (gpio == BTN_PIN_GREEN) {
      foi_green = 1;
    } else if(gpio == BTN_PIN_RED){
      foi_red = 1;  
    } else if (gpio == BTN_PIN_BLUE){
      foi_blue = 1;
    } else if (gpio == BTN_PIN_YELLOW){
      foi_yellow = 1;
    }
  }
}

void reproduz(double tempo, int freq, int pino){
  float periodo = (1.0/freq) * (float) pow(10,6);
  float s = (periodo / (float)2) ;
  int giro = (tempo*1000)/(periodo);
  int i = 0;
  while(i<=giro){
    gpio_put(pino, 1);
    sleep_us((int)s);
    gpio_put(pino, 0);
    sleep_us((int)s);
    i++;
  }

}

int main(){
  stdio_init_all();

  gpio_init(BTN_PIN_RED);
  gpio_set_dir(BTN_PIN_RED, GPIO_IN);
  gpio_pull_up(BTN_PIN_RED);

  gpio_init(BTN_PIN_GREEN);
  gpio_set_dir(BTN_PIN_GREEN, GPIO_IN);
  gpio_pull_up(BTN_PIN_GREEN);

  gpio_init(BTN_PIN_BLUE);
  gpio_set_dir(BTN_PIN_BLUE, GPIO_IN);
  gpio_pull_up(BTN_PIN_BLUE);

  gpio_init(BTN_PIN_YELLOW);
  gpio_set_dir(BTN_PIN_YELLOW, GPIO_IN);
  gpio_pull_up(BTN_PIN_YELLOW);

  gpio_init(BUZZ_PIN);
  gpio_set_dir(BUZZ_PIN, GPIO_OUT);

  // callback led r (first)
  gpio_set_irq_enabled_with_callback(BTN_PIN_RED, GPIO_IRQ_EDGE_RISE, true,
                                     &btn_callback);

  gpio_set_irq_enabled_with_callback(BTN_PIN_GREEN, GPIO_IRQ_EDGE_RISE, true,
                                     &btn_callback);

  gpio_set_irq_enabled_with_callback(BTN_PIN_BLUE, GPIO_IRQ_EDGE_RISE, true,
                                      &btn_callback); 

  gpio_set_irq_enabled_with_callback(BTN_PIN_YELLOW, GPIO_IRQ_EDGE_RISE, true,
                                      &btn_callback);

  // callback led g (nao usar _with_callback)
  while (true) {
    if (foi_green == 1) {
      printf("green");
      reproduz(300,freq_green,18);
      foi_green = 0;
    } else if (foi_red == 1) {
      printf("red");
      reproduz(300,freq_red,18);
      foi_red = 0;
    } else if (foi_blue == 1) {
      printf("blue");
      reproduz(300,freq_blue,18);
      foi_blue = 0;
    } else if (foi_yellow == 1) {
      printf("yellow");
      reproduz(300,freq_yellow,18);
      foi_yellow = 0;
    }

  }
}