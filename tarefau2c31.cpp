#include "clienteHttp.h"
#include "auxiliarWifi.h"

void resposta(const char *r, int t) { printf("Resposta (%d):\n%.*s\n", t, t, r); }
void erro(const char *msg) { printf("Erro: %s\n", msg); }

int main()
{
    stdio_init_all();
    inicializar_wifi();

    while (1)
    {

        printf("\nGET 1:\n");
        cliente_http_executar(TIPO_GET, "httpbin.org", "/get", NULL, NULL, resposta, erro);

        sleep_ms(100);

        printf("\nPOST 1:\n");
        cliente_http_executar(TIPO_POST, "httpbin.org", "/post", "{\"valor\":1}", NULL, resposta, erro);
        sleep_ms(100);

        printf("\nGET 2:\n");
        cliente_http_executar(TIPO_GET, "httpbin.org", "/get", NULL, NULL, resposta, erro);
        sleep_ms(100);

        printf("\nPOST 2:\n");
        cliente_http_executar(TIPO_POST, "httpbin.org", "/post", "{\"valor\":2}", NULL, resposta, erro);
        sleep_ms(100);
    }

    return 0;
}
