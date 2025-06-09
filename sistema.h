#pragma once

#include <stdio.h>
#include <string.h>
#include <functional>
#include "hardware/adc.h"
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include "pico/platform/sections.h"
#include "lwip/tcp.h"
#include "lwip/netif.h"
#include "Acionador.h"

// botoes
#define PIN_BOTAO_A 5
#define PIN_BOTAO_B 6

// leds
#define LED_AZUL 12
#define LED_VERDE 11
#define LED_VERMELHO 13

// wifi
#define WIFI_SSID "RECEPCAO"
#define WIFI_PASS "naotedouasenha1234"

// webserver
#define PORTA_HTTP 80
#define TAMANHO_HTML 2048

// sensor de temperatura
#define ADC_INPUT_TEMPERATURA 4
#define ADC_FATOR_CONVERSAO (3.3f / (1 << 12))
#define TEMPERATURA_BASE 27.0f
#define TEMPERATURA_OFFSET 0.706f
#define TEMPERATURA_ESCALA 0.001721f

// status dos botões
extern bool botao_a_estado;
extern bool botao_b_estado;

// pagina html index
static const char body_fmt[] __in_flash("body") =
"<!DOCTYPE html>\n"
"<html>\n"
"<head>\n"
"  <meta charset=\"utf-8\">\n"
"  <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n"
"  <title>Painel de Controle</title>\n"
"  <style>\n"
"    button {\n"
"      font-size: 1.2em;\n"
"      padding: 10px 20px;\n"
"      margin: 5px;\n"
"      white-space: nowrap;\n"
"    }\n"
"    #botao_azul { background-color: #007BFF; color: white; }\n"
"    #botao_verde { background-color: #28A745; color: white; }\n"
"    #botao_vermelho { background-color: #DC3545; color: white; }\n"
"  </style>\n"
"</head>\n"
"<body>\n"
"  <h1>Painel de Controle: LEDs, Temperatura e Botões</h1>\n"
"  <h2>Sem necessidade de atualizar a página</h2>\n"
"  <button id=\"botao_azul\" onclick=\"alternarLed('az', this)\">Ligar Azul</button>\n"
"  <button id=\"botao_verde\" onclick=\"alternarLed('vr', this)\">Ligar Verde</button>\n"
"  <button id=\"botao_vermelho\" onclick=\"alternarLed('vm', this)\">Ligar Vermelho</button>\n"
"  <p class=\"temperatura\">Temperatura: <span id=\"temperatura\">--</span> &deg;C</p>\n"
"  <p>Status Botão A: <span id=\"botao_a\">--</span></p>\n"
"  <p>Status Botão B: <span id=\"botao_b\">--</span></p>\n"
"  <script>\n"
"    function alternarLed(cor, botao) {\n"
"      fetch(`/${cor}/toggle`).then(res => res.json()).then(data => {\n"
"        const nome = cor === 'az' ? 'Azul' : cor === 'vr' ? 'Verde' : 'Vermelho';\n"
"        botao.textContent = data.ligado ? `Desligar ${nome}` : `Ligar ${nome}`;\n"
"      });\n"
"    }\n"
"    function updateStatus() {\n"
"      fetch('/status').then(r => r.json()).then(data => {\n"
"        document.getElementById('temperatura').textContent = data.temperatura.toFixed(2);\n"
"        document.getElementById('botao_a').textContent = data.botao_a ? 'Pressionado' : 'Solto';\n"
"        document.getElementById('botao_b').textContent = data.botao_b ? 'Pressionado' : 'Solto';\n"
"      });\n"
"    }\n"
"    setInterval(updateStatus, 1000);\n"
"  </script>\n"
"</body>\n"
"</html>\n";
