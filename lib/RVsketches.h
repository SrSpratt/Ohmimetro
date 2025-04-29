#ifndef RVSKETCHES_H
#define RVSKETCHES_H

#include <RVgeneral.h>

#define matrix 25


/**
 * @brief Estrutura que representa uma cor RGB normalizada.
 */
typedef struct rgb{
    double red;
    double green;
    double blue;
} rgb;


/**
 * @brief Estrutura que representa um desenho para o display.
 */
typedef struct drawing {
    double figure[matrix]; /**< Matriz de dados da figura. */
    uint8_t index; /**< Índice atual da figura. */
    rgb main_color;  /**< Cor principal da figura. */
    rgb background_color; /**< Cor de fundo da figura. */
} sketch;


/**
 * @brief Retorna um vetor de figura correspondente ao caractere.
 * @param character Caractere de identificação ('o', 'x', etc).
 * @return Ponteiro para a figura.
 */
double* sketch_array(char character);
/**
 * @brief Converte uma cor RGB em valor de 32 bits para matriz de LEDs.
 * @param color Estrutura RGB.
 * @return Cor codificada em uint32_t.
 */
uint32_t rgb_matrix(rgb);
/**
 * @brief Desenha um sketch enviando os dados via PIO.
 * @param sketch Estrutura de desenho.
 * @param led_cfg Configuração inicial de LEDs.
 * @param pio Configuração PIO.
 * @param vector_size Tamanho do vetor de figuras.
 */
void draw(sketch, uint32_t, pio, const uint8_t);
/**
 * @brief Copia dados de um vetor para outro.
 * @param first_vec Vetor de destino.
 * @param second_vec Vetor de origem.
 * @param vector_size Tamanho do vetor.
 */
void vector_copy(double*, double*, uint8_t);
/**
 * @brief Imprime o conteúdo de um sketch.
 * @param sketch Estrutura de desenho.
 */
void print_sketch(sketch);

/**
 * @brief Mapeia valores normalizados para coordenadas do display.
 * @param values Vetor de valores.
 * @param vector_size Tamanho do vetor.
 */
void map_to_display(float*, uint8_t);
/**
 * @brief Desenha um ponto no display com base na leitura do ADC.
 * @param ssd Ponteiro para o driver do display.
 * @param a_pins Vetor de pinos ADC.
 * @param size Tamanho do vetor.
 * @param color Cor do ponto.
 * @param sketch Sketch a ser desenhado.
 * @param pio Configuração do PIO.
 */
void trace_dot(ssd1306_t*, adc*, uint8_t, bool, sketch, pio);

rgb get_color(int);

#endif