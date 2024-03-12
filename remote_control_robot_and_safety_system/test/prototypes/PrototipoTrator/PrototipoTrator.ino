/*******************************************
       FUNCTIONS TO CONTROL ROBOT
********************************************/

#include <WiFi.h> //biblioteca para conexão do ESP 32 à rede

// SAÍDAS PWM => saídas GPIO. Corresponde a #define PONTEH_PWM pin_num
const int servoPin = 17;
const int pwm_forward = 16;
const int pwm_reverse = 18;
const int ledChannel = 0;
const int ledChannel2 = 2;
const int ledChannel3 = 4;

// baud rate

long PARAMS_SERIAL = 115200;

// resolução para PWM
const int resolution = 8; //8bits

int myval;
int angulo;

const char *ssid = "SSD";
const char *password = "PASSWORD";
WiFiServer server(80); //seleciona a porta de número 80 (padrão)

/*******************************************
 Comandos remotos código Arduino (no carrinho)
********************************************/

// Variáveis que controlam as saídas PWM

int mypwm_fr = 60; //pwm forward mais rapido em %
int mypwm_bk = 45; //pwm backward mais rapido em %

int INCLINE = 0;

// Frequencia dos sinais PWM
const int freq = 500;

// Duty Cicle Motor

int dutyciclef = 0;
int dutycicleb = 0;

// setar o máximo PWM em %

//forward em bits
int VAL_MAX_pwm_fr = (mypwm_fr * 255) / 100; 

//backward em bits
int VAL_MAX_pwm_br = (mypwm_bk * 255)/ 100;

// Duty Cicle Servo
int VAL_MED_PWM = 50; // em %
int VAL_MED_pwm = 1+(VAL_MED_PWM * 255)/100; //pwm posicao central
int pwmServo = VAL_MED_pwm;

int PWMswitch1 = 50; // PWM de ajuste –> posicao 1 esq e dir
int PWMswitch2 = 45; // PWM de ajuste -> posicao 2 esq e dir
int PWMswitchBITS = 0;


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
  Serial.println(WiFi.macAddress());
  server.begin();
  
  //configurar PWM para o motor e o servo (corresponde ao AnalogWrite)
  pinMode(pwm_forward, OUTPUT);
  pinMode(pwm_reverse,OUTPUT);
  pinMode(servoPin,OUTPUT);

  //anexar para o pino GPIO
  ledcAttachPin(pwm_forward, ledChannel); 
  ledcSetup(ledChannel, freq, resolution); //PWM motor forward

  ledcAttachPin(pwm_reverse, ledChannel2); 
  ledcSetup(ledChannel2, freq, resolution); //PWM motor reverse
  
  ledcAttachPin(servoPin, ledChannel3);
  ledcSetup(ledChannel3, freq, resolution);//PWM Servo 

}

