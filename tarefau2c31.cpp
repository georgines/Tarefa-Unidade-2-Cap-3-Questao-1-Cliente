#include <string.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"

#include "lwip/pbuf.h"
#include "lwip/tcp.h"
#include "lwip/dns.h"

#define WIFI_SSID "RECEPCAO"
#define WIFI_PASS "naotedouasenha1234"
#define TCP_SERVER_HOST "httpbin.org"
#define TCP_PORT 80
#define DEBUG_printf printf
#define HTTP_REQUEST_BUFFER_SIZE 2048

typedef enum {
    REQUEST_TYPE_GET,
    REQUEST_TYPE_POST
} REQUEST_TYPE;

typedef struct HTTP_CLIENT_STATE_T {
    struct tcp_pcb *tcp_pcb;
    ip_addr_t remote_addr;
    char request[HTTP_REQUEST_BUFFER_SIZE];
    uint16_t request_len;
    bool complete;
    int sent_len;
    REQUEST_TYPE type;
} HTTP_CLIENT_STATE_T;

static err_t fechar_conexao_http(void *arg);
static err_t ao_conectar_http(void *arg, struct tcp_pcb *tpcb, err_t err);
static err_t ao_receber_dados_http(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err);
static void ao_ocorrer_erro_http(void *arg, err_t err);
static err_t ao_enviar_dados_http(void *arg, struct tcp_pcb *tpcb, u16_t len);
static void ao_resolver_dns(const char *name, const ip_addr_t *ipaddr, void *arg);
static const char* depurar_erro(err_enum_t erro);

static err_t fechar_conexao_http(void *arg) {
    HTTP_CLIENT_STATE_T *estado = (HTTP_CLIENT_STATE_T *)arg;
    err_t erro = ERR_OK;
    if (estado->tcp_pcb != NULL) {
        tcp_arg(estado->tcp_pcb, NULL);
        tcp_poll(estado->tcp_pcb, NULL, 0);
        tcp_sent(estado->tcp_pcb, NULL);
        tcp_recv(estado->tcp_pcb, NULL);
        tcp_err(estado->tcp_pcb, NULL);
        erro = tcp_close(estado->tcp_pcb);
        DEBUG_printf("tcp_close retornou: %s\n", depurar_erro((err_enum_t)erro));
        if (erro != ERR_OK) {
            DEBUG_printf("Falha ao fechar o pcb: %s\n", depurar_erro((err_enum_t)erro));
        } else {
            estado->tcp_pcb = NULL;
        }
    }
    estado->complete = true;
    return erro;
}

static err_t ao_receber_dados_http(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err) {
    HTTP_CLIENT_STATE_T *estado = (HTTP_CLIENT_STATE_T *)arg;
    if (!p) {
        DEBUG_printf("Conexão fechada pelo servidor.\n");
        return fechar_conexao_http(arg);
    }

    if (p->tot_len > 0) {
        DEBUG_printf("--- DADOS RECEBIDOS (%d bytes) ---\n", p->tot_len);
        for (struct pbuf *q = p; q != NULL; q = q->next) {
            printf("%.*s", q->len, (char *)q->payload);
        }
        printf("\n--- FIM DOS DADOS ---\n");
        tcp_recved(tpcb, p->tot_len);
    }
    pbuf_free(p);
    return ERR_OK;
}

static err_t ao_conectar_http(void *arg, struct tcp_pcb *tpcb, err_t err) {
    HTTP_CLIENT_STATE_T *estado = (HTTP_CLIENT_STATE_T *)arg;
    DEBUG_printf("ao_conectar_http recebeu err: %s\n", depurar_erro((err_enum_t)err));
    if (err != ERR_OK) {
        DEBUG_printf("Falha ao conectar: %s\n", depurar_erro((err_enum_t)err));
        return fechar_conexao_http(arg);
    }

    const char* tipo_requisicao = (estado->type == REQUEST_TYPE_GET) ? "GET" : "POST";
    DEBUG_printf("Conectado ao servidor. Enviando requisição %s...\n", tipo_requisicao);

    estado->sent_len = 0;

    DEBUG_printf("➡️ Iniciando envio via tcp_write...\n");
    err_t erro_escrita = tcp_write(tpcb, estado->request, estado->request_len, TCP_WRITE_FLAG_COPY);
    DEBUG_printf("tcp_write retornou: %s\n", depurar_erro((err_enum_t)erro_escrita));

    if (erro_escrita != ERR_OK) {
        DEBUG_printf("Falha ao escrever no buffer TCP.\n");
        return fechar_conexao_http(arg);
    }

    DEBUG_printf("➡️ Chamando tcp_output...\n");
    err_t erro_saida = tcp_output(tpcb);
    DEBUG_printf("tcp_output retornou: %s\n", depurar_erro((err_enum_t)erro_saida));

    if (erro_saida != ERR_OK) {
        DEBUG_printf("Falha ao chamar tcp_output.\n");
        return fechar_conexao_http(arg);
    }

    return ERR_OK;
}

