#include "auxiliar.h"

Acionador BotaoA(PIN_BOTAO_A);
Acionador BotaoB(PIN_BOTAO_B);

bool botao_a_estado = false;
bool botao_b_estado = false;


void inicializar_stdio()
{
    stdio_init_all();
    printf("Iniciando monitor de botões...\n");
}

void resposta(const char *r, int t)
{
    printf("Resposta (%d):\n%.*s\n", t, t, r);
}

void erro(const char *msg)
{
    printf("Erro: %s\n", msg);
}

void monitorar_botoes()
{
    BotaoA.estaPressionadoAgora() ? botao_a_estado = true : botao_a_estado = false;
    BotaoB.estaPressionadoAgora() ? botao_b_estado = true : botao_b_estado = false;
}

float obter_temperatura_interna()
{
    adc_select_input(ADC_INPUT_TEMPERATURA);
    uint16_t valor_bruto = adc_read();
    return TEMPERATURA_BASE - ((valor_bruto * ADC_FATOR_CONVERSAO) - TEMPERATURA_OFFSET) / TEMPERATURA_ESCALA;
}

void inicializar_sensor_temperatura()
{
    adc_init();
    adc_set_temp_sensor_enabled(true);
}