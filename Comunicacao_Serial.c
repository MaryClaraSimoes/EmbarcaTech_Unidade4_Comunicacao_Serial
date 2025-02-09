#include <stdlib.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "inc/ssd1306.h"
#include "inc/font.h"

#include <stdio.h>
#include <hardware/pio.h>
#include "hardware/clocks.h"
#include "hardware/gpio.h"

#include "animacao_matriz.pio.h" // Biblioteca PIO para controle de LEDs WS2818B

// Definição de pinos e parâmetros
#define PINO_LED_VERDE 11
#define PINO_LED_AZUL 12
#define PINO_LED_VERMELHO 13
#define QUANTIDADE_LEDS 25
#define PINO_MATRIZ 7
#define PINO_BOTAO_A 5
#define PINO_BOTAO_B 6
#define PORTA_I2C i2c1
#define PINO_SDA 14
#define PINO_SCL 15
#define ENDERECO_DISPLAY 0x3C

// Variáveis globais
PIO pio;
uint sm;
ssd1306_t ssd;
static volatile uint numero_atual = 0;
static volatile uint32_t ultimo_tempo = 0;
static volatile uint estado_led_verde = 0;
static volatile uint estado_led_azul = 0;

// Definição dos padrões de números para a matriz de LEDs
double padrao_numeros[10][QUANTIDADE_LEDS] = {
    {0, 1, 1, 1, 0, 0, 1, 0, 1, 0, 0, 1, 0, 1, 0, 0, 1, 0, 1, 0, 0, 1, 1, 1, 0}, // 0
    {0, 0, 1, 0, 0, 0, 1, 1, 0, 0, 1, 0, 1, 0, 0, 0, 1, 0, 1, 0, 0, 1, 0, 0, 0}, // 1
    {0, 1, 1, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 1, 0, 1, 0, 1, 0, 0, 1, 1, 1, 0}, // 2
    // ... outros números até o 9
};

// Mapeamento dos LEDs da matriz
int mapeamento_leds[QUANTIDADE_LEDS] = {0, 1, 2, 3, 4, 9, 8, 7, 6, 5, 10, 11, 12, 13, 14, 19, 18, 17, 16, 15, 20, 21, 22, 23, 24};

// Função para gerar o valor RGB do LED
uint32_t gerar_rgb(double intensidade) {
    unsigned char valor = intensidade * 255;
    return (valor << 16); // Retorna o valor RGB para o LED
}

// Função para atualizar a exibição dos números na matriz de LEDs
void atualizar_matriz(int numero) {
    uint32_t cor_led;
    for (int i = 0; i < QUANTIDADE_LEDS; i++) {
        cor_led = gerar_rgb(padrao_numeros[numero][mapeamento_leds[24 - i]]);
        pio_sm_put_blocking(pio, sm, cor_led); // Atualiza a matriz com a cor calculada
    }
}

// Função para lidar com as interrupções dos botões com debounce
static void interrupcao_gpio(uint gpio, uint32_t eventos) {
    uint32_t tempo_atual = to_ms_since_boot(get_absolute_time());

    // Evita múltiplos disparos rápidos
    if (tempo_atual - ultimo_tempo > 200) {
        if (gpio == PINO_BOTAO_A) {
            estado_led_verde = !estado_led_verde; // Alterna o estado do LED verde
            gpio_put(PINO_LED_VERDE, estado_led_verde);

            // Atualiza o display OLED
            char texto[13];
            snprintf(texto, 13, "Led verde %u\n", estado_led_verde);
            ssd1306_fill(&ssd, true);
            ssd1306_rect(&ssd, 3, 3, 122, 58, false, true);
            ssd1306_draw_string(&ssd, "Botao A", 8, 10);
            ssd1306_draw_string(&ssd, "pressionado", 8, 20);
            ssd1306_draw_string(&ssd, texto, 8, 30);
            ssd1306_send_data(&ssd);
        } else if (gpio == PINO_BOTAO_B) {
            estado_led_azul = !estado_led_azul; // Alterna o estado do LED azul
            gpio_put(PINO_LED_AZUL, estado_led_azul);

            // Atualiza o display OLED
            char texto[13];
            snprintf(texto, 13, "Led azul %u\n", estado_led_azul);
            ssd1306_fill(&ssd, true);
            ssd1306_rect(&ssd, 3, 3, 122, 58, false, true);
            ssd1306_draw_string(&ssd, "Botao B", 8, 10);
            ssd1306_draw_string(&ssd, "pressionado", 8, 20);
            ssd1306_draw_string(&ssd, texto, 8, 30);
            ssd1306_send_data(&ssd);
        }

        ultimo_tempo = tempo_atual; // Atualiza o último tempo de interrupção
    }
}

