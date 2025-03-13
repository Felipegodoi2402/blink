/**
 * Exemplo de controle de motor de passo com Raspberry Pi Pico e ULN2003
 * Conexões: GPIO0-IN1, GPIO1-IN2, GPIO2-IN3, GPIO3-IN4
 */

#include "pico/stdlib.h"

// Definição dos pinos conectados ao ULN2003
#define STEP_PIN_1 0  // IN4
#define STEP_PIN_2 1  // IN3
#define STEP_PIN_3 2  // IN2
#define STEP_PIN_4 3  // IN1

// Delay entre os passos (em milissegundos)
#define STEP_DELAY_MS 2  // Reduzi para um movimento mais fluido

// Número de passos para uma volta completa (modo onda completa, 28BYJ-48)
#define STEPS_PER_REVOLUTION 1024

// Sequência de passos para motor de passo (modo onda completa)
const uint8_t STEP_SEQUENCE[4][4] = {
    {1, 0, 0, 1},  // Passo 1
    {1, 1, 0, 0},  // Passo 2
    {0, 1, 1, 0},  // Passo 3
    {0, 0, 1, 1}   // Passo 4
};

// Função para inicializar os pinos do motor
void stepper_init(void) {
    // Inicializa os pinos como saída
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
    
    // Inicializa os pinos do motor
    stepper_init();
    
    while (true) {
        // Gira uma volta completa no sentido horário
        rotate_stepper(true, STEPS_PER_REVOLUTION);
        
        // Pequena pausa antes de inverter
        sleep_ms(400);
        
        // Gira uma volta completa no sentido anti-horário
        rotate_stepper(false, STEPS_PER_REVOLUTION);
        
        // Pequena pausa antes de repetir
        sleep_ms(400);
    }
    
    return 0;
}