#include <RVsketches.h>

double* sketch_array(char character) {
    static double white[] = {
        0.0, 0.0, 8.0, 0.0, 9.0,
        9.0, 0.0, 8.0, 0.0, 0.0,
        0.0, 0.0, 8.0, 0.0, 9.0,
        9.0, 0.0, 8.0, 0.0, 0.0,
        0.0, 0.0, 8.0, 0.0, 9.0
    };
    static double playing [] = {
        0.0, 0.0, 0.0, 0.0, 0.0,
        0.0, 1.0, 1.0, 1.0, 0.0,
        0.0, 1.0, 0.0, 1.0, 0.0,
        0.0, 1.0, 1.0, 1.0, 0.0,
        0.0, 0.0, 0.0, 0.0, 0.0
    };
    static double lost[] = {
        1.0, 0.0, 0.0, 0.0, 1.0,
        0.0, 1.0, 0.0, 1.0, 0.0,
        0.0, 0.0, 1.0, 0.0, 0.0,
        0.0, 1.0, 0.0, 1.0, 0.0,
        1.0, 0.0, 0.0, 0.0, 1.0
    };

    switch (character) {
        case 'o':
            return playing;
        case 'x':
            return lost;
        default:
            return white;
    }
}

uint32_t rgb_matrix(rgb color){
    unsigned char r, g, b;
    r = color.red* 255;
    g = color.green * 255;
    b = color.blue * 255;
    return (g << 24) | (r << 16) | (b << 8);
}

void draw(sketch sketch, uint32_t led_cfg, pio pio, const uint8_t vector_size){
    pio_sm_restart(pio.address, pio.state_machine);

    for(int16_t i = 0; i < vector_size; i++){
        if (sketch.figure[i] > 0.0)
            led_cfg = rgb_matrix(get_color(sketch.figure[i]));
        else
            led_cfg = rgb_matrix(sketch.background_color);
        pio_sm_put_blocking(pio.address, pio.state_machine, led_cfg);
    }
};


void vector_copy(double* first_vec, double* second_vec, const uint8_t vector_size){
    for(uint8_t i = 0; i < vector_size; i++){
        first_vec[i] = 0.0;
        first_vec[i] = second_vec[i];
    }
}

void map_to_display(float* values, uint8_t vector_size){
    float normalizations[2];

    normalizations[0] = (values[0])/(4095.0f);
    normalizations[1] = (values[1])/(4095.0f);

    values[0] = (uint16_t) (normalizations[0] * 56.0f);
    values[1] = (uint16_t) (normalizations[1] * 120.0f);
}

void trace_dot(ssd1306_t* ssd, adc* a_pins, uint8_t size, bool color, sketch sketch, pio pio){
    uint16_t previous_values[2];
    uint16_t values[2];

    //efetua as leituras no vetor values
    read_adc(values, a_pins, 2);
    //printf("value Y: %d", values[0]);
    //printf("value X: %d", values[1]);


    // se a posição mudou
    if (values[0] != previous_values[0] || values[1] != previous_values[1]){

        previous_values[0] = values[0];
        previous_values[1] = values[1];

        float mappings[2];
        mappings[0] = values[0];
        mappings[1] = values[1];

        //normaliza as posições
        map_to_display(mappings, 2);

        //desenha a "arena"
        ssd1306_fill(ssd, !color);
        ssd1306_rect(ssd, 0, 0, 127, 63, color, !color);

        //posiciona x e y dentro da lógica do display
        uint8_t char_x = 56 - mappings[0];
        uint8_t char_y = mappings[1];

        printf("\nchar_x: %d\n", char_x);
        //printf("char_y: %d\n",char_y);

        int pos_x = -1;
        int pos_y = -1;

        
        //verifica se o retângulo colidiu com as paredes e executa a lógica: jogo ou derrota?
        if (char_x > 3 && char_x < 56 && char_y > 0 && char_y < 119){
            rgb main_color = {
                .green = 0.01,
                .blue = 0.00,
                .red = 0.00
            };
            rgb background_color = {
                .green = 0.01,
                .blue = 0.01,
                .red = 0.01
            };
            sketch.main_color = main_color;
            sketch.background_color = background_color;
            vector_copy(sketch.figure, sketch_array('o'), matrix);
            draw(sketch, 0, pio, matrix);
            ssd1306_rect(ssd, char_x, char_y, 8, 8, color, color);
            ssd1306_send_data(ssd);
            gpio_put(11, 1);
        } else {
            ssd1306_draw_string(ssd, "PERDEU!", (128/2) - ((6*8)/2), 64/2);
                rgb main_color = {
                .green = 0.00,
                .blue = 0.00,
                .red = 0.01
            };
            rgb background_color = {
                .green = 0.00,
                .blue = 0.01,
                .red = 0.01
            };
            sketch.main_color = main_color;
            sketch.background_color = background_color;
            vector_copy(sketch.figure, sketch_array('x'), matrix);
            draw(sketch, 0, pio, matrix);
            ssd1306_send_data(ssd);
            gpio_put(11, 0);
            gpio_put(13, 1);
            pwm_set_gpio_level(21, 512);
            sleep_ms(500);
            pwm_set_gpio_level(21, 0);
            gpio_put(13, 0);
            context.play = false;
        }

    }
}

rgb get_color(int number){
    rgb color;
    switch (number){
        case 9:
            color.red = 0.06;
            color.green = 0.06;
            color.blue = 0.06;
            break;
        case 8:
            color.red = 0.01;
            color.green = 0.01;
            color.blue = 0.01;
            break;
        case 7:
            color.red = 0.04;
            color.green = 0.00;
            color.blue = 0.07;
            break;
        case 6:
            color.red = 0.00;
            color.green = 0.00;
            color.blue = 0.08;
            break;
        case 5:
            color.red = 0.01;
            color.green = 0.05;
            color.blue = 0.00;
            break;
        case 4:
            color.red = 0.04;
            color.green = 0.04;
            color.blue = 0.00;
            break;
        case 3:
            color.red = 0.08;
            color.green = 0.01;
            color.blue = 0.00;
            break;
        case 2:
            color.red = 0.05;
            color.green = 0.00;
            color.blue = 0.00;
            break;
        case 1:
            color.red = 0.03;
            color.green = 0.01;
            color.blue = 0.00;
            break;
        case 0:
        default:
            color.red = 0.00;
            color.green = 0.00;
            color.blue = 0.00;
            break;                     
    }
    return color;
}