static void ao_resolver_dns(const char *name, const ip_addr_t *ipaddr, void *arg) {
    HTTP_CLIENT_STATE_T *estado = (HTTP_CLIENT_STATE_T *)arg;
    if (ipaddr) {
        estado->remote_addr = *ipaddr;
        DEBUG_printf("DNS resolvido para: %s\n", ip4addr_ntoa(ipaddr));

        estado->tcp_pcb = tcp_new();
        if (!estado->tcp_pcb) {
            DEBUG_printf("Falha ao criar o pcb.\n");
            estado->complete = true;
            return;
        }

        tcp_arg(estado->tcp_pcb, estado);
        tcp_sent(estado->tcp_pcb, ao_enviar_dados_http);
        tcp_recv(estado->tcp_pcb, ao_receber_dados_http);
        tcp_err(estado->tcp_pcb, ao_ocorrer_erro_http);

        cyw43_arch_lwip_begin();
        err_t erro = tcp_connect(estado->tcp_pcb, &estado->remote_addr, TCP_PORT, ao_conectar_http);
        cyw43_arch_lwip_end();

        DEBUG_printf("tcp_connect retornou: %s\n", depurar_erro((err_enum_t)erro));

        if (erro != ERR_OK) {
            DEBUG_printf("Falha ao iniciar a conexão: %s\n", depurar_erro((err_enum_t)erro));
            fechar_conexao_http(estado);
        }
    } else {
        DEBUG_printf("Falha na resolução DNS.\n");
        estado->complete = true;
    }
}

static bool run_http_request(HTTP_CLIENT_STATE_T *state) {
    cyw43_arch_lwip_begin();
    err_t err = dns_gethostbyname(TCP_SERVER_HOST, &state->remote_addr, ao_resolver_dns, state);
    cyw43_arch_lwip_end();

    DEBUG_printf("dns_gethostbyname retornou: %s\n", depurar_erro((err_enum_t)err));

    if (err == ERR_OK) {
    } else if (err != ERR_INPROGRESS) {
        DEBUG_printf("Falha ao iniciar a consulta DNS: %s\n", depurar_erro((err_enum_t)err));
        state->complete = true;
    }

    while (!state->complete) {
        sleep_ms(100);
    }

    free(state);
    return true;
}

static err_t ao_enviar_dados_http(void *arg, struct tcp_pcb *tpcb, u16_t len) {
    HTTP_CLIENT_STATE_T *state = (HTTP_CLIENT_STATE_T *)arg;
    state->sent_len += len;
    DEBUG_printf("Dados enviados: %d bytes\n", len);
    return ERR_OK;
}

static void ao_ocorrer_erro_http(void *arg, err_t err) {
    HTTP_CLIENT_STATE_T *state = (HTTP_CLIENT_STATE_T *)arg;
    DEBUG_printf("Erro na conexão TCP: %s\n", lwip_strerr(err));
    fechar_conexao_http(arg);
}

static const char* depurar_erro(err_enum_t erro) {
    switch (erro) {
        case ERR_OK: return "Nenhum erro, tudo OK.";
        case ERR_MEM: return "Erro de memória.";
        case ERR_BUF: return "Erro de buffer.";
        case ERR_TIMEOUT: return "Timeout.";
        case ERR_RTE: return "Problema de roteamento.";
        case ERR_INPROGRESS: return "Operação em andamento.";
        case ERR_VAL: return "Valor ilegal.";
        case ERR_WOULDBLOCK: return "Operação bloquearia.";
        case ERR_USE: return "Endereço em uso.";
        case ERR_ALREADY: return "Já conectando.";
        case ERR_ISCONN: return "Conexão já estabelecida.";
        case ERR_CONN: return "Não conectado.";
        case ERR_IF: return "Erro de interface de rede.";
        case ERR_ABRT: return "Conexão abortada.";
        case ERR_RST: return "Conexão resetada.";
        case ERR_CLSD: return "Conexão fechada.";
        case ERR_ARG: return "Argumento ilegal.";
        default: return "Erro desconhecido.";
    }
}

