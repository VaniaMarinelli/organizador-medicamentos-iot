# organizador-medicamentos-iot

# Organizador Inteligente de Medicamentos (IoT)

## 📌 Descrição do Projeto
Este projeto consiste num protótipo de um Organizador Inteligente de Medicamentos baseado em Internet das Coisas (IoT), desenvolvido para apoiar o Objetivo de Desenvolvimento Sustentável 3 (Saúde e Bem-Estar). O sistema emite alertas sonoros e visuais nos horários programados, exige a confirmação física do paciente (botão) e regista a abertura do compartimento de medicação, enviando todos os dados em tempo real via internet para um servidor.

## 🛠️ Descrição do Hardware Utilizado
O projeto foi prototipado e validado no ambiente de simulação Wokwi. A arquitetura física é composta pelos seguintes componentes:
* **Microcontrolador:** Placa ESP32 DevKit V1 (responsável pelo processamento e conectividade Wi-Fi nativa).
* **Sensores:** 1x Módulo Fotoresistor LDR (para detetar a entrada de luz e identificar a abertura da caixa).
* **Atuadores:** 1x Buzzer piezoelétrico (alerta sonoro) e 1x LED de 5mm com resistor de 220 Ohm (alerta visual).
* **Entrada (Interface Humana):** 1x Botão Tátil (Pushbutton) para confirmação de ingestão do medicamento.

## 🌐 Interfaces, Protocolos e Comunicação
Para garantir a conectividade remota exigida pelo projeto, foram utilizadas as seguintes tecnologias:
* **Rede:** Comunicação via Internet através da pilha TCP/IP (módulo Wi-Fi do ESP32).
* **Protocolo IoT:** MQTT (Message Queuing Telemetry Transport) rodando na porta 1883.
* **Broker:** HiveMQ (Servidor público `broker.hivemq.com`).
* **Tópicos MQTT Utilizados:** 
  * `medicamento/alerta` (Publicação)
  * `medicamento/abertura` (Publicação)
  * `medicamento/confirmacao` (Publicação)
  * `medicamento/comando` (Subscrição para acionamento remoto)
* **Sincronização de Tempo:** Protocolo NTP (Network Time Protocol) acedendo ao servidor `pool.ntp.org` para obter a hora real da internet.

## 💻 Software e Documentação do Código
O software/firmware foi desenvolvido em linguagem C++ utilizando o framework do Arduino. O código-fonte completo está disponível neste repositório no ficheiro `sketch.ino`. O código está estruturado em funções e devidamente comentado, utilizando a biblioteca `PubSubClient` para a gestão do protocolo MQTT e integração das interrupções de hardware com a comunicação web.

## 🚀 Como reproduzir o projeto (Guia de Uso)
1. **Ambiente:** Aceda ao simulador [Wokwi](https://wokwi.com/) e inicie um projeto em branco com o ESP32.
2. **Montagem:** Conecte o LDR ao pino D34, o Botão ao pino D33, o LED ao D2 e o Buzzer ao pino D4. Ligue todos os componentes ao GND da placa.
3. **Código:** Copie o conteúdo do ficheiro `sketch.ino` e cole no editor do Wokwi. Adicione a biblioteca `PubSubClient` no gestor de bibliotecas do simulador.
4. **Execução:** Dê o "Play" na simulação. O ESP32 irá conectar-se automaticamente à rede virtual e ao HiveMQ.
5. **Monitorização:** Abra o [HiveMQ Websocket Client](http://www.hivemq.com/demos/websocket-client/), clique em Connect e subscreva ao tópico `medicamento/#` para visualizar as mensagens de abertura da caixa e confirmação do botão em tempo real.
