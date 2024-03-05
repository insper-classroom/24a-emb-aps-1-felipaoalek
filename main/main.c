/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */


#include "hardware/gpio.h"
#include "pico/stdlib.h"
#include <stdio.h>
#include <math.h>

const int BTN_PIN_RED = 12;
const int BTN_PIN_GREEN = 13;
const int BTN_PIN_BLUE = 10;
const int BTN_PIN_YELLOW = 11;
const int LED_PIN_RED = 6;
const int LED_PIN_GREEN = 8;
const int LED_PIN_BLUE = 7;
const int LED_PIN_YELLOW = 9;

const int BUZZ_PIN = 18;
volatile int foi_red = 0;
volatile int foi_green = 0;
volatile int foi_blue = 0;
volatile int foi_yellow = 0;

volatile int led_red = 0;
volatile int led_green = 0;
volatile int led_blue = 0;
volatile int led_yellow = 0;

volatile int freq_green = 1000;
volatile int freq_red = 6000;
volatile int freq_blue = 4000;
volatile int freq_yellow = 2000;
volatile int ledSelecionado = 0;
volatile int jogo = 1;
volatile int escolheu = 1;
volatile int rodada = 0;

volatile int possiveis[4] = {0, 1, 2, 3};
volatile int escolhido[100];
volatile int selecionados[100];
volatile int recorde =0;

void reproduz(double tempo, int freq, int pino, int led_pino){
  float periodo = (1.0/freq) * (float) pow(10,6);
  float s = (periodo / (float)2) ;
  int giro = (tempo*1000)/(periodo);
  int i = 0;
  while(i<=giro){
    gpio_put(pino, 1);
    sleep_us((int)s);
    gpio_put(pino, 0);
    sleep_us((int)s);

    gpio_put(led_pino, 1);
    i++;
  }
    sleep_ms(100);
    gpio_put(led_pino, 0);

}

int calcula_tamanho(volatile int v[]){
  int i = 0;
  while(v[i] != 5){
    i++;
  }
  return i;
}

void erro(double tempo, int freq, int pino){
  float periodo = (1.0/freq) * (float) pow(10,6);
  float s = (periodo / (float)2) ;
  int giro = (tempo*1000)/(periodo);
  int i = 0;
  while(i<=giro){
    gpio_put(pino, 1);
    sleep_us((int)s);
    gpio_put(pino, 0);
    sleep_us((int)s);

    gpio_put(LED_PIN_RED ,1);
    gpio_put(LED_PIN_GREEN ,1);
    gpio_put(LED_PIN_BLUE, 1);
    gpio_put(LED_PIN_YELLOW ,1);

    i++;
  }
    sleep_ms(100);
    gpio_put(LED_PIN_RED ,0);
    gpio_put(LED_PIN_GREEN ,0);
    gpio_put(LED_PIN_BLUE ,0);
    gpio_put(LED_PIN_YELLOW ,0);
    jogo = 0;
}


void escolheLEDaleatorio(int i) {
    srand(time(NULL));
    int ledAleatorio = srand() % 4; // Gera um número entre 0 e 3
    escolhido[i] = ledAleatorio;
    ledSelecionado = ledAleatorio;
}

void reproduzLedsAleatorios(){
    for (int i = 0; i < 100; i++) {
        if (escolhido[i] == 0){
            reproduz(700, freq_green, BUZZ_PIN, LED_PIN_GREEN);
        } else if (escolhido[i] == 1){
            reproduz(700, freq_red, BUZZ_PIN, LED_PIN_RED);
        }else if (escolhido[i] == 2){
            reproduz(700, freq_blue, BUZZ_PIN, LED_PIN_BLUE);
        }else if (escolhido[i] == 3){
            reproduz(700, freq_yellow, BUZZ_PIN, LED_PIN_YELLOW);
        }
    }
}

void print_lista(volatile int v[]){
    for (int i = 0; i < 100; i++) {
        printf("%d ,", v[i]);    
    }
    printf("\n");
}

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


