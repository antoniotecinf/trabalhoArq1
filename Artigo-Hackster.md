# LootBox Guard: RFID e PIN para um baú de recompensas

Em um jogo presencial, uma equipe encontra a recompensa no final de uma missão. O LootBox Guard simula um baú que libera essa recompensa somente quando o jogador apresenta o cartão cadastrado e informa o PIN correto.

## Como o circuito funciona

O ESP32 virtual recebe o UID do cartão pelo leitor MFRC522 e lê o PIN no teclado matricial. Quando as duas verificações passam, o servo libera a trava e o jogador tem cinco segundos para iniciar a abertura. O interruptor deslizante representa a tampa. Ao voltar para a posição fechada, o servo trava o baú novamente.

Após três PINs incorretos consecutivos, o firmware ignora as tentativas por 30 segundos. O ESP32 continua lendo a tampa nesse período. Se alguém abrir o baú sem autorização, o buzzer toca e o monitor serial registra a violação. Fechar a tampa encerra o alarme.

## Como reproduzir

1. Abra a [simulação do LootBox Guard](https://wokwi.com/projects/481675741334122497) e clique em **Start Simulation**.
2. Clique no MFRC522, selecione **Blue Card** e clique em **TAP**.
3. Digite **2580#**. Deslize o interruptor para a direita para abrir a tampa e depois para a esquerda para fechar.
4. Repita com **Green Card** e confirme a recusa. Para testar o bloqueio, use **Blue Card** e digite **1111#** três vezes.
5. Durante o bloqueio, tente **2580#** e abra a tampa pelo interruptor. O monitor serial registra a tentativa ignorada e o alarme.

## Resultados

Verificamos no Wokwi a recusa de PIN sem cartão, de cartão desconhecido e de PIN errado. Também verificamos a abertura autorizada, o travamento no fechamento e o alarme com o teclado bloqueado. O monitor serial registrou o fim do bloqueio.

## Arquivos

O firmware em C++ está em sketch.ino e as conexões em diagram.json. O README contém a tabela de pinos e o roteiro dos testes. [Acesse o código e o diagrama no GitHub](https://github.com/antoniotecinf/trabalhoArq1).