#include "clienteHttp.h"
#include "auxiliarWifi.h"
#include "auxiliar.h"

ClienteHttp cliente;

void resposta(const char *r, int t) { printf("Resposta (%d):\n%.*s\n", t, t, r); }
void erro(const char *msg) { printf("Erro: %s\n", msg); }

void inicializar_stdio()
{
    stdio_init_all();
    printf("Iniciando monitor de botões...\n");
}

int main()
{
    inicializar_stdio();
    inicializar_sensor_temperatura();
    inicializar_wifi();

    while (true)
    {
        printf("Monitorando botões e temperatura...\n");
        monitorar_botoes();
        float temperatura = obter_temperatura_interna();
        char conteudo[256];
        snprintf(conteudo, sizeof(conteudo), "Temperatura interna: %.2f °C\nBotao A: %s\nBotao B: %s\n",
                 temperatura,
                 botao_a_estado ? "Pressionado" : "Liberado",
                 botao_b_estado ? "Pressionado" : "Liberado");
        // printf("%s", conteudo);
        cliente.post("httpbin.org", "/post", conteudo, nullptr, "application/json", resposta, erro);
        sleep_ms(1000);
        printf("\n\n\n\n");
    }
    return 0;
}
