// Projeto de Detecção de Luz,Umidade e Temperatura
#include <LiquidCrystal.h>
#include "DHT.h"
#include <RTClib.h> // Biblioteca para Relógio em Tempo Real
#include <EEPROM.h>

#define DHTPIN 2 // Pino do DHT

#define DHTTYPE DHT22 // TIpo do DHT, podendo ser DHT11 ou DHT22
DHT dht(DHTPIN,DHTTYPE); // Definição do DHT

#define UTC_OFFSET -3    // Ajuste de fuso horário para UTC-3
#define READ_LOG 1 // 0 - Não mostra no console o log do EEPROM, 1 - Mostra no console o log do EEPROM
RTC_DS1307 RTC;

// Informando as variáveis para o código
int sensorPin = A0; // Pino do sensor.
int redPin = 13; // Pino do LED vermelho.
int yellowPin = 12; // Pino do LED amarelo.
int greenPin = 11; // Pino do LED verde.
int buzzerPin = 3; // Pino do buzzer.
int lRaw = 0; // Variável para armazenar o valor do sensor

int button = A1; // Pino do botão
int buttonMode = 0; // 0 - Temperatura, 1 - Humidade, 2 - Luz
int buttonLastState = 0;

unsigned long previousMillis = 0; //Variável para estocar o millis anterior
int buzzerDelay = 3000; //Delay do buzzer para desligar

const int yellowAlertLight = 55; //Limite para acionar alerta amarelo (LUZ)
const int redAlertLight = 67; // Limite para acionar o alerta vermelho (LUZ)

const int yellowAlertMinTemp = 12;// Limite minimo para acionar o alerta amarelo (TEMPERATURA)
const int yellowAlertMaxTemp = 16;// Limite máximo para acionar o alerta amarelo (TEMPERATURA)
const int redAlertMinTemp = 5;// Limite minimo para acionar o alerta vermelho (TEMPERATURA)
const int redAlertMaxTemp = 24; // Limite máximo para acionar o alerta vermelho (TEMPERATURA)

const int yellowAlertMinUmi = 60;// Limite minimo para acionar o alerta amarelo (UMIDADE)
const int yellowAlertMaxUmi = 70;// Limite máximo para acionar o alerta vermelho (UMIDADE)
const int redAlertMinUmi = 40;// Limite minimo para acionar o alerta vermelho (UMIDADE)
const int redAlertMaxUmi= 90;// Limite máximo para acionar o alerta vermelho (UMIDADE)


const int maxRecords = 100; // Quantidade máxima 
const int recordSize = 10; // Tamanho de cada registro em bytes (4 para RTC, 2 para temperatura, 2 para humidade e 2 para luz )
int startAddress = 0; 
int endAddress = maxRecords * recordSize;
int currentAddress = 0;

// Icones para cada modo do LCD

byte termo_ul[8] = { B00000, B00000, B00011, B00100, B00100, B00100, B00100, B00100 };
byte termo_ur[8] = { B00000, B00000, B10000, B01000, B01100, B01000, B01100, B01000 };
byte termo_dl[8] = { B00100, B00100, B00100, B01011, B01011, B01001, B00100, B00011 };
byte termo_dr[8] = { B01100, B01000, B01100, B10100, B10100, B10100, B01000, B10000 };

byte light_ul[8] = { B00000, B00111, B01100, B01000, B10000, B10000, B10000, B10000 };
byte light_ur[8] = { B00000, B11100, B00110, B00010, B00001, B00001, B00001, B00001 };
byte light_dl[8] = { B01001, B01010, B01010, B00100, B00111, B00100, B00100, B00011 };
byte light_dr[8] = { B10010, B01010, B01010, B00100, B11100, B00100, B00100, B11000 };

byte humi_ul[8] = { B00000, B00001, B00010, B00010, B00100, B00100, B01000, B01000 };
byte humi_ur[8] = { B00000, B10000, B01000, B01000, B00100, B00100, B00010, B00010 };
byte humi_dl[8] = { B10100, B10100, B10110, B10010, B01011, B01001, B00100, B00011 };
byte humi_dr[8] = { B00001, B00001, B00001, B00001, B00010, B00010, B00100, B11000 };
 

LiquidCrystal lcd(10,9,8,7,6,5,4);

