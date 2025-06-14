#include "clienteHttp.h"
#include "auxiliarWifi.h"

void resposta(const char *r, int t) { printf("Resposta (%d):\n%.*s\n", t, t, r); }
void erro(const char *msg) { printf("Erro: %s\n", msg); }

int main()
{
    stdio_init_all();
    inicializar_wifi();

    ClienteHttp cliente;

    while (1)
    {
        printf("\nGET 1:\n");
        cliente.get("httpbin.org", "/get?maria=teste&georgines=75", nullptr, resposta, erro);

        sleep_ms(1000);

        printf("\nPOST 1:\n");
        cliente.post("httpbin.org", "/post", "{\"valor\":1}", nullptr, "application/json", resposta, erro);

        sleep_ms(1000);

        printf("\nGET 2:\n");
        cliente.get("httpbin.org", "/get", nullptr, resposta, erro);
        sleep_ms(1000);

        printf("\nPOST 2:\n");
        cliente.post("httpbin.org", "/post", "{\"valor\":2}", nullptr, "application/json", resposta, erro);
        sleep_ms(1000);

        printf("\nPOST JSON:\n");
        cliente.post("httpbin.org", "/post", "{\"valor\":1}", nullptr, "application/json", resposta, erro);
        sleep_ms(1000);

        printf("\nPOST Non-JSON:\n");
        cliente.post("httpbin.org", "/post", "valor=1&outro=2", nullptr, "application/x-www-form-urlencoded", resposta, erro);
        sleep_ms(1000);
    }

    return 0;
}
