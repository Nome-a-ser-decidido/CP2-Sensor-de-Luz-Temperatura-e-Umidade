# Sensor-de-Luz-Temperatura-e-Umidade

Este projeto lê os níveis de umidade, temperatura e luminosidade, exibindo os valores em um display. Também registra automaticamente no EEPROM dados que ultrapassam os limites definidos pelo usuário, com data e hora registradas pelo RTC.

## Sobre o projeto
Este projeto teve como principal objetivo fazer um sensor para uma vinharia utilizando um sistema arduino. O circuito:
- Lê a umidade, temperatura e luminosidade do ambiente local.
- Mostra os valores em um display.
- Caso um dos valores ultrapasse o limite colocado pelo usuário, aciona um alerta através de um buzzer junto com LEDs amarelo e vermelho para indicar os níveis.
- Grava os dados que ultrapassaram os limites junto com o horário no EEPROM do arduíno.
- Através da configuração do código, é possivel ler e mostrar no console os dados marcados no EEPROM.

## Componentes do projeto
 - 1 x Arduino UNO R3
 - 3 x LEDs
 - 1 x Buzzer / Piezo
 - 1 x LDR / Fotoresistor
 - 1 x LCD
 - 1 x DHT22
 - 1 x RTC DS1307
 - 1 x Pushbutton
 - 8 x Resistores

## Esquema do circuito
![image](https://github.com/user-attachments/assets/80cf8c42-ff1d-466a-a009-7cc7408c4b03)

## Pré-requisitos
- IDE do arduino
- Biblioteca DHT (Sensor de umidade e temperatura)
- Biblioteca Liquid Crystal (Display)
- Biblioteca RTC (Relógio digital)

## Instalação
1. Conecte o arduíno ao computador
2. Coloque o código .ino ao IDE do arduíno
3. Carregue o código

## Como usar
- Utilize o botão para trocar as interfaces
- Para ler os dados salvos no EEPROM basta mudar configuração "READ_LOG" na linha 13:
  ![image](https://github.com/user-attachments/assets/d37dc89c-a4e5-47cb-88fb-80aac4271d86)

## Video

[![Vídeo](![Checkpoint 2 Arduino-00132](https://github.com/user-attachments/assets/9fb10122-ad87-4163-a68b-7bef3a54a77f))](https://www.youtube.com/watch?v=D6XuQYamK4I)