// Design dos sistemas de temperatura, umidade e luz
void lcdTerm(){
  lcd.clear();
  lcd.createChar(0, termo_ul);
  lcd.createChar(1, termo_ur);
  lcd.createChar(2, termo_dl);
  lcd.createChar(3, termo_dr);
  lcd.setCursor(0,0);
  lcd.write(byte(0));
  lcd.setCursor(1,0);
  lcd.write(byte(1));
  lcd.setCursor(0,1);
  lcd.write(byte(2));
  lcd.setCursor(1,1);
  lcd.write(byte(3));
  lcd.setCursor(3,0);
  lcd.write("Temperatura");
  lcd.setCursor(8,1);
  lcd.write("C");
}

void lcdLight(){
  lcd.clear();
  lcd.createChar(0, light_ul);
  lcd.createChar(1, light_ur);
  lcd.createChar(2, light_dl);
  lcd.createChar(3, light_dr);
  lcd.setCursor(0,0);
  lcd.write(byte(0));
  lcd.setCursor(1,0);
  lcd.write(byte(1));
  lcd.setCursor(0,1);
  lcd.write(byte(2));
  lcd.setCursor(1,1);
  lcd.write(byte(3));
  lcd.setCursor(3,0);
  lcd.write("Luminosidade");
  lcd.setCursor(8,1);
  lcd.write("%");
}

void lcdHumi(){
  lcd.clear();
  lcd.createChar(0, humi_ul);
  lcd.createChar(1, humi_ur);
  lcd.createChar(2, humi_dl);
  lcd.createChar(3, humi_dr);
  lcd.setCursor(0,0);
  lcd.write(byte(0));
  lcd.setCursor(1,0);
  lcd.write(byte(1));
  lcd.setCursor(0,1);
  lcd.write(byte(2));
  lcd.setCursor(1,1);
  lcd.write(byte(3));
  lcd.setCursor(3,0);
  lcd.write("Umidade");
  lcd.setCursor(8,1);
  lcd.write("%");
}

// Setar a tipagem dos pinos
void setup(){
  pinMode(redPin,OUTPUT);
  pinMode(yellowPin,OUTPUT);
  pinMode(greenPin,OUTPUT);
  pinMode(buzzerPin,OUTPUT);
  pinMode(button, INPUT);
  Serial.begin(9600);
  RTC.begin();    // Inicialização do Relógio em Tempo Real
  RTC.adjust(DateTime(F(__DATE__), F(__TIME__)));
  lcd.begin(16,2);
  lcd.print("EPDM Corp.");
  delay(5000);
  lcdTerm();
  dht.begin();
}

// Função para pegar o tempo

DateTime getTime(){
  DateTime now = RTC.now();

  //Formantando o tempo
  int offsetsecond = UTC_OFFSET * 3600;
  unsigned long adjustedUnixTime = now.unixtime() + offsetsecond;

  return DateTime(adjustedUnixTime);
}

// Função para pegar o próximo adress do EEPROM
void getNextAddress() {
  currentAddress += recordSize;
  if (currentAddress >= endAddress) {
      currentAddress = 0; // Volta para o começo se atingir o limite
  }
}

// Função para salvar os dados no EEPROM
void recordData(float lightValue, float tempValue, float humiValue){
  // Convertendo os valores em float para INT
  DateTime adjustedTime = getTime();
  Serial.println("Salvando para o EEPROM as variáveis");
  int tempInt = (int)(tempValue * 100);
  int humiInt = (int)(humiValue * 100);
  int lightInt = (int)(lightValue * 100);
  EEPROM.put(currentAddress, adjustedTime.unixtime());
  EEPROM.put(currentAddress + 4, tempInt);
  EEPROM.put(currentAddress + 6, humiInt);
  EEPROM.put(currentAddress + 8, lightInt);
  Serial.println("Variáveis salvas");
  getNextAddress();
}

