#include <RVgeneral.h>
#include <stdio.h>


irq_context context = {
    .pin = 0,
    .previous_pin = 0,
    .presses = 1,
    .double_border = false,
    .play = false
};
volatile uint32_t interval_a = 0;
volatile uint32_t interval_b = 0;
volatile uint32_t interval_j = 0;

void init_interfaces(){
    stdio_init_all();

    i2c_init(I2C_PORT, 400 * 1000);
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);
}

void init_display(ssd1306_t* ssd){
    ssd1306_init(ssd, WIDTH, HEIGHT, false, ADDRESS, I2C_PORT);
    ssd1306_config(ssd);
    ssd1306_send_data(ssd);

    ssd1306_fill(ssd, false);
    ssd1306_send_data(ssd);
}

void debug_display(ssd1306_t* ssd, bool color){
    ssd1306_fill(ssd, !color);
    ssd1306_rect(ssd, 3, 3, 120, 60, color, !color);
    ssd1306_draw_string(ssd, "DEBUG!", (128/2) - ((6*8)/2), 64/2);
    ssd1306_send_data(ssd);
    printf("\nDepuração de um retângulo de 120x60 no display de %dx%d!\n", WIDTH, HEIGHT);
}

void config_adc(adc* pins, uint8_t vector_size){
    adc_init();
    for (uint8_t i =0; i < vector_size; i++)
        adc_gpio_init(pins[i].pin);
}

void read_adc(uint16_t* readings, adc* pins, uint8_t vector_size){
    for (uint8_t i = 0; i < vector_size; i++){
        adc_select_input(pins[i].channel);
        readings[i] = adc_read();
    }
}

void debug_adc(adc* pins, uint8_t vector_size){
    printf("\nADC\n");
    printf("\nReadings:\n");
    uint16_t readings[vector_size];
    read_adc(readings, pins, vector_size);
    for (uint8_t i = 0; i < vector_size; i++){
        printf("Pin %d, channel %d: %d\n", pins[i].pin, pins[i].channel, readings[i]);
    }
}

int init_gpio(gpio* pins, uint8_t vector_size){
    for (uint8_t i = 0; i < vector_size; i++){
        gpio_init(pins[i].pin);
        gpio_set_dir(pins[i].pin, pins[i].pin_dir);
        if (pins[i].pin_dir == 0)
            gpio_pull_up(pins[i].pin);
        else
            gpio_put(pins[i].pin, 0);
    }

    return 0;
}

void debug_gpio(gpio* pins, uint8_t vector_size){
    printf("Entrou aqui!");
    for (uint8_t i = 0; i < vector_size; i++){
        print_gpio(pins[i]);
    }
}

void print_gpio(gpio pin){
    printf("\nGPIO");
    printf("\nDireção do pino %d: %s", pin.pin, gpio_get_dir(pin.pin) == 0 ? "IN" : "OUT");
    printf("\nNível: %s", gpio_get(pin.pin) == 0 ? "0V" : "3,3V");
}

void config_pwm(pwm* pins, uint8_t vector_size){
    for(uint8_t i = 0; i < vector_size; i++){
        gpio_set_function(pins[i].pin, GPIO_FUNC_PWM);
        pins[i].slice = pwm_gpio_to_slice_num(pins[i].pin);

        if (i > 0 && pins[i].slice == pins[i-1].slice)
            continue;

        pwm_set_wrap(pins[i].slice, PWM_WRAP);
        pwm_set_clkdiv(pins[i].slice, PWM_CLKDIV);
        pwm_set_enabled(pins[i].slice, true);
    }
}

void debug_pwm(pwm* pins, uint8_t vector_size){
    for(uint8_t i = 0; i < vector_size; i++)
        print_pwm(pins[i]);
}

void print_pwm(pwm pin){
    printf("\nPWM");
    printf("\nContador do pino %d: %d", pin.pin, pwm_get_counter(pin.slice));
}

void config_pio(pio* pio){
    pio->address = pio0;
    if (!set_sys_clock_khz(128000, false))
        printf("clock errado!");
    pio->offset = pio_add_program(pio->address, &pio_review_program);
    pio->state_machine = pio_claim_unused_sm(pio->address, true);

    pio_review_program_init(pio->address, pio->state_machine, pio->offset, pio->pin);
}

void debug_pio(pio pio){
    printf("\nPIO");
    printf("\nPino: %d", pio.pin);
    printf("\nEndereço: %d", pio.address);
    printf("\nDeslocamento: %d", pio.offset);
    printf("\nMáquina de estados: %d", pio.state_machine);
}

void set_interrupts(gpio* pins, uint8_t vector_size){
    gpio_set_irq_enabled_with_callback(pins[0].pin,GPIO_IRQ_EDGE_RISE, true, &interrupt_callback);
    for (uint8_t i = 1; i < vector_size; i++)
        gpio_set_irq_enabled(pins[i].pin, GPIO_IRQ_EDGE_RISE, true);
}

void interrupt_callback(uint gpio, uint32_t events){
    context.pin = gpio;
    uint32_t interval = 0;
    if (gpio == 5)
        interval = interval_a;
    else if (gpio == 6)
        interval = interval_b;
    else
        interval = interval_j;

    uint32_t current_time = to_us_since_boot(get_absolute_time());
    if (current_time - interval > 250000){

        if (context.previous_pin == context.pin)
            context.presses++;
        else
            context.presses = 1;

        printf("\ngpio:%d\ntimes:%d\n", gpio, context.presses);

        if (gpio == 5){
            pwm_set_gpio_level(21, 0);
            pwm_set_gpio_level(10, 0);
            context.play = !context.play;
            interval_a = current_time;
        } else if (gpio == 6) {
            pwm_set_gpio_level(10, 0);
            pwm_set_gpio_level(21, 0);
            context.play = !context.play;
            interval_b = current_time;
        } else if (gpio = 22){
            pwm_set_gpio_level(10, 0);
            pwm_set_gpio_level(21, 0);
            context.double_border = !context.double_border;
            context.play = !context.play;
            interval_j = current_time;
        }

        context.previous_pin = context.pin;
    }
}