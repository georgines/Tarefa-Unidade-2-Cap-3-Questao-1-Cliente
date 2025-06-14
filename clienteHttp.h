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

int montar_requisicao_http(char *buffer, size_t tamanho_buffer, const char *metodo, const char *caminho, const char *host, const char *cabecalhos, const char *corpo);
