/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

//https://github.com/raspberrypi/pico-examples/blob/master/i2c/lcd_1602_i2c/lcd_1602_i2c.c

#include "hardware/gpio.h"
#include "pico/stdlib.h"
#include <stdio.h>
#include <math.h>

const int BTN_PIN_RED = 12;
const int BTN_PIN_GREEN = 13;
const int BTN_PIN_BLUE = 9;     
const int BTN_PIN_YELLOW = 11;

const int BTN_PIN_RED_2 = 28;
const int BTN_PIN_GREEN_2 = 17;
const int BTN_PIN_BLUE_2 = 26;
const int BTN_PIN_YELLOW_2 = 27;

const int BTN_PIN_START = 14;

const int BTN_PIN_ONE_PLAYER = 15;
const int BTN_PIN_TWO_PLAYERS = 16;

const int LED_PIN_RED = 6;
const int LED_PIN_GREEN = 8;
const int LED_PIN_BLUE = 7;
const int LED_PIN_YELLOW = 5;

const int LED_PIN_RED_2 = 20;
const int LED_PIN_GREEN_2 = 21;
const int LED_PIN_BLUE_2 = 22;
const int LED_PIN_YELLOW_2 = 19;

const int LED_PIN_RECORD = 2;

const int BUZZ_PIN = 18;

volatile int Ligado = 1;

int jogador_atual = 1;

volatile int foi_red = 0;
volatile int foi_green = 0;
volatile int foi_blue = 0;
volatile int foi_yellow = 0;

volatile int foi_red_2 = 0;
volatile int foi_green_2 = 0;
volatile int foi_blue_2 = 0;
volatile int foi_yellow_2 = 0;

volatile int led_red = 0;
volatile int led_green = 0;
volatile int led_blue = 0;
volatile int led_yellow = 0;

volatile int led_red_2 = 0;
volatile int led_green_2 = 0;
volatile int led_blue_2 = 0;
volatile int led_yellow_2 = 0;

volatile int freq_green = 1000;
volatile int freq_red = 6000;
volatile int freq_blue = 4000;
volatile int freq_yellow = 2000;

int ledSelecionado = 0;
int jogo = 0;
int escolheu = 1;
int rodada = 0;
int one = 0;
int two = 0;
int l = 0;

volatile int possiveis[4] = {0, 1, 2, 3};
int escolhido[100];
int selecionados[100];
int recorde =0;
int x=0;


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

void musica_inicio() {
    reproduz(300, 494, BUZZ_PIN, LED_PIN_YELLOW); // B
    reproduz(300, 587, BUZZ_PIN, LED_PIN_GREEN);   // D
    reproduz(300, 659, BUZZ_PIN, LED_PIN_RED_2);  // E
    reproduz(300, 587, BUZZ_PIN, LED_PIN_BLUE_2); // D
    reproduz(300, 659, BUZZ_PIN, LED_PIN_YELLOW_2); // E
    reproduz(300, 784, BUZZ_PIN, LED_PIN_GREEN_2);   // G
    reproduz(300, 880, BUZZ_PIN, LED_PIN_RED);  // A
    reproduz(300, 880, BUZZ_PIN, LED_PIN_BLUE);  // A
    sleep_ms(500);
}

void musica_final() {
    reproduz(500, 392, BUZZ_PIN, LED_PIN_GREEN); // G
    reproduz(500, 330, BUZZ_PIN, LED_PIN_RED);   // E
    reproduz(500, 294, BUZZ_PIN, LED_PIN_BLUE);  // D
    reproduz(500, 262, BUZZ_PIN, LED_PIN_YELLOW); // C
    sleep_ms(500);
}


int calcula_tamanho(const volatile int v[]){
  int i = 0;
  while(v[i] != 5){
    i++;
  }
  return i;
}