int main() {
    // Configuração da comunicação I2C com o display SSD1306
    i2c_init(PORTA_I2C, 400 * 1000); // I2C a 400kHz
    gpio_set_function(PINO_SDA, GPIO_FUNC_I2C); // Configura o pino SDA
    gpio_set_function(PINO_SCL, GPIO_FUNC_I2C); // Configura o pino SCL
    gpio_pull_up(PINO_SDA);  // Ativa pull-up para SDA
    gpio_pull_up(PINO_SCL);  // Ativa pull-up para SCL
    ssd1306_init(&ssd, LARGURA, ALTURA, false, ENDERECO_DISPLAY, PORTA_I2C); // Inicializa o display
    ssd1306_config(&ssd); // Configura o display

    // Inicialização da comunicação serial
    stdio_init_all();
    printf("Sistema iniciado.\n");

    // Configuração do PIO para controle dos LEDs WS2818B
    pio = pio0;
    bool sucesso = set_sys_clock_khz(128000, false); // Configura o clock do sistema
    if (sucesso) printf("Clock configurado para %ld Hz\n", clock_get_hz(clk_sys));

    uint offset = pio_add_program(pio, &animacao_matriz_program);
    sm = pio_claim_unused_sm(pio, true);
    animacao_matriz_program_init(pio, sm, offset, PINO_MATRIZ);

    // Configuração dos LEDs RGB
    gpio_init(PINO_LED_VERMELHO);
    gpio_set_dir(PINO_LED_VERMELHO, GPIO_OUT);
    gpio_init(PINO_LED_VERDE);
    gpio_set_dir(PINO_LED_VERDE, GPIO_OUT);
    gpio_init(PINO_LED_AZUL);
    gpio_set_dir(PINO_LED_AZUL, GPIO_OUT);

    // Configuração dos botões
    gpio_init(PINO_BOTAO_A);
    gpio_set_dir(PINO_BOTAO_A, GPIO_IN);
    gpio_pull_up(PINO_BOTAO_A);

    gpio_init(PINO_BOTAO_B);
    gpio_set_dir(PINO_BOTAO_B, GPIO_IN);
    gpio_pull_up(PINO_BOTAO_B);

    // Configuração das interrupções dos botões
    gpio_set_irq_enabled_with_callback(PINO_BOTAO_A, GPIO_IRQ_EDGE_FALL, true, &interrupcao_gpio);
    gpio_set_irq_enabled_with_callback(PINO_BOTAO_B, GPIO_IRQ_EDGE_FALL, true, &interrupcao_gpio);

    // Loop principal
    while (true) {
        if (stdio_usb_connected()) { // Verifica conexão USB
            char caractere;
            if (scanf("%c", &caractere) == 1) { // Lê um caractere da entrada serial
                printf("Caractere recebido: '%c'\n", caractere);

                // Se o caractere for um número, exibe na matriz de LEDs
                if (caractere >= '0' && caractere <= '9') {
                    numero_atual = caractere - '0';
                    atualizar_matriz(numero_atual); // Exibe o número na matriz de LEDs
                }

                // Atualiza o display OLED com o caractere recebido
                char texto[2] = {caractere, '\0'};
                ssd1306_fill(&ssd, true);
                ssd1306_rect(&ssd, 3, 3, 122, 58, false, true);
                ssd1306_draw_string(&ssd, texto, 8, 10);
                ssd1306_send_data(&ssd);
            }
        }
        sleep_ms(100); // Aguarda para não sobrecarregar a CPU
    }

    return 0; // Finaliza o programa
}
