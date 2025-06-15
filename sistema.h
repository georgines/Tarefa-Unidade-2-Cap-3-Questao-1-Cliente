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
#include "clienteHttp.h"

// botoes
#define PIN_BOTAO_A 5
#define PIN_BOTAO_B 6

// wifi
#define WIFI_SSID "RECEPCAO"
#define WIFI_PASS "naotedouasenha1234"

// sensor de temperatura
#define ADC_INPUT_TEMPERATURA 4
#define ADC_FATOR_CONVERSAO (3.3f / (1 << 12))
#define TEMPERATURA_BASE 27.0f
#define TEMPERATURA_OFFSET 0.706f
#define TEMPERATURA_ESCALA 0.001721f

// status dos botões
extern bool botao_a_estado;
extern bool botao_b_estado;

ClienteHttp cliente;