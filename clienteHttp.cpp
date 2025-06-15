#include "clienteHttp.h"
#include <string.h>
#include <stdio.h>
#include <algorithm>

static const char MENSAGEM_ERRO_CONEXAO[] __in_flash("erro_conexao") = "Erro de conexao";
static const char MENSAGEM_ERRO_DNS[] __in_flash("erro_dns") = "Erro DNS";
static const char TEXTO_VAZIO_POST[] __in_flash("vazio_post") = "{}";

static err_t fechar_conexao(void *arg);
static err_t ao_receber(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err);
static err_t ao_conectar(void *arg, struct tcp_pcb *tpcb, err_t err);
static void ao_erro(void *arg, err_t err);
static err_t ao_enviar(void *arg, struct tcp_pcb *tpcb, u16_t len);
static void ao_resolver_dns(const char *nome, const ip_addr_t *ip, void *arg);

static err_t fechar_conexao(void *arg)
{
    auto *estado = static_cast<EstadoClienteHttp *>(arg);
    if (!estado->pcb)
        return ERR_OK;

    tcp_arg(estado->pcb, nullptr);
    tcp_poll(estado->pcb, nullptr, 0);
    tcp_recv(estado->pcb, nullptr);
    tcp_sent(estado->pcb, nullptr);
    tcp_err(estado->pcb, nullptr);
    tcp_close(estado->pcb);
    estado->pcb = nullptr;
    estado->completo = true;
    return ERR_OK;
}

static err_t ao_receber(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err)
{
    auto *estado = static_cast<EstadoClienteHttp *>(arg);
    if (!p)
        return fechar_conexao(arg);

    if (p->tot_len > 0 && estado->cb_resposta)
    {
        int offset = 0;
        for (struct pbuf *q = p; q != nullptr; q = q->next)
        {
            int len = std::min(static_cast<int>(q->len), TAM_BUFFER_REQUISICAO_HTTP - offset);
            memcpy(estado->buffer + offset, q->payload, len);
            offset += len;
        }
        estado->buffer[std::min(offset, TAM_BUFFER_REQUISICAO_HTTP - 1)] = '\0';
        estado->cb_resposta(estado->buffer, offset);
        tcp_recved(tpcb, p->tot_len);
    }

    pbuf_free(p);
    return ERR_OK;
}

static err_t ao_conectar(void *arg, struct tcp_pcb *tpcb, err_t err)
{
    auto *estado = static_cast<EstadoClienteHttp *>(arg);
    if (err != ERR_OK)
        return fechar_conexao(arg);

    if (tcp_write(tpcb, estado->buffer, estado->tamanho, TCP_WRITE_FLAG_COPY) != ERR_OK)
        return fechar_conexao(arg);

    cyw43_arch_lwip_begin();
    err_t erro = tcp_output(tpcb);
    cyw43_arch_lwip_end();
    return erro;
}

static void ao_erro(void *arg, err_t err)
{
    auto *estado = static_cast<EstadoClienteHttp *>(arg);
    if (estado->cb_erro)
        estado->cb_erro(MENSAGEM_ERRO_CONEXAO);
    estado->completo = true;
}

static err_t ao_enviar(void *arg, struct tcp_pcb *tpcb, u16_t len)
{
    auto *estado = static_cast<EstadoClienteHttp *>(arg);
    estado->enviados += len;
    return ERR_OK;
}

static void ao_resolver_dns(const char *nome, const ip_addr_t *ip, void *arg)
{
    auto *estado = static_cast<EstadoClienteHttp *>(arg);
    if (!ip)
    {
        estado->completo = true;
        return;
    }

    estado->pcb = tcp_new();
    if (!estado->pcb)
    {
        estado->completo = true;
        return;
    }

    tcp_arg(estado->pcb, estado);
    tcp_sent(estado->pcb, ao_enviar);
    tcp_recv(estado->pcb, ao_receber);
    tcp_err(estado->pcb, ao_erro);

    cyw43_arch_lwip_begin();
    if (tcp_connect(estado->pcb, ip, 80, ao_conectar) != ERR_OK)
        estado->completo = true;
    cyw43_arch_lwip_end();
}

int montar_requisicao_http(char *buffer, size_t tamanho_buffer, const char *metodo, const char *caminho, const char *host, const char *cabecalhos, const char *corpo, const char *content_type)
{
    if (!corpo)
        corpo = TEXTO_VAZIO_POST;

    return snprintf(buffer, tamanho_buffer,
                    "%s %s HTTP/1.1\r\n"
                    "Host: %s\r\n"
                    "%s"
                    "Content-Type: %s\r\n"
                    "Content-Length: %d\r\n"
                    "Connection: close\r\n\r\n"
                    "%s",
                    metodo, caminho, host,
                    cabecalhos ? cabecalhos : "",
                    content_type ? content_type : "application/json",
                    (int)strlen(corpo), corpo);
}

ClienteHttp::ClienteHttp()
{
    memset(&estado, 0, sizeof(estado));
}

void ClienteHttp::get(const char *host, const char *caminho, const char *cabecalhos, CallbackRespostaHttp on_resposta, CallbackErroHttp on_erro)
{
    executar("GET", host, caminho, nullptr, cabecalhos, "application/json", on_resposta, on_erro);
}

void ClienteHttp::post(const char *host, const char *caminho, const char *conteudo_post, const char *cabecalhos, const char *content_type, CallbackRespostaHttp on_resposta, CallbackErroHttp on_erro)
{
    executar("POST", host, caminho, conteudo_post, cabecalhos, content_type, on_resposta, on_erro);
}

void ClienteHttp::executar(const char *metodo, const char *host, const char *caminho, const char *conteudo_post, const char *cabecalhos, const char *content_type, CallbackRespostaHttp on_resposta, CallbackErroHttp on_erro)
{
    estado.cb_resposta = on_resposta;
    estado.cb_erro = on_erro;

    estado.tamanho = montar_requisicao_http(estado.buffer,
                                            TAM_BUFFER_REQUISICAO_HTTP,
                                            metodo,
                                            caminho,
                                            host,
                                            cabecalhos,
                                            conteudo_post,
                                            content_type);

    cyw43_arch_lwip_begin();
    err_t err = dns_gethostbyname(host, &estado.endereco, ao_resolver_dns, &estado);
    cyw43_arch_lwip_end();

    if (err == ERR_OK)
    {
        ao_resolver_dns(host, &estado.endereco, &estado);
    }
    else if (err != ERR_INPROGRESS)
    {
        if (estado.cb_erro)
        {
            estado.cb_erro(MENSAGEM_ERRO_DNS);
        }
        estado.completo = true;
    }

    while (!estado.completo)
    {
        sleep_ms(10);
    }
}