void readData(){
  Serial.println("Data salva no EEPROM: ");
  Serial.println("Dia e Hora\t\tTemperatura\t\tUmidade\t\tLuz");

  for (int address = startAddress; address < endAddress; address += recordSize) {
      long timeStamp;
      int tempInt, humiInt, lightInt;

      // Ler dados da EEPROM
      EEPROM.get(address, timeStamp);
      EEPROM.get(address + 4, tempInt);
      EEPROM.get(address + 6, humiInt);
      EEPROM.get(address + 8, lightInt);

      // Converter valores
      float temperature = tempInt / 100.0;
      float humidity = humiInt / 100.0;
      float light = lightInt / 100.0;

      // Verificar se os dados são válidos antes de imprimir
      if (timeStamp != 0xFFFFFFFF) { // 0xFFFFFFFF é o valor padrão de uma EEPROM não inicializada
          DateTime dt = DateTime(timeStamp);
          Serial.print(dt.timestamp(DateTime::TIMESTAMP_FULL));
          Serial.print("\t\t");
          Serial.print(temperature);
          Serial.print(" C\t\t");
          Serial.print(humidity);
          Serial.print(" %\t\t");
          Serial.print(light);
          Serial.println(" %");
      }
  }

}
// Função para ver os LEDS ligados dependentes do valor e acionar o Buzzer caso fique no alerta amarelo ou vermelho
void ledBuzzerCheck(float lightValue, float tempValue, float humiValue){
  if (lightValue >= redAlertLight || tempValue >= redAlertMaxTemp || tempValue <= redAlertMinTemp || humiValue >= redAlertMaxUmi || humiValue <= redAlertMinUmi){ //Condição para acionar o alerta vermelho
  digitalWrite(redPin,HIGH);
  digitalWrite(yellowPin,LOW);
  digitalWrite(greenPin,LOW);
  digitalWrite(buzzerPin,HIGH);
  buzzerDelay = 1000;
  recordData(lightValue, tempValue, humiValue);

  } else if (lightValue >= yellowAlertLight || tempValue >= yellowAlertMaxTemp || tempValue <= yellowAlertMinTemp || humiValue >= yellowAlertMaxUmi || humiValue <= yellowAlertMinUmi){ //Condição para acionar o alerta amarelo
  digitalWrite(redPin,LOW);
  digitalWrite(yellowPin,HIGH);
  digitalWrite(greenPin,LOW);
  digitalWrite(buzzerPin,LOW);
  buzzerDelay = 3000;
  recordData(lightValue, tempValue, humiValue);

  } else{	// Caso nenhuma das condições acima é acionada, fica no verde
  digitalWrite(redPin,LOW);
  digitalWrite(yellowPin,LOW);
  digitalWrite(greenPin,HIGH);
  digitalWrite(buzzerPin, LOW);
  buzzerDelay = 3000;
  }
  
  if ((millis() - previousMillis) > buzzerDelay){
    digitalWrite(buzzerPin, LOW);
    previousMillis = millis();
  }
}



// Função principal
void loop(){
  lRaw = analogRead(sensorPin); // Lê o valor do sensor
  int l = map(lRaw, 1005, 17, 0, 100); // Converte o valor para o intervalo de PWM
  float h = dht.readHumidity(); // Lê valor de humidade
  float t = dht.readTemperature(); // Lê valor de temperatura
  int buttonState = analogRead(button);
  buttonState = map(buttonState,0,1023,0,1);

  if (buttonState == 1 && buttonLastState == 0){ // Checagem do botão e debounce
    if (buttonMode >= 3){
      buttonMode = 0;
    } else{
      buttonMode++;
    }
    switch(buttonMode){
      case 0: // Modo temperatura
        lcdTerm();
        break;
      case 1: // Modo umidade
        lcdHumi();
        break;
      case 2: // Modo Luz
        lcdLight();
        break;
    }
    delay(200);
  }

  lcd.setCursor(3,1);
  if (buttonMode == 0){ // Modo temperatura
  lcd.print(t);
  } else if (buttonMode == 1){ // Modo umidade
  lcd.print(h);
  } else{ // Modo Luz
  lcd.print(l);
  if (l < 100){
    lcd.setCursor(5,1);
    lcd.print(" "); // Deletar o digito 0 caso chegue no 100 e volte
  }
  }

  ledBuzzerCheck(l, t,h);

  // Descomentar essa parte para DEBUG
  // Serial.print("Intensidade de Luz: ");
  // Serial.println(l); // Imprime o valor do sensor no monitor serial DELETAR
   // Serial.println(lRaw);
  // Serial.print("Umidade");
  // Serial.print(h);
  // Serial.print("\n");
  // Serial.print("Temperatura");
  // Serial.print(t);
  // Serial.print("\n");

  if(READ_LOG) readData(); // Leiturar do EEPROM caso o modo esteja ativado.
  buttonLastState = buttonState;
  delay(500);

}
