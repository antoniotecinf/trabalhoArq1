#include <Arduino.h>
#include <Keypad.h>
#include <ESP32Servo.h>
#include <SPI.h>
#include <MFRC522.h>

constexpr byte pinoTampa = 4;
constexpr byte pinoBuzzer = 16;
constexpr byte pinoServo = 23;
constexpr byte pinoChipSelect = 5;
constexpr byte pinoReset = 22;
constexpr byte pinoClock = 18;
constexpr byte pinoMiso = 19;
constexpr byte pinoMosi = 21;
constexpr byte limiteErros = 3;
constexpr unsigned long tempoBloqueio = 30000;
constexpr unsigned long tempoPin = 30000;
constexpr unsigned long tempoAbertura = 5000;
const byte uidCadastrado[] = {0x01, 0x02, 0x03, 0x04};
const char pinCorreto[] = "2580";

char teclas[4][3] = {{'1','2','3'},{'4','5','6'},{'7','8','9'},{'*','0','#'}};
byte linhas[] = {32, 33, 25, 26};
byte colunas[] = {27, 14, 13};
Keypad teclado(makeKeymap(teclas), linhas, colunas, 4, 3);
MFRC522 leitor(pinoChipSelect, pinoReset);
Servo trava;

enum class EstadoTampa { Travada, Liberada, Aberta, Violada };
EstadoTampa estado = EstadoTampa::Travada;
unsigned long liberadaEm = 0;
unsigned long bloqueadaEm = 0;
unsigned long cartaoLidoEm = 0;
bool bloqueado = false;
bool cartaoAceito = false;
byte erros = 0;
char entrada[5] = {};
byte tamanho = 0;
bool excedeu = false;

void limparPin() {
  tamanho = 0;
  entrada[0] = '\0';
  excedeu = false;
}

void atualizarTampa(unsigned long agora) {
  const bool aberta = digitalRead(pinoTampa) == HIGH;
  switch (estado) {
    case EstadoTampa::Travada:
      if (aberta) {
        estado = EstadoTampa::Violada;
        cartaoAceito = false;
        limparPin();
        tone(pinoBuzzer, 2200);
        Serial.println("ALARME: abertura indevida!");
      }
      break;
    case EstadoTampa::Liberada:
      if (aberta) {
        estado = EstadoTampa::Aberta;
        Serial.println("Tampa aberta com autorizacao.");
      } else if (agora - liberadaEm >= tempoAbertura) {
        trava.write(10);
        estado = EstadoTampa::Travada;
        Serial.println("Prazo de abertura encerrado. Baú travado.");
      }
      break;
    case EstadoTampa::Aberta:
    case EstadoTampa::Violada:
      if (!aberta) {
        if (estado == EstadoTampa::Violada) noTone(pinoBuzzer);
        trava.write(10);
        estado = EstadoTampa::Travada;
        Serial.println("Tampa fechada. Baú travado.");
      }
      break;
  }
}

void lerCartao(unsigned long agora) {
  if (!leitor.PICC_IsNewCardPresent() || !leitor.PICC_ReadCardSerial()) return;
  cartaoAceito = leitor.uid.size == sizeof uidCadastrado &&
                 memcmp(leitor.uid.uidByte, uidCadastrado, sizeof uidCadastrado) == 0;
  cartaoLidoEm = agora;
  limparPin();
  leitor.PICC_HaltA();
  Serial.println(cartaoAceito ? "Cartao aceito. Digite o PIN." : "Cartao desconhecido.");
}

void setup() {
  Serial.begin(115200);
  pinMode(pinoTampa, INPUT_PULLUP);
  pinMode(pinoBuzzer, OUTPUT);
  trava.setPeriodHertz(50);
  trava.attach(pinoServo);
  trava.write(10);
  SPI.begin(pinoClock, pinoMiso, pinoMosi, pinoChipSelect);
  leitor.PCD_Init();
  Serial.println("LootBox Guard pronto. Aproxime o cartao.");
}

void loop() {
  const unsigned long agora = millis();
  atualizarTampa(agora);
  const char tecla = teclado.getKey();

  if (bloqueado) {
    if (agora - bloqueadaEm < tempoBloqueio) {
      if (tecla == '#') Serial.println("Tentativa ignorada: teclado bloqueado.");
      return;
    }
    bloqueado = false;
    erros = 0;
    Serial.println("Bloqueio encerrado. Aproxime o cartao.");
  }
  if (estado != EstadoTampa::Travada) return;
  lerCartao(agora);
  if (cartaoAceito && agora - cartaoLidoEm >= tempoPin) {
    cartaoAceito = false;
    limparPin();
    Serial.println("Prazo do PIN encerrado. Aproxime o cartao.");
  }
  if (!cartaoAceito) {
    if (tecla == '#') Serial.println("Aproxime um cartao cadastrado antes do PIN.");
    return;
  }
  if (tecla == '*') {
    limparPin();
  } else if (tecla == '#') {
    const bool correto = !excedeu && tamanho == 4 && strcmp(entrada, pinCorreto) == 0;
    limparPin();
    if (correto) {
      cartaoAceito = false;
      erros = 0;
      trava.write(80);
      liberadaEm = agora;
      estado = EstadoTampa::Liberada;
      Serial.println("Acesso autorizado. Abra a tampa em ate 5 segundos.");
    } else {
      Serial.println("PIN incorreto.");
      if (++erros == limiteErros) {
        cartaoAceito = false;
        bloqueadaEm = agora;
        bloqueado = true;
        Serial.println("Teclado bloqueado por 30 segundos.");
      }
    }
  } else if (tecla >= '0' && tecla <= '9') {
    if (tamanho < 4) {
      entrada[tamanho++] = tecla;
      entrada[tamanho] = '\0';
    } else {
      excedeu = true;
    }
  }
}