void loop()
{
  WiFiClient client = server.available();
  if (client)
  {
    String currentLine = "";
    while (client.connected())
    {
      if (client.available())
      {
        char c = client.read();
        Serial.write(c);
        if (c == '\n') //comando de nova linha
        {
          if (currentLine.length() == 0)
          {
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println();
            break;
          }
          else
          {
            currentLine = "";
          }
        }
        else if (c != '\r')
        {
          currentLine += c;
        }

        if (currentLine.endsWith("]")) {

          // ALTERNATIVA: usar a biblioteca Regexp para receber o dado GET
          // usando a expressão regular:
          // GET [\/][[]\D---\D---(?:\d\D|\D)[]]\/

          // INCLINAÇÃO DO SERVO

          if (currentLine.indexOf("j") != -1) {
            INCLINE = 1;
            PWMswitchBITS = map(PWMswitch1, 0, 100, 0, VAL_MED_pwm);
          }
          else
          {
            INCLINE = 0;
          }
          if (currentLine.indexOf("k") != -1) {
            INCLINE = 2;
            PWMswitchBITS = map(PWMswitch2, 0, 100, 0, VAL_MED_pwm);
          }

          if (currentLine.indexOf("d") != -1) {
            pwmServo = VAL_MED_pwm + PWMswitchBITS*INCLINE;
            
          }
          else
          {
            pwmServo = VAL_MED_pwm;
          }
          if (currentLine.indexOf("a") != -1) {
            if (INCLINE == 1){
            PWMswitchBITS = PWMswitchBITS + 20;
            } // corrigir a menor inclinacao na esquerda devido a queda do numero  
            pwmServo = VAL_MED_pwm - PWMswitchBITS*INCLINE;
          }
          myval = map(pwmServo, 0, 245, 165, 230); //165 e 230 foram obtidos manualmente
          angulo = map(myval, 165, 230, 0, 130);
        
          // ACIONA O MOTOR

          Serial.println();

          if (currentLine.indexOf("v") != -1) {

          // DEFINE O SENTIDO - PONTE H

          if (currentLine.indexOf("w") != -1) {
            definirpwmFORW(currentLine);
            frente();
          }
          else if (currentLine.indexOf("s") != -1) {
            definirpwmBACK(currentLine);
            tras();
          }
          else if (currentLine.indexOf("w") == -1 && currentLine.indexOf("s") == -1)
          {
            parado();
          }
          }
          else
          {
            dutyciclef = 0;
            dutycicleb = 0;
            ledcWrite(ledChannel, dutyciclef);
            ledcWrite(ledChannel2,dutycicleb);
            ledcWrite(ledChannel3, myval);
          }

          Serial.println("PWM motor:    ");
          Serial.print(dutycicleb);
          Serial.print("  ");
          Serial.print(dutyciclef);
          Serial.println();
          
          Serial.println("Servo:   ");
          Serial.print(angulo);
          Serial.println();
        }
      }
    }
    client.stop();
    Serial.println("Comando efetuado.");
  }
}

void frente()
{ 
  dutycicleb = 0;
  ledcWrite(ledChannel2, 0);
  delay(200);
  ledcWrite(ledChannel, dutyciclef);
  ledcWrite(ledChannel3, myval);
  Serial.println();
  Serial.println("Ponte H:    ");
  Serial.println("forward");
  Serial.println();
}

void tras()
{
  dutyciclef = 0;
  ledcWrite(ledChannel, 0);
  delay(200);
  ledcWrite(ledChannel2, dutycicleb);
  ledcWrite(ledChannel3, myval);
  Serial.println();
  Serial.println("Ponte H:    ");
  Serial.println("backward");
  Serial.println();
}

void parado()
{ 
  dutyciclef = 0;
  dutycicleb = 0;
  ledcWrite(ledChannel, 0);
  ledcWrite(ledChannel2, 0);
  ledcWrite(ledChannel3, myval);
  Serial.println();
  Serial.println("Ponte H:    ");
  Serial.println("stop");
  Serial.println();
}

void definirpwmBACK(String currentLine)
{
            // DEFINIR O PWM!!
          if (currentLine.indexOf("0") != -1) {
            dutycicleb = 0;
          }
          if (currentLine.indexOf("1") != -1) {
            dutycicleb = map(20, 0, 100, 0, VAL_MAX_pwm_br);
          }
          if (currentLine.indexOf("2") != -1) {
            dutycicleb = map(30, 0, 100, 0, VAL_MAX_pwm_br);
          }
          if (currentLine.indexOf("3") != -1) {
            dutycicleb = map(70, 0, 100, 0, VAL_MAX_pwm_br);
          }
          if (currentLine.indexOf("4") != -1) {
            dutycicleb = VAL_MAX_pwm_br;
          }
}


void definirpwmFORW(String currentLine)
{
            // DEFINIR O PWM!!
          if (currentLine.indexOf("0") != -1) {
            dutyciclef = 0;
          }
          if (currentLine.indexOf("1") != -1) {
            dutyciclef = map(20, 0, 100, 0, VAL_MAX_pwm_fr);
          }
          if (currentLine.indexOf("2") != -1) {
            dutyciclef = map(30, 0, 100, 0, VAL_MAX_pwm_fr);
          }
          if (currentLine.indexOf("3") != -1) {
            dutyciclef = map(70, 0, 100, 0, VAL_MAX_pwm_fr);
          }
          if (currentLine.indexOf("4") != -1) {
            dutyciclef = VAL_MAX_pwm_fr;
          }
}