int main(){
  stdio_init_all();

  for (int i = 0; i < 100; i++) {
        escolhido[i] = 5;
        selecionados[i] = 5;
    }

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


  gpio_init(LED_PIN_RED);
  gpio_set_dir(LED_PIN_RED, GPIO_OUT);

  gpio_init(LED_PIN_GREEN);
  gpio_set_dir(LED_PIN_GREEN, GPIO_OUT);

  gpio_init(LED_PIN_BLUE);
  gpio_set_dir(LED_PIN_BLUE, GPIO_OUT);

  gpio_init(LED_PIN_YELLOW);
  gpio_set_dir(LED_PIN_YELLOW, GPIO_OUT);

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

  
  while (jogo ==1) {
    for (int i = 0; i < 100; i++) {
        if (calcula_tamanho(escolhido) == calcula_tamanho(selecionados)){
          selecionados[i] = 5;
        }
    }

    if (escolheu == 1){
    escolheLEDaleatorio(rodada);
    reproduzLedsAleatorios();
    escolheu = 0;
    }
    rodada = 0;

    while(calcula_tamanho(escolhido) > calcula_tamanho(selecionados)){
    
      if (foi_green == 1) {
        escolheu = 1;
        selecionados[rodada] = 0;
        printf("Selecionados: ");
        print_lista(selecionados);
        printf("green\n");
        printf("%d\n", selecionados[rodada]);
        printf("%d\n", escolhido[rodada]);
        reproduz(300,freq_green,18, LED_PIN_GREEN);
        sleep_ms(500);
        foi_green = 0;
        if ((escolhido[rodada] != selecionados[rodada])&&(selecionados[rodada] != 5)){
            erro(600, 180, BUZZ_PIN);
            jogo = 0;
        }
        rodada++;
      } else if (foi_red == 1) {
        escolheu = 1;
        selecionados[rodada] = 1;
        printf("Selecionados: ");
        print_lista(selecionados);
        printf("Escolhidos: ");
        print_lista(escolhido);
        printf("red\n");
        printf("%d\n", selecionados[rodada]);
        printf("%d\n", escolhido[rodada]);
        reproduz(300,freq_red,18, LED_PIN_RED);
        sleep_ms(500);
        foi_red = 0;
        if ((escolhido[rodada] != selecionados[rodada])&&(selecionados[rodada] != 5)){
            erro(600, 180, BUZZ_PIN);
            jogo = 0;
        }
        rodada++;
      } else if (foi_blue == 1) {
        escolheu = 1;
        selecionados[rodada] = 2;
        printf("Selecionados: ");
        print_lista(selecionados);
        printf("Escolhidos: ");
        print_lista(escolhido);
        printf("blue\n");
        printf("%d\n", selecionados[rodada]);
        printf("%d\n", escolhido[rodada]);
        reproduz(300,freq_blue,18, LED_PIN_BLUE);
        sleep_ms(500);
        foi_blue = 0;
        if ((escolhido[rodada] != selecionados[rodada])&&(selecionados[rodada] != 5)){
            erro(600, 180, BUZZ_PIN);
            jogo = 0;
        }
        rodada++;
      } else if (foi_yellow == 1) {
        escolheu = 1;
        selecionados[rodada] = 3;
        printf("Selecionados: ");
        print_lista(selecionados);
        printf("Escolhidos: ");
        print_lista(escolhido);
        printf("yellow\n");
        printf("%d\n", selecionados[rodada]);
        printf("%d\n", escolhido[rodada]);
        reproduz(300,freq_yellow,18, LED_PIN_YELLOW);
        sleep_ms(500);
        foi_yellow = 0;
        if ((escolhido[rodada] != selecionados[rodada])&&(selecionados[rodada] != 5)){
            erro(600, 180, BUZZ_PIN);
            jogo = 0;
        }
        rodada++;
        if (rodada > recorde){
          recorde = rodada;
        }
      } 
    }
  }
}
