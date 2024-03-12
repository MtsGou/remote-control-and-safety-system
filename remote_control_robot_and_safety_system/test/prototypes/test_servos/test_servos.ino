/*******************************************
  Carrinho comando com ESP32 (servos e ponte H)
********************************************/

#include <WiFi.h> //biblioteca para conexão do ESP 32 à rede

// SAÍDAS PWM => saídas GPIO. Corresponde a #define PONTEH_PWM pin_num


const int servoPin = 13;
const int ledChannel = 0;
const int PONTEH_PWM = 2;

// SAÍDAS DIGITAIS (HIGH,LOW) PONTE H
const int CHAVE_A = 27;
const int CHAVE_B = 26;


// baud rate
long PARAMS_SERIAL = 115200;

// resolução para PWM
const int resolution = 8; //8bits

int myval = 127;

const char *ssid = "SSID";
const char *password = "password";
WiFiServer server(80); //seleciona a porta de número 80 (padrão)

/*******************************************
   Comandos remotos código Arduino (no carrinho)
********************************************/

// Variáveis que controlam as saídas digitais

int chA = HIGH;
int chB = HIGH;

int INCLINE = 0;

// Frequencia dos sinais PWM
const byte freq = 500;

// Duty Cicle Motor
int VAL_MAX_PWM = 80; // setar o máximo PWM em %
int VAL_MAX_pwm = (VAL_MAX_PWM * 255) / 100;
int dutycicle = 0;

// Duty Cicle Servo
int VAL_MED_PWM = 50; // em %
int VAL_MED_pwm = (VAL_MED_PWM * 255) / 100; //pwm posicao central
int pwmServo = VAL_MED_pwm;

int PWMswitch = 30; // 30% PWM de ajuste –> 2 posicoes esq e dir
int PWMswitchBITS = map(PWMswitch, 0, 100, 0, VAL_MED_pwm);


/******************************************
             Código Principal
******************************************/

void setup()
{
  //conexão à internet.
  Serial.begin(PARAMS_SERIAL);
  Serial.print("Conectando-se a ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi conectada.");
  Serial.println("Endereço de IP: ");
  Serial.println(WiFi.localIP());
  server.begin();

  //attach servo
  ledcAttachPin(servoPin, ledChannel);
  ledcSetup(ledChannel, freq, resolution);
  pinMode(CHAVE_A, OUTPUT);
  pinMode(CHAVE_B, OUTPUT);

}

void loop()
{
  ledcWrite(ledChannel,210);
  delay(2000);
  ledcWrite(ledChannel,100);
  delay(2000);
  digitalWrite(CHAVE_A,HIGH);
  digitalWrite(CHAVE_B,HIGH);
  delay(500);
  digitalWrite(CHAVE_A,LOW);
  digitalWrite(CHAVE_B,LOW);
}
