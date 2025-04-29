#ifndef RVGENERAL_H
#define RVGENERAL_H

#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "hardware/adc.h"
#include "hardware/pwm.h"
#include "hardware/clocks.h"
#include "hardware/pio.h"
#include "RVpio.pio.h"
#include <ssd1306.h>
#include <stdio.h>

#define I2C_PORT i2c1
#define I2C_SDA 14
#define I2C_SCL 15
#define ADDRESS 0x3C

#define PWM_WRAP 20000
#define PWM_CLKDIV 125.0f // com o divisor a esse número, o contador conta até 20000ms



/**
 * @brief Estrutura de configuração de um pino GPIO.
 */
typedef struct gpio_pinout{
    uint8_t pin;
    bool pin_dir;
} gpio;

/**
 * @brief Estrutura de configuração de um pino ADC.
 */
typedef struct adc_pinout{
    uint8_t pin;
    uint8_t channel;
} adc;

/**
 * @brief Estrutura de configuração de um pino PWM.
 */
typedef struct pwm_pinout{
    uint8_t pin;
    uint8_t slice;
} pwm;


/**
 * @brief Estrutura que representa uma configuração PIO.
 */
typedef struct pio_refs{
    PIO address;
    int state_machine;
    int offset;
    int pin;
} pio;


/**
 * @brief Contexto para manipulação de interrupções.
 */
typedef struct interruption_context{
    uint8_t pin; /**< Último pino que gerou interrupção. */
    uint8_t previous_pin; /**< Pino anterior. */
    uint8_t presses; /**< Contador de pressionamentos. */
    bool double_border; /**< Estado da flag de duplo clique. */
    bool play; /**< Flag para indicar o estado do jogo. */
} irq_context;


/**
 * @brief Variável global do contexto de interrupção.
 */
extern irq_context context;


/**
 * @brief Inicializa as interfaces de comunicação e periféricos.
 */
void init_interfaces();

/**
 * @brief Inicializa o display OLED.
 * @param ssd Ponteiro para o driver do display.
 */
void init_display(ssd1306_t*);

/**
 * @brief Exibe um retângulo de debug no display.
 * @param ssd Ponteiro para o driver do display.
 * @param color Cor do retângulo.
 */
void debug_display(ssd1306_t*, bool);

/**
 * @brief Inicializa pinos GPIO.
 * @param pins Vetor de pinos.
 * @param vector_size Tamanho do vetor.
 * @return 0 em caso de sucesso.
 */
int init_gpio(gpio*, uint8_t);
/**
 * @brief Exibe informações de debug de pinos GPIO.
 * @param pins Vetor de pinos.
 * @param vector_size Tamanho do vetor.
 */
void debug_gpio(gpio*, uint8_t);

/**
 * @brief Imprime informações de um pino GPIO.
 * @param pin Estrutura gpio.
 */
void print_gpio(gpio);


/**
 * @brief Configura os canais ADC.
 * @param pins Vetor de pinos ADC.
 * @param vector_size Tamanho do vetor.
 */
void config_adc(adc*, uint8_t);

/**
 * @brief Lê os valores dos canais ADC.
 * @param readings Vetor de armazenamento das leituras.
 * @param pins Vetor de pinos ADC.
 * @param vector_size Tamanho do vetor.
 */
void read_adc(uint16_t*, adc*, uint8_t);

/**
 * @brief Exibe leituras dos canais ADC.
 * @param pins Vetor de pinos ADC.
 * @param vector_size Tamanho do vetor.
 */
void debug_adc(adc*, uint8_t);


/**
 * @brief Configura pinos PWM.
 * @param pins Vetor de pinos PWM.
 * @param vector_size Tamanho do vetor.
 */
void config_pwm(pwm*, uint8_t);

/**
 * @brief Exibe informações de pinos PWM.
 * @param pins Vetor de pinos PWM.
 * @param vector_size Tamanho do vetor.
 */
void debug_pwm(pwm*, uint8_t);

/**
 * @brief Imprime o status de um pino PWM.
 * @param pin Estrutura pwm.
 */
void print_pwm(pwm);


/**
 * @brief Configura periféricos PIO.
 * @param pio Ponteiro para a configuração PIO.
 */
void config_pio(pio*);

/**
 * @brief Exibe informações de debug de um PIO.
 * @param pio Estrutura pio.
 */
void debug_pio(pio);


/**
 * @brief Configura interrupções nos pinos informados.
 * @param pins Vetor de pinos GPIO.
 * @param vector_size Tamanho do vetor.
 */
void set_interrupts(gpio*, uint8_t);

/**
 * @brief Callback de interrupção dos pinos.
 * @param gpio Número do pino.
 * @param events Eventos gerados.
 */
void interrupt_callback(uint, uint32_t);

#endif