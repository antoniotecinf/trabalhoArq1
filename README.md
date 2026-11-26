# LootBox Guard: O baú que protege o loot

> Projeto prático de IoT e Segurança da Informação desenvolvido na disciplina de Algoritmos e Estrutura de Dados (CESAR School).

## 👥 Integrantes da Equipe

- Antonio Pedro — antonio.olima@upe.br
- Miguel Costa — miguel.costa@upe.br
- Ryan Ellesson — ryan.ellesson@upe.br

## 📖 1. Contexto e História (O Problema)

Em um jogo presencial, um baú guarda a recompensa de uma missão. O protótipo simulado controla quem pode retirar a recompensa e sinaliza se a tampa for aberta sem autorização.

## ⚙️ 2. Funcionalidades do Dispositivo

- [x] Liberação com cartão RFID cadastrado e PIN de quatro dígitos.
- [x] Bloqueio por 30 segundos após três PINs incorretos consecutivos.
- [x] Janela de cinco segundos para iniciar a abertura e travamento ao fechar a tampa.
- [x] Alarme sonoro enquanto a tampa estiver aberta indevidamente, inclusive durante o bloqueio do teclado.

O PIN e o UID cadastrados são dados de demonstração. O firmware compara o identificador do cartão. Os tempos são medidos com millis() no ESP32 virtual.

## 🛠️ 3. Especificações Técnicas e Componentes

- ESP32 DevKit v1, teclado matricial 4×3, leitor MFRC522, microservo e buzzer piezoelétrico.
- Interruptor deslizante simulando o estado da tampa. Esquerda = fechada. Direita = aberta.
- Wokwi, framework Arduino em C++ e bibliotecas Keypad, ESP32Servo, MFRC522 e SPI.

## 🔌 4. Diagrama de Conexões (Pinout)

O arquivo diagram.json contém as conexões do circuito simulado.

| Componente | Terminal | ESP32 |
|---|---|---|
| Teclado | R1, R2, R3, R4 | GPIO 32, 33, 25, 26 |
| Teclado | C1, C2, C3 | GPIO 27, 14, 13 |
| Servo | PWM, V+, GND | GPIO 23, VIN virtual, GND |
| MFRC522 | SDA/SS, SCK, MOSI, MISO, RST | GPIO 5, 18, 21, 19, 22 |
| MFRC522 | 3.3V, GND | 3V3, GND |
| Interruptor da tampa | 2 (comum), 1 (esquerda) | GPIO 4, GND |
| Buzzer | 2 (+), 1 (−) | GPIO 16 (RX2), GND |
| Monitor serial | RX | TX0 |

## 🚀 5. Instruções de Compilação e Gravação

1. Abra https://wokwi.com/projects/481675741334122497 e clique em Start Simulation. O Wokwi compila o código e inicia o ESP32 virtual.
2. Clique no MFRC522, selecione Blue Card e clique em TAP. O cartão cadastrado tem UID 01:02:03:04.
3. Digite 2580# no teclado virtual. Mova o interruptor para a direita antes de cinco segundos e depois volte para a esquerda.

### Resultados observados no Wokwi

| Teste | Resultado |
|---|---|
| PIN correto sem cartão | Acesso recusado |
| Green Card seguido do PIN correto | Cartão desconhecido e acesso recusado |
| Blue Card seguido de PIN incorreto | PIN recusado |
| Blue Card + 2580#, abrir e fechar a tampa | Acesso autorizado e servo volta à posição travada ao fechar |
| Blue Card + três PINs incorretos consecutivos | Bloqueio de 30 segundos. PIN correto ignorado durante a espera |
| Abrir a tampa travada, inclusive durante o bloqueio | Buzzer acionado e aviso no monitor serial. Fechar a tampa encerra o alarme |
