# WiFi-Controlled-Gate

Projeto criado com o objetivo de controlar remotamente um portão através de uma conexão wifi local.
Os materiais utilizados foram:  
1x NodeMCU ESP8266 ESP-12E  
1x Transistor NPN  
1x Resistor de 1kΩ  
1x Controle do portão eletrônico  
1x Módulo XL6009 Regulador de Tensão Step Up Ajustável DC-DC  
1x Fonte de 5VCC  
  
Montagem e funcionamento da parte física:  
O circuito é simples, o esp8266 é responsável por enviar um sinal de saída que controla o transistor NPN, o transistor toma lugar de um dos botões físicos do controle eletrônico, fazendo a função de chaveamento. Na alimentação é utilizada uma fonte de 5 volts, os 5 volts são ligados no VIN do esp8266 e o step up é responsável por subir essa tensão para 12v e então alimentar o controle eletrônico.

Aplicação:  
Para a interface, foi utilizada uma página web simples, quando um dispositivo abre no navegador o endereço IP local do uC, é carregada a página e é realizada a conexão entre Cliente (Smartphone) e Servidor (esp8266), os dados trafegam via websocket no formato JSON, a aplicação possui autorecuperação, constantemente é checado se há conexão entre cliente e servidor, caso haja queda e retorno do serviço, a aplicação restabelece a comunicação. Cada 1000ms o servidor envia um "sinal" para o cliente informando o tempo desde que foi ligado (máx ~49 dias) e o status do controle, da parte do cliente, a única informação passada é se o botão foi clicado, ao clique do botão, o servidor recebe essa informação e realiza o acionamento do controle, por segurança, a aplicação só pode ativar o botão, quem controla o tempo de quanto tempo o controle eletrônico fica ligado é o servidor.
