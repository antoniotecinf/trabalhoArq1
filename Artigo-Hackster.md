# LootBox Guard: RFID e PIN para um baú de recompensas

O LootBox Guard é um baú virtual para jogos presenciais. A ideia é guardar a recompensa da missão e liberar a tampa só para quem tem o cartão cadastrado e sabe o PIN.

## Como o circuito funciona

O ESP32 virtual recebe o UID do cartão pelo leitor MFRC522 e lê o PIN no teclado matricial. Com o cartão e o PIN corretos, o servo libera a trava e o jogador tem cinco segundos para iniciar a abertura. O interruptor deslizante representa a tampa. Ao voltar para a posição fechada, o servo trava o baú novamente.

Após três PINs incorretos consecutivos, o firmware ignora as tentativas por 30 segundos. O ESP32 continua lendo a tampa nesse período. Se alguém abrir o baú sem autorização, o buzzer toca e o monitor serial registra a violação. Fechar a tampa encerra o alarme.

## Como reproduzir

1. Abra a [simulação do LootBox Guard](https://wokwi.com/projects/481675741334122497) e clique em **Start Simulation**.
2. Clique no MFRC522, selecione **Blue Card** e clique em **TAP**.
3. Digite **2580#**. Deslize o interruptor para a direita para abrir a tampa e depois para a esquerda para fechar.
4. Repita com **Green Card** e confirme a recusa. Para testar o bloqueio, use **Blue Card** e digite **1111#** três vezes.
5. Durante o bloqueio, tente **2580#** e abra a tampa pelo interruptor. O monitor serial registra a tentativa ignorada e o alarme.

## Resultados

Nos testes no Wokwi, o baú recusou PIN sem cartão, cartão desconhecido e PIN errado. Com o cartão e o PIN corretos, a trava abriu e voltou à posição inicial quando fechamos a tampa. O alarme funcionou mesmo com o teclado bloqueado, e o monitor serial registrou o fim da espera.

## Arquivos

O firmware em C++ está em sketch.ino e as conexões em diagram.json. O README contém a tabela de pinos e o roteiro dos testes. [Acesse o código e o diagrama no GitHub](https://github.com/antoniotecinf/trabalhoArq1).