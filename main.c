/***
 * Communication Serial
 * By: Adimael Santos da Silva
 * Código disponível em: github.com/adimael
 */
#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "hardware/i2c.h"
#include "hardware/pwm.h"
#include "hardware/clocks.h"
#include "include/ssd1306.h"
#include "include/font.h"

// LED RGB, com os pinos conectados às GPIOs (11, 12 e 13).
#define LED_PIN_GREEN 11 // LED VERDE
#define LED_PIN_BLUE  12 // LED AZUL
#define LED_PIN_RED   13 // LED VERMELHO

// Botão do Joystick conectado à GPIO 22.
#define JOYSTICK_BUTTON_PIN 22

// Joystick conectado aos GPIOs 26 e 27.
#define JOYSTICK_X_PIN 26
#define JOYSTICK_Y_PIN 27

// Botão A conectado à GPIO 5.
#define BUTTON_A_PIN 5

// Display SSD1306 conectado via I2C (GPIO 14 e GPIO15).
#define I2C_SDA_PIN 14
#define I2C_SCL_PIN 15

#define ENDERECO 0x3C      // Endereço do display OLED
#define I2C_PORTA i2c1     // Porta I2C

// Definições PWM
#define VALOR_MAXIMO_PWM 4096 // Periodo de wrap

// Variáveis
static volatile uint32_t ultima_hora = 0;
static volatile uint slice_blue, slice_red, limite_lateral = 125, limite_vertical = 60;
static volatile bool estado_pwm = true, cor_pixels = true, borda_display = true;

// Escopo Funções
void init();                                                 // inicializa o led verde e os botões
uint pwm_init_gpio(uint gpio, uint wrap);                    // inicializa os LEDs PWM
void atualizar_estado_led_X(uint16_t x);                    // Muda o estado do LED vermelho de Acordo com X do Joystick
void atualizar_estado_led_Y(uint16_t y);                   // Muda o estado do LED azul de Acordo com Y do Joystick
static void gpio_irq_handler(uint gpio, uint32_t events); // Trata a interrupção Ao apertar um botão

void atualizar_estado_led_X(uint16_t x)
{
    uint16_t posicao = x;
    static bool subida;
    subida = (posicao >= 2048);
    
    if (subida)
        posicao = (posicao % 2048);
    else
        posicao = ((posicao * -1) + 2048);

    pwm_set_gpio_level(LED_PIN_BLUE, posicao * 2);
}

void atualizar_estado_led_Y(uint16_t y)
{
    uint16_t posicao = y;
    static bool subida;
    subida = (posicao >= 2048);
    
    if (subida)
        posicao = (posicao % 2048);
    else
        posicao = ((posicao * -1) + 2048);

    pwm_set_gpio_level(LED_PIN_RED, posicao * 2);
}

void init()
{
    // inicializa o botão do joystick e a interrupção
    gpio_init(JOYSTICK_BUTTON_PIN);
    gpio_set_dir(JOYSTICK_BUTTON_PIN, GPIO_IN);
    gpio_pull_up(JOYSTICK_BUTTON_PIN);
    gpio_set_irq_enabled_with_callback(JOYSTICK_BUTTON_PIN, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler); // Rotina de Interrupção

    // inicializa o botão A e a interrupção
    gpio_init(BUTTON_A_PIN);
    gpio_set_dir(BUTTON_A_PIN, GPIO_IN);
    gpio_pull_up(BUTTON_A_PIN);
    gpio_set_irq_enabled_with_callback(BUTTON_A_PIN, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler); // Rotina de Interrupção

    // inicializa o LED verde
    gpio_init(LED_PIN_GREEN);
    gpio_set_dir(LED_PIN_GREEN, GPIO_OUT);
    gpio_put(LED_PIN_GREEN, 0);
}

static void gpio_irq_handler(uint gpio, uint32_t events)
{
    // Configura a ação ao apertar o botão e implementa o Debouce

    // Obtém o tempo atual em microssegundos
    uint32_t current_time = to_us_since_boot(get_absolute_time());

    // Verifica se passou tempo suficiente desde o último evento
    if (current_time - ultima_hora > 200000) // 200 ms de debouncing
    {
        ultima_hora = current_time; // Atualiza o tempo do último evento
        // Código Função:
        if (gpio == BUTTON_A_PIN)
        {
            // Altera o estado de Funcionamento dos LEDs PWM
            estado_pwm = !estado_pwm;
            pwm_set_enabled(slice_blue, estado_pwm);
            pwm_set_enabled(slice_red, estado_pwm);
        }
        else if (gpio == JOYSTICK_BUTTON_PIN)
        {
            // Alterna o Estado do LED Verde
            bool state = gpio_get(LED_PIN_GREEN);
            gpio_put(LED_PIN_GREEN, !state); 

            // Muda o tamanho da borda e a cor
            borda_display = !borda_display;
            if (borda_display){
                limite_lateral = 125;
                limite_vertical = 60;
            }else{
                limite_vertical = 55;
                limite_lateral = 120;
            }
        }
    }
}