int main() {
    stdio_init_all();
    DEBUG_printf("Inicializando...\n");

    if (cyw43_arch_init()) {
        DEBUG_printf("Falha ao inicializar o Wi-Fi\n");
        return 1;
    }
    cyw43_arch_enable_sta_mode();

    DEBUG_printf("Conectando ao Wi-Fi: %s\n", WIFI_SSID);
    if (cyw43_arch_wifi_connect_timeout_ms(WIFI_SSID, WIFI_PASS, CYW43_AUTH_WPA2_AES_PSK, 30000)) {
        DEBUG_printf("Falha ao conectar ao Wi-Fi.\n");
        cyw43_arch_deinit();
        return 1;
    }
    DEBUG_printf("Conectado com sucesso!\n");

    auto executar_requisicao = [](REQUEST_TYPE tipo, const char* conteudo_post = nullptr) {
        HTTP_CLIENT_STATE_T* estado = (HTTP_CLIENT_STATE_T*)calloc(1, sizeof(HTTP_CLIENT_STATE_T));
        if (!estado) {
            DEBUG_printf("Falha ao alocar estado\n");
            return;
        }
        estado->type = tipo;

        if (tipo == REQUEST_TYPE_GET) {
            estado->request_len = snprintf(estado->request, sizeof(estado->request),
                "GET /get?param1=value1&param2=value2 HTTP/1.1\r\n"
                "Host: %s\r\n"
                "Connection: close\r\n"
                "\r\n",
                TCP_SERVER_HOST
            );
        } else {
            if (!conteudo_post) conteudo_post = "{}";
            estado->request_len = snprintf(estado->request, sizeof(estado->request),
                "POST /post HTTP/1.1\r\n"
                "Host: %s\r\n"
                "Content-Type: application/json\r\n"
                "Content-Length: %d\r\n"
                "Connection: close\r\n"
                "\r\n"
                "%s",
                TCP_SERVER_HOST, (int)strlen(conteudo_post), conteudo_post
            );
        }

        // DNS com tratamento de retorno instantâneo
        cyw43_arch_lwip_begin();
        err_t err = dns_gethostbyname(TCP_SERVER_HOST, &estado->remote_addr, ao_resolver_dns, estado);
        cyw43_arch_lwip_end();

        DEBUG_printf("dns_gethostbyname retornou: %s\n", depurar_erro((err_enum_t)err));

        if (err == ERR_OK) {
            ao_resolver_dns(TCP_SERVER_HOST, &estado->remote_addr, estado);
        } else if (err != ERR_INPROGRESS) {
            DEBUG_printf("Falha ao iniciar a consulta DNS: %s\n", depurar_erro((err_enum_t)err));
            estado->complete = true;
        }

        while (!estado->complete) {
            sleep_ms(100);
        }

        free(estado);
    };

    DEBUG_printf("\n--- REQUISIÇÃO GET ---\n");
    executar_requisicao(REQUEST_TYPE_GET);

    DEBUG_printf("\n--- REQUISIÇÃO POST ---\n");
    const char *dados_post = "{\"sensor\":\"Pico W\", \"linguagem\":\"C++\", \"param1\":\"value1\", \"param2\":\"value2\"}";
    executar_requisicao(REQUEST_TYPE_POST, dados_post);



     DEBUG_printf("\n--- REQUISIÇÃO GET2 ---\n");
    executar_requisicao(REQUEST_TYPE_GET);

     DEBUG_printf("\n--- REQUISIÇÃO POST2 ---\n");
    const char *dados_post2 = "{\"sensor\":\"Pico W\", \"linguagem\":\"C++\", \"param1\":\"value1\", \"param2\":\"value2\"}";
    executar_requisicao(REQUEST_TYPE_POST, dados_post2);

    DEBUG_printf("\nTestes concluídos.\n");
    cyw43_arch_deinit();
    return 0;
}

