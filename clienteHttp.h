#pragma once

#include "pico/cyw43_arch.h"
#include "lwip/tcp.h"
#include "lwip/dns.h"

#define TAM_BUFFER_REQUISICAO_HTTP 1024

typedef enum
{
    TIPO_GET,
    TIPO_POST
} TipoRequisicaoHttp;

typedef void (*CallbackRespostaHttp)(const char *resposta, int tamanho);
typedef void (*CallbackErroHttp)(const char *erro);

void cliente_http_executar(TipoRequisicaoHttp tipo,
                           const char *host,
                           const char *caminho,
                           const char *conteudo_post,
                           const char *cabecalhos,
                           CallbackRespostaHttp on_resposta,
                           CallbackErroHttp on_erro);

int montar_requisicao_http(char *buffer, size_t tamanho_buffer, const char *metodo, const char *caminho, const char *host, const char *cabecalhos, const char *corpo, const char *content_type);

typedef struct
{
    struct tcp_pcb *pcb;
    ip_addr_t endereco;
    char buffer[TAM_BUFFER_REQUISICAO_HTTP];
    int tamanho;
    int enviados;
    bool completo;
    TipoRequisicaoHttp tipo;
    CallbackRespostaHttp cb_resposta;
    CallbackErroHttp cb_erro;
} EstadoClienteHttp;

class ClienteHttp
{
public:
    ClienteHttp();
    void get(const char *host, const char *caminho, const char *cabecalhos, CallbackRespostaHttp on_resposta, CallbackErroHttp on_erro);
    void post(const char *host, const char *caminho, const char *conteudo_post, const char *cabecalhos, const char *content_type, CallbackRespostaHttp on_resposta, CallbackErroHttp on_erro);

private:
    EstadoClienteHttp estado;
    void executar(const char *metodo,
                  const char *host,
                  const char *caminho,
                  const char *conteudo_post,
                  const char *cabecalhos,
                  const char *content_type,
                  CallbackRespostaHttp on_resposta,
                  CallbackErroHttp on_erro);
};
