# Projeto Tarefa U2 C3 Q1

## Propósito

Este projeto foi desenvolvido para monitorar botões, controlar LEDs e medir a temperatura interna utilizando um Raspberry Pi Pico W. Além disso, ele implementa um servidor HTTP que permite interagir com o dispositivo via uma interface web.

## Funcionalidades

### Monitoramento de Botões

- Detecta o estado dos botões físicos.

### Controle de LEDs

- Alternar o estado dos LEDs (azul, verde e vermelho) através de rotas HTTP.

### Sensor de Temperatura

- Mede a temperatura interna do microcontrolador.
### Conexão Wi-Fi

- Inicialização do módulo Wi-Fi.
- Conexão automática a uma rede Wi-Fi configurada.

### Servidor HTTP

- Servir uma página HTML para interação.
- Rotas para obter o status dos botões, temperatura e alternar LEDs.

## Estrutura do Código

### Principais Arquivos

- `tarefau2c31.cpp`: Função principal que inicializa os componentes e mantém o loop principal.
- `auxiliar.cpp` e `auxiliar.h`: Funções auxiliares para controle de LEDs e leitura de sensores.
- `auxiliarWifi.cpp` e `auxiliarWifi.h`: Funções para inicialização e conexão Wi-Fi.
- `servidor.cpp` e `servidor.h`: Implementação do servidor HTTP.
- `ServidorHttp.cpp` e `ServidorHttp.h`: Classe para gerenciar rotas e requisições HTTP.
- `sistema.h`: Definições de constantes e configurações do sistema.

### Configuração

- O projeto utiliza o SDK do Raspberry Pi Pico.
- Configuração de Wi-Fi com SSID e senha definidos em `sistema.h`.

## Como Compilar e Executar

1. Certifique-se de que o SDK do Raspberry Pi Pico está configurado.
2. Compile o projeto utilizando o comando de build.
3. Carregue o binário gerado no Raspberry Pi Pico W.

## Dependências

- Raspberry Pi Pico SDK
- Biblioteca LWIP para rede
- Ferramentas de build como CMake e Ninja

## Observações

Este projeto é ideal para aprendizado e demonstração de controle de hardware e comunicação via rede utilizando o Raspberry Pi Pico W.
