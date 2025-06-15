# Projeto Tarefa U2 C3 Q1 Cliente

## Propósito

Este projeto foi desenvolvido para monitorar o estado de botões físicos e medir a temperatura interna utilizando um Raspberry Pi Pico W. Além disso, ele implementa um cliente HTTP que envia os dados coletados para um servidor remoto para validação e depuração.

## Funcionalidades

### Monitoramento de Botões

- Detecta o estado dos botões físicos (pressionado ou liberado).

### Sensor de Temperatura

- Mede a temperatura interna do microcontrolador.

### Conexão Wi-Fi

- Inicialização do módulo Wi-Fi.
- Conexão automática a uma rede Wi-Fi configurada.

### Cliente HTTP

- Envia os dados de temperatura e estado dos botões para um servidor remoto.
- Trata as respostas HTTP do servidor.

## Estrutura do Código

### Principais Arquivos

- `tarefau2c31_cliente.cpp`: Função principal que inicializa os componentes e mantém o loop principal.
- `auxiliar.cpp` e `auxiliar.h`: Funções auxiliares para monitoramento de botões e leitura de sensores.
- `auxiliarWifi.cpp` e `auxiliarWifi.h`: Funções para inicialização e conexão Wi-Fi.
- `clienteHttp.cpp` e `clienteHttp.h`: Implementação do cliente HTTP.
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

## Envio de Dados para o Servidor

Os dados de temperatura e estado dos botões são enviados para o site de testes `httpbin.org`. Este site é utilizado para testar e depurar requisições HTTP. Ele retorna informações detalhadas sobre a requisição recebida, permitindo verificar se os dados foram enviados corretamente.

### Objetivo do Envio

O objetivo de enviar os dados para `httpbin.org` é validar a funcionalidade do cliente HTTP implementado no projeto. Isso inclui verificar a formatação da requisição, o envio correto dos dados e o tratamento das respostas.

### Resposta do Servidor

Quando os dados são enviados para o endpoint `/post` do `httpbin.org`, o servidor retorna um JSON contendo informações como:
- Os cabeçalhos da requisição.
- O corpo da requisição enviado.
- Informações adicionais sobre a conexão.

Exemplo de resposta:
```json
{
  "args": {},
  "data": "Temperatura interna: 25.50 °C\nBotao A: Pressionado\nBotao B: Liberado\n",
  "files": {},
  "form": {},
  "headers": {
    "Content-Length": "75",
    "Content-Type": "application/json",
    "Host": "httpbin.org"
  },
  "json": null,
  "origin": "192.168.1.100",
  "url": "https://httpbin.org/post"
}
```