void erro(double tempo, int freq, int pino, int x, int recorde){
  float periodo = (1.0/freq) * (float) pow(10,6);
  float s = (periodo / (float)2) ;
  int giro = (tempo*1000)/(periodo);
  int i = 0;
  //Errou, Recorde
  printf("X: %d\n", x);
  if (x > recorde){
  recorde = x;
  }
  printf("Errou, Recorde: %d\n", recorde);
  musica_final();

  while(i<=giro){

    gpio_put(pino, 1);
    sleep_us((int)s);
    gpio_put(pino, 0);
    sleep_us((int)s);



    gpio_put(LED_PIN_RED ,1);
    gpio_put(LED_PIN_GREEN ,1);
    gpio_put(LED_PIN_BLUE, 1);
    gpio_put(LED_PIN_YELLOW ,1);
    gpio_put(LED_PIN_RED_2 ,1);
    gpio_put(LED_PIN_GREEN_2 ,1);
    gpio_put(LED_PIN_BLUE_2, 1);
    gpio_put(LED_PIN_YELLOW_2 ,1);

    i++;

  }
    sleep_ms(100);
    gpio_put(LED_PIN_RED ,0);
    gpio_put(LED_PIN_GREEN ,0);
    gpio_put(LED_PIN_BLUE ,0);
    gpio_put(LED_PIN_YELLOW ,0);
    gpio_put(LED_PIN_RED_2 ,0);
    gpio_put(LED_PIN_GREEN_2 ,0);
    gpio_put(LED_PIN_BLUE_2 ,0);
    gpio_put(LED_PIN_YELLOW_2 ,0);
    jogo = 0;
    piscando_recorde(recorde);
}