uint pwm_init_gpio(uint gpio, uint wrap)
{
    // Inicializa um LED como PWM
    gpio_set_function(gpio, GPIO_FUNC_PWM);

    uint slice_num = pwm_gpio_to_slice_num(gpio);
    pwm_set_wrap(slice_num, wrap);

    pwm_set_enabled(slice_num, true);
    return slice_num;
}

// Função principal
int main() {
    stdio_init_all();

    // Variáveis
    uint16_t adc_value_x;                                  // Armazena o valor digital de x
    uint16_t adc_value_y;                                  // Armazena o valor digital de y
    uint16_t pos_x;                                        // Armazena a posição de X no display
    uint16_t pos_y;                                        // Armazena a posição de Y no display
    slice_red = pwm_init_gpio(LED_PIN_RED, VALOR_MAXIMO_PWM);   // Inicializa o LED PWM Vermelho
    slice_blue = pwm_init_gpio(LED_PIN_BLUE, VALOR_MAXIMO_PWM); // Inicializa o LED Azul

    init(); // Inicialização dos Pinos

    // Inicialização do I2C

    i2c_init(I2C_PORTA, 400 * 1000);
    gpio_set_function(I2C_SDA_PIN, GPIO_FUNC_I2C);                    // Set the GPIO pin function to I2C
    gpio_set_function(I2C_SCL_PIN, GPIO_FUNC_I2C);                    // Set the GPIO pin function to I2C
    gpio_pull_up(I2C_SDA_PIN);                                        // Pull up the data line
    gpio_pull_up(I2C_SCL_PIN);                                        // Pull up the clock line
    ssd1306_t ssd;                                                // Inicializa a estrutura do display
    ssd1306_init(&ssd, WIDTH, HEIGHT, false, ENDERECO, I2C_PORTA); // Inicializa o display
    ssd1306_config(&ssd);                                         // Configura o display
    ssd1306_send_data(&ssd);                                      // Envia os dados para o display
    ssd1306_fill(&ssd, false);                                    // Limpa o display. O display inicia com todos os pixels apagados.
    ssd1306_send_data(&ssd);

    // Inicialização ADC
    adc_init();
    adc_gpio_init(JOYSTICK_X_PIN);
    adc_gpio_init(JOYSTICK_Y_PIN);

    while (true)
    {
        adc_select_input(0);      // Seleciona o ADC para eixo X. O pino 26 como entrada analógica
        adc_value_x = adc_read(); // lê o valor de X
        adc_select_input(1);      // Seleciona o ADC para eixo Y. O pino 27 como entrada analógica
        adc_value_y = adc_read(); // lê o valor de Y

        atualizar_estado_led_X(adc_value_x); // Muda o estado do led de acordo com x
        atualizar_estado_led_Y(adc_value_y); // Muda o estado do led de acordo com y

        pos_x = (((adc_value_x * -1) + 4095) / 64); // configura a posição X do quadrado no display
        pos_y = (adc_value_y / 32);                 // configura a posição Y do quadrado no display

        // Ajustes para que o Quadrado não passe da borda
        if (pos_x > 50)
            pos_x = 50;
        else if (pos_x < 8)
            pos_x = 8;
        if (pos_y > 115)
            pos_y = 115;
        else if (pos_y < 8)
            pos_y = 8;

        ssd1306_fill(&ssd, !cor_pixels);                                // limpa o buffer do display
        ssd1306_rect(&ssd, pos_x, pos_y, 8, 8, cor_pixels, true);        // gera o quadrado 8x8 no buffer
        ssd1306_rect(&ssd, 3, 3, limite_lateral, limite_vertical, cor_pixels, false); // Desenha um retângulo de borda no Buffer
        ssd1306_send_data(&ssd);                                  // envia os dados do buffer ao display

        sleep_ms(100); // aguarda 100ms
    }

}