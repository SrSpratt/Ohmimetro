#include <stdio.h>
#include <stdlib.h>
#include <RVgeneral.h>
#include <RVSketches.h>
#define ADC_PIN 28  
#define Botao_A 5 
#define TOLERANCE_PERCENT 15

//int Rc = 334.35; // 330
int Rc = 990; // 1000 -> referência encontrada no multímetro
//int Rc = 221;

//Trecho para modo BOOTSEL com botão B
#include "pico/bootrom.h"   
#define botaoB 6
void gpio_irq_handler(uint gpio, uint32_t events)
{
  reset_usb_boot(0, 0);
}

int main()
{
  ssd1306_t display;

  gpio button_a = {
    .pin = Botao_A,
    .pin_dir = GPIO_IN
  };
  gpio button_b = {
    .pin = botaoB,
    .pin_dir = GPIO_IN
  };
  gpio g_pins[2] = {button_a, button_b};

  init_gpio(g_pins, 2);
  gpio_set_irq_enabled_with_callback(botaoB, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);

  init_interfaces();

  ssd1306_t ssd; // Inicializa a estrutura do display

  init_display(&ssd);

  adc ohmmeter = {
    .pin = 28,
    .channel = 2
  };

  adc a_pins[1] = {ohmmeter};
  config_adc(a_pins, 1);

    //armazena as informações da pio
  pio pio = {
      .pin = 7,
      .address = 0,
      .offset = 0,
      .state_machine = 0
  };

  //structs para manipular os desenhos na matriz de LEDs
  rgb main_color = {.red = 0.0, .green = 0.03, .blue = 0.0};
  rgb background_color = {.red = 0.0, .green = 0.0, .blue = 0.0};

  sketch sketch = {
      .figure = {
          0.0, 0.0, 0.0, 0.0, 0.0,
          0.0, 0.0, 0.0, 0.0, 0.0,
          0.0, 0.0, 0.0, 0.0, 0.0,
          0.0, 0.0, 0.0, 0.0, 0.0,
          0.0, 0.0, 0.0, 0.0, 0.0
      },
      .index = -1,
      .main_color = main_color,
      .background_color = background_color
  };
  config_pio(&pio);
  //draw(sketch, 0, pio, matrix);

  float r_x;
  char str_x[5];  // Buffer para armazenar a string
  char str_y[5]; // Buffer para armazenar a string
  
  bool cor = true;
  while (true)
  {
    adc_select_input(2); // Seleciona o ADC para eixo X. O pino 28 como entrada analógica

    float soma = 0.0f;
    for (int i = 0; i < 100000; i++) {
        soma += adc_read();
        sleep_us(1);
    }

    float media = soma / 100000.0f;
    r_x = (Rc * media) / (4095 - media); // Calcula a resistência real

    // Valores padrão da série E24
    const float standard_values[] = {
        10, 11, 12, 13, 15, 16, 18, 20, 22, 24, 27, 30,
        33, 36, 39, 43, 47, 51, 56, 62, 68, 75, 82, 91, 100
    };

    // Normaliza r_x para a faixa [10, 100] e calcula a potência
    int power = 0;
    float normalized_rx = r_x;
    while (normalized_rx >= 100.0f) {
        normalized_rx /= 10.0f;
        power++;
    }
    while (normalized_rx < 10.0f && power > 0) {
        normalized_rx *= 10.0f;
        power--;
    }

    // Encontra o valor mais próximo na série E24
    float min_diff = 1e9f; // Valor inicial grande para diferença mínima
    int best_index = 0;

    for (int i = 0; i < 25; i++) {
        float diff = normalized_rx - standard_values[i];
        // Calcula o valor absoluto manualmente (sem fabs)
        if (diff < 0) {
            diff = -diff; // Torna positivo se negativo
        }
        // Verifica se é a menor diferença encontrada até agora
        if (diff < min_diff) {
            min_diff = diff;
            best_index = i;
        }
    }

    int new_value = standard_values[best_index];

    // Ajusta se new_value == 100 (ex: 100 → 10 * 10^1)
    if (new_value == 100) {
        new_value = 10;
        power++;
    }

    // Extrai os dígitos para as faixas de cores
    int line[5] = {new_value / 10, 0, new_value % 10, 0, power};

    //Debug
    //printf("\ntemporiginal: %d temp: %d temp/10: %d temp%10: %d power: %d",r_x, new_value, line[0], line[2], line[4]);

    const char colors[10] = {'p', 'm', 'v', 'l', 'a','V', 'A', 'r', 'c', 'b'};
    const char pattern[8] = {colors[line[0]], ' ', colors[line[2]], ' ', colors[line[4]], ' ', '5', '\0'}; 

    double new_matrix[matrix];
    int j = 0;
    int row = 0;
    int col = 0;
    for (int i = 0; i < matrix; i++){
      row = i / 5;
      col = i % 5;
      if (row % 2 != 0)
        new_matrix[i] = (double) line[col];
      else 
        new_matrix[i] = (double) line[4-col];
    }

    vector_copy(sketch.figure, new_matrix, 5*5);
    draw(sketch,0, pio, matrix);
    
    sprintf(str_x, "%1.0f", media);  // Converte o inteiro em string
    sprintf(str_y, "%1.2f", r_x);  // Converte o float em string
    
    //cor = !cor;
    // Atualiza o conteúdo do display com animações
    ssd1306_fill(&ssd, !cor); // Limpa o display
    ssd1306_rect(&ssd, 3, 3, 122, 60, cor, !cor); // Desenha um retângulo
    ssd1306_line(&ssd, 3, 25, 123, 25, cor); // Desenha uma linha
    ssd1306_line(&ssd, 3, 37, 123, 37, cor); // Desenha uma linha   
    ssd1306_draw_string(&ssd, "CEPEDI   TIC37", 8, 6); // Desenha uma string
    //ssd1306_draw_string(&ssd, "EMBARCATECH", 20, 16); // Desenha uma string
    ssd1306_draw_string(&ssd, "  Ohmimetro", 10, 16); // Desenha uma string
    ssd1306_draw_string(&ssd, "P: ", 10, 28); // Desenha uma string 
    ssd1306_draw_string(&ssd, pattern, 30, 28); // Desenha uma string 
    ssd1306_draw_string(&ssd, "ADC", 13, 41); // Desenha uma string    
    ssd1306_draw_string(&ssd, "R(Ohms)", 50, 41); // Desenha uma strinhg    
    ssd1306_line(&ssd, 44, 37, 44, 60, cor); // Desenha uma linha vertical         
    ssd1306_draw_string(&ssd, str_x, 8, 52); // Desenha uma string     
    ssd1306_draw_string(&ssd, str_y, 64, 52); // Desenha uma string   
    ssd1306_send_data(&ssd); // Atualiza o display
    sleep_ms(700);
  }
}