void piscando_recorde(int recorde){
  for (int i = 0; i < recorde; i++) {
    reproduz(200, freq_green, BUZZ_PIN, LED_PIN_RECORD);
    sleep_ms(500);
  }
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

void reproduzLedsAleatorios2(){
    for (int i = 0; i < 100; i++) {
        if (escolhido[i] == 0){
            reproduz(700, freq_green, BUZZ_PIN, LED_PIN_GREEN_2);
        } else if (escolhido[i] == 1){
            reproduz(700, freq_red, BUZZ_PIN, LED_PIN_RED_2);
        }else if (escolhido[i] == 2){
            reproduz(700, freq_blue, BUZZ_PIN, LED_PIN_BLUE_2);
        }else if (escolhido[i] == 3){
            reproduz(700, freq_yellow, BUZZ_PIN, LED_PIN_YELLOW_2);
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
    else if (gpio == BTN_PIN_RED_2) {
      foi_red = 1;
    } else if(gpio == BTN_PIN_GREEN_2){
      foi_green = 1;
    } else if (gpio == BTN_PIN_BLUE_2){
      foi_blue = 1;
    } else if (gpio == BTN_PIN_YELLOW_2){
      foi_yellow = 1;
    }
      else if(gpio == BTN_PIN_START){
      jogo = 1;
    }
      else if(gpio == BTN_PIN_ONE_PLAYER){
      one = 1;
    }
      else if(gpio == BTN_PIN_TWO_PLAYERS){
      two = 1;
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

  gpio_init(BTN_PIN_RED_2);
  gpio_set_dir(BTN_PIN_RED_2, GPIO_IN);
  gpio_pull_up(BTN_PIN_RED_2);

  gpio_init(BTN_PIN_GREEN_2);
  gpio_set_dir(BTN_PIN_GREEN_2, GPIO_IN);
  gpio_pull_up(BTN_PIN_GREEN_2);

  gpio_init(BTN_PIN_BLUE_2);
  gpio_set_dir(BTN_PIN_BLUE_2, GPIO_IN);
  gpio_pull_up(BTN_PIN_BLUE_2);

  gpio_init(BTN_PIN_YELLOW_2);
  gpio_set_dir(BTN_PIN_YELLOW_2, GPIO_IN);
  gpio_pull_up(BTN_PIN_YELLOW_2);

  gpio_init(BTN_PIN_START);
  gpio_set_dir(BTN_PIN_START, GPIO_IN);
  gpio_pull_up(BTN_PIN_START);

  gpio_init(BTN_PIN_ONE_PLAYER);
  gpio_set_dir(BTN_PIN_ONE_PLAYER, GPIO_IN);
  gpio_pull_up(BTN_PIN_ONE_PLAYER);

  gpio_init(BTN_PIN_TWO_PLAYERS);
  gpio_set_dir(BTN_PIN_TWO_PLAYERS, GPIO_IN);
  gpio_pull_up(BTN_PIN_TWO_PLAYERS);

  gpio_init(LED_PIN_RED);
  gpio_set_dir(LED_PIN_RED, GPIO_OUT);

  gpio_init(LED_PIN_GREEN);
  gpio_set_dir(LED_PIN_GREEN, GPIO_OUT);

  gpio_init(LED_PIN_BLUE);
  gpio_set_dir(LED_PIN_BLUE, GPIO_OUT);

  gpio_init(LED_PIN_YELLOW);
  gpio_set_dir(LED_PIN_YELLOW, GPIO_OUT);

  gpio_init(LED_PIN_RED_2);
  gpio_set_dir(LED_PIN_RED_2, GPIO_OUT);

  gpio_init(LED_PIN_GREEN_2);
  gpio_set_dir(LED_PIN_GREEN_2, GPIO_OUT);

  gpio_init(LED_PIN_BLUE_2);
  gpio_set_dir(LED_PIN_BLUE_2, GPIO_OUT);

  gpio_init(LED_PIN_YELLOW_2);
  gpio_set_dir(LED_PIN_YELLOW_2, GPIO_OUT);

  gpio_init(LED_PIN_RECORD);
  gpio_set_dir(LED_PIN_RECORD, GPIO_OUT);

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

  gpio_set_irq_enabled_with_callback(BTN_PIN_RED_2, GPIO_IRQ_EDGE_RISE, true,
                                      &btn_callback); 

  gpio_set_irq_enabled_with_callback(BTN_PIN_GREEN_2, GPIO_IRQ_EDGE_RISE, true,
                                      &btn_callback); 

  gpio_set_irq_enabled_with_callback(BTN_PIN_BLUE_2, GPIO_IRQ_EDGE_RISE, true,
                                      &btn_callback); 

  gpio_set_irq_enabled_with_callback(BTN_PIN_YELLOW_2, GPIO_IRQ_EDGE_RISE, true,
                                      &btn_callback);

  gpio_set_irq_enabled_with_callback(BTN_PIN_START, GPIO_IRQ_EDGE_RISE, true,
                                      &btn_callback);

  gpio_set_irq_enabled_with_callback(BTN_PIN_ONE_PLAYER, GPIO_IRQ_EDGE_RISE, true,
                                      &btn_callback);

  gpio_set_irq_enabled_with_callback(BTN_PIN_TWO_PLAYERS, GPIO_IRQ_EDGE_RISE, true,
                                      &btn_callback);

  // callback led g (nao usar _with_callback)

  while (Ligado==1) {

    //Mostrando recorde
    printf("Recorde: %d\n", recorde);

    if(recorde > 0){
      piscando_recorde(recorde);
    }

    //Fazendo reestart do jogo
    if (jogo == 0){
      for (int i = 0; i < 100; i++) {
        escolhido[i] = 5;
        selecionados[i] = 5;
      }
      rodada = 0;
      one=0;
      two=0;
      x=0;
    }

  printf(("APERTE START PARA INICIAR O JOGO\n"));
  while(jogo == 0){
    l++;
  }
  musica_inicio();
  sleep_ms(100);
  printf("Jogo iniciado\n");

  printf("Escolha o modo de jogo\n");
  while(one == 0 && two == 0){
    l++;
  }

  while(jogo == 1){
    if(one == 1){
      reproduz(300, freq_green, BUZZ_PIN, LED_PIN_RECORD);
      printf("Modo de jogo: 1 jogador\n");
      break;
    } else if(two == 1){
      reproduz(300, freq_green, BUZZ_PIN, LED_PIN_RECORD);
      printf("Modo de jogo: 2 jogadores\n");
      break;
    }
    sleep_ms(500);
  }

  printf("Jogo iniciado\n");
  sleep_ms(1000);

  while (jogo ==1 && one == 1) {
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
            erro(600, 180, BUZZ_PIN, x, recorde);
            if (rodada > recorde){
              recorde = rodada;
            }
            jogo = 0;
            break;
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
            erro(600, 180, BUZZ_PIN, x, recorde);
            if (rodada > recorde){
              recorde = rodada;
            }
            jogo = 0;
            break;
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
            erro(600, 180, BUZZ_PIN, x, recorde);
            if (rodada > recorde){
              recorde = rodada;
            }
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
            erro(600, 180, BUZZ_PIN, x, recorde);
            if (rodada > recorde){
              recorde = rodada;
            }
            jogo = 0;
            break;
        }
        rodada++;

      } 
    }
    x++;
    printf("X: %d\n", x);
  }

  while(jogo == 1 && two == 1){

    if(jogador_atual == 1){
      printf("Jogador 1\n");
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
            erro(600, 180, BUZZ_PIN, x, recorde);
              jogo = 0;
              break;
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
            erro(600, 180, BUZZ_PIN, x, recorde);
              jogo = 0;
              break;
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
            erro(600, 180, BUZZ_PIN, x, recorde);
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
            erro(600, 180, BUZZ_PIN, x, recorde);
              jogo = 0;
              break;
          }
          rodada++;

        } 
      }
    } else {
        printf("Jogador 2\n");
          for (int i = 0; i < 100; i++) {
          if (calcula_tamanho(escolhido) == calcula_tamanho(selecionados)){
            selecionados[i] = 5;
          }
      }

      if (escolheu == 1){
      escolheLEDaleatorio(rodada);
      reproduzLedsAleatorios2();
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
          reproduz(300,freq_green,18, LED_PIN_GREEN_2);
          sleep_ms(500);
          foi_green = 0;
          if ((escolhido[rodada] != selecionados[rodada])&&(selecionados[rodada] != 5)){
            erro(600, 180, BUZZ_PIN, x, recorde);
            if (rodada > recorde){
              recorde = rodada;
            }
              jogo = 0;
              break;
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
          reproduz(300,freq_red,18, LED_PIN_RED_2);
          sleep_ms(500);
          foi_red = 0;
          if ((escolhido[rodada] != selecionados[rodada])&&(selecionados[rodada] != 5)){
            erro(600, 180, BUZZ_PIN, x, recorde);
            if (rodada > recorde){
              recorde = rodada;
            }
              jogo = 0;
              break;
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
          reproduz(300,freq_blue,18, LED_PIN_BLUE_2);
          sleep_ms(500);
          foi_blue = 0;
          if ((escolhido[rodada] != selecionados[rodada])&&(selecionados[rodada] != 5)){
            erro(600, 180, BUZZ_PIN, x, recorde);
            if (rodada > recorde){
              recorde = rodada;
            }
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
          reproduz(300,freq_yellow,18, LED_PIN_YELLOW_2);
          sleep_ms(500);
          foi_yellow = 0;
          if ((escolhido[rodada] != selecionados[rodada])&&(selecionados[rodada] != 5)){
            erro(600, 180, BUZZ_PIN, x, recorde);
            if (rodada > recorde){
              recorde = rodada;
            }
              jogo = 0;
              break;
          }
          rodada++;
          if (rodada > recorde){
            recorde = rodada;
            printf("Recorde: %d\n", recorde);
          }
        } 
      }
      x++;
      printf("X: %d\n", x);
    }
    if(jogador_atual==1){
      jogador_atual = 2;
    }
    else{
      jogador_atual = 1;
    }
  }
  }
}

