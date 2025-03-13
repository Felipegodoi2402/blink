/**
 * Exemplo de controle de motor de passo com Raspberry Pi Pico e ULN2003
 * Conexões: GPIO0-IN1, GPIO1-IN2, GPIO2-IN3, GPIO3-IN4
 * Botões: GPIO4 (sentido horário), GPIO5 (sentido anti-horário)
 */

#include "pico/stdlib.h"

// Definição dos pinos conectados ao ULN2003
#define STEP_PIN_1 0  // IN4
#define STEP_PIN_2 1  // IN3
#define STEP_PIN_3 2  // IN2
#define STEP_PIN_4 3  // IN1

// Definição dos pinos dos botões
#define BUTTON_CW  4  // Botão para sentido horário (GPIO4)
#define BUTTON_CCW 5  // Botão para sentido anti-horário (GPIO5)

// Delay entre os passos (em milissegundos)
#define STEP_DELAY_MS 2  // Reduzi para um movimento mais fluido

// Número de passos para uma volta completa (modo onda completa, 28BYJ-48)
#define STEPS_PER_REVOLUTION 512  // Ajustado conforme seu código

// Sequência de passos para motor de passo (modo onda completa)
const uint8_t STEP_SEQUENCE[4][4] = {
    {1, 0, 0, 1},  // Passo 1
    {1, 1, 0, 0},  // Passo 2
    {0, 1, 1, 0},  // Passo 3
    {0, 0, 1, 1}   // Passo 4
};

// Função para inicializar os pinos do motor e botões
void stepper_init(void) {
    // Inicializa os pinos do motor como saída
    gpio_init(STEP_PIN_1);
    gpio_init(STEP_PIN_2);
    gpio_init(STEP_PIN_3);
    gpio_init(STEP_PIN_4);
    
    gpio_set_dir(STEP_PIN_1, GPIO_OUT);
    gpio_set_dir(STEP_PIN_2, GPIO_OUT);
    gpio_set_dir(STEP_PIN_3, GPIO_OUT);
    gpio_set_dir(STEP_PIN_4, GPIO_OUT);
    
    // Define estado inicial como baixo
    gpio_put(STEP_PIN_1, 0);
    gpio_put(STEP_PIN_2, 0);
    gpio_put(STEP_PIN_3, 0);
    gpio_put(STEP_PIN_4, 0);

    // Inicializa os pinos dos botões como entrada com pull-up
    gpio_init(BUTTON_CW);
    gpio_init(BUTTON_CCW);
    
    gpio_set_dir(BUTTON_CW, GPIO_IN);
    gpio_set_dir(BUTTON_CCW, GPIO_IN);
    
    gpio_pull_up(BUTTON_CW);  // Ativa pull-up interno
    gpio_pull_up(BUTTON_CCW); // Ativa pull-up interno
}

// Função para executar um passo do motor
void stepper_step(int step) {
    gpio_put(STEP_PIN_1, STEP_SEQUENCE[step][0]);
    gpio_put(STEP_PIN_2, STEP_SEQUENCE[step][1]);
    gpio_put(STEP_PIN_3, STEP_SEQUENCE[step][2]);
    gpio_put(STEP_PIN_4, STEP_SEQUENCE[step][3]);
}

// Função para girar o motor em uma direção específica
void rotate_stepper(bool clockwise, int steps) {
    int step = 0;
    for (int i = 0; i < steps; i++) {
        if (clockwise) {
            // Sentido horário: incrementa o passo
            step = i % 4;
        } else {
            // Sentido anti-horário: decrementa o passo
            step = (4 - (i % 4)) % 4;
        }
        stepper_step(step);
        sleep_ms(STEP_DELAY_MS);
    }
}

int main() {
    // Inicializa o stdio
    stdio_init_all();
    
    // Inicializa os pinos do motor e botões
    stepper_init();
    
    while (true) {
        // Aguarda até que um dos botões seja pressionado
        // Os botões são ativos em nível baixo (0) devido ao pull-up
        if (!gpio_get(BUTTON_CW)) {
            // Botão GPIO4 pressionado: gira no sentido horário
            rotate_stepper(true, STEPS_PER_REVOLUTION);
            // Aguarda o botão ser solto para evitar múltiplas ativações
            while (!gpio_get(BUTTON_CW)) {
                sleep_ms(10);
            }
        }
        else if (!gpio_get(BUTTON_CCW)) {
            // Botão GPIO5 pressionado: gira no sentido anti-horário
            rotate_stepper(false, STEPS_PER_REVOLUTION);
            // Aguarda o botão ser solto para evitar múltiplas ativações
            while (!gpio_get(BUTTON_CCW)) {
                sleep_ms(10);
            }
        }
        
        // Pequena pausa para debounce e evitar sobrecarga da CPU
        sleep_ms(10);
    }
    
    return 0;
}