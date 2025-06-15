#include "sistema.h"
#include "auxiliarWifi.h"
#include "auxiliar.h"

ClienteHttp cliente;

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
        cliente.post("httpbin.org", "/post", conteudo, nullptr, "application/json", resposta, erro);
        sleep_ms(1000);
        printf("\n\n\n\n");
    }
    return 0;
}
