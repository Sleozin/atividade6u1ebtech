#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "tusb.h"

// Definição dos pinos dos LEDs
#define LED_VERDE 11
#define LED_AZUL  12
#define LED_VERMELHO 13

// Buffer para receber dados
#define BUF_SIZE 64
static char buf[BUF_SIZE];

// Função para configurar os GPIOs
void setup_gpio() {
    gpio_init(LED_VERDE);
    gpio_init(LED_AZUL);
    gpio_init(LED_VERMELHO);
    
    gpio_set_dir(LED_VERDE, GPIO_OUT);
    gpio_set_dir(LED_AZUL, GPIO_OUT);
    gpio_set_dir(LED_VERMELHO, GPIO_OUT);
    
    gpio_put(LED_VERDE, 0);
    gpio_put(LED_AZUL, 0);
    gpio_put(LED_VERMELHO, 0);
}
// Função para limpar completamente o buffer do CDC
void clear_cdc_buffer() {
    while (tud_cdc_available()) {
        uint32_t count = tud_cdc_read(buf, BUF_SIZE);
        // Simplesmente descarta os dados lidos
    }
    // Limpa o buffer local também
    memset(buf, 0, BUF_SIZE);
}

// Função para processar os comandos recebidos
void process_command(const char* command) {
    // Faz eco do comando recebido
    tud_cdc_write_str(command);
    tud_cdc_write_str("\r\n");
    tud_cdc_write_flush();
    
    // Desliga todos os LEDs primeiro
    gpio_put(LED_VERDE, 0);
    gpio_put(LED_AZUL, 0);
    gpio_put(LED_VERMELHO, 0);
    
    // Verifica qual comando foi recebido e acende o LED correspondente
    if (strcmp(command, "verde") == 0) {
        gpio_put(LED_VERDE, 1);
        clear_cdc_buffer();
    } else if (strcmp(command, "azul") == 0) {
        gpio_put(LED_AZUL, 1);
        clear_cdc_buffer();
    } else if (strcmp(command, "vermelho") == 0) {
        gpio_put(LED_VERMELHO, 1);
        clear_cdc_buffer();
    }
    
    // Mantém o LED aceso por 1 segundo
    sleep_ms(1000);
    
    // Desliga o LED após 1 segundo
    gpio_put(LED_VERDE, 0);
    gpio_put(LED_AZUL, 0);
    gpio_put(LED_VERMELHO, 0);
}

// Função principal
int main() {
    // Inicializa o SDK
    stdio_init_all();
    
    // Configura os GPIOs
    setup_gpio();
    
    // Inicializa o TinyUSB
    tusb_init();
    
    while (true) {
        // Task do TinyUSB
        tud_task();
        
        // Verifica se há dados disponíveis para leitura
        if (tud_cdc_available()) {
            uint32_t count = tud_cdc_read(buf, BUF_SIZE);
            
            // Processa apenas se recebeu dados
            if (count > 0) {
                // Remove possíveis caracteres de nova linha
                if (buf[count-1] == '\n' || buf[count-1] == '\r') {
                    buf[count-1] = '\0';
                }
                if (count > 1 && (buf[count-2] == '\r' || buf[count-2] == '\n')) {
                    buf[count-2] = '\0';
                }
                
                // Processa o comando
                process_command(buf);
            }
        }
        
        // Pequeno delay para evitar uso excessivo da CPU
        sleep_ms(1);
    }
    
    return 0;
}

// Callbacks do TinyUSB
void tud_cdc_rx_cb(uint8_t itf) {
    // Chamado quando dados são recebidos
    // Não é necessário implementar nada aqui
}