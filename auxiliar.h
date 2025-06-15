#pragma once

#include "sistema.h"
#include "string"

void inicializar_stdio();

void resposta(const char *r, int t);

void erro(const char *msg);

// botoes
void monitorar_botoes();

// sensores
float obter_temperatura_interna();

void inicializar_sensor_temperatura();