/**********************************************
  Remote controlled robot with ESP32 (servo and H bridge)
***********************************************/

/* OBS.:
 I've built an app with MIT APP inventor to control the robot with a smart phone or any computer with the app.
 USED THIS CODE TO CONTROL ROBOT IN AN EXTERNAL NETWORK USING NGROK
 So that way the robot can be controlled remotely from any place in the world.
*/

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

// Network credentials
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
int VAL_MAX_pwm_br = (mypwm_bk * 255) / 100;

// Duty Cicle Servo
int VAL_MED_PWM = 50; // em %
int VAL_MED_pwm = 1 + (VAL_MED_PWM * 255) / 100; //pwm posicao central
int pwmServo = VAL_MED_pwm;

int PWMswitch1 = 50; // PWM de ajuste –> posicao 1 esq e dir
int PWMswitch2 = 45; // PWM de ajuste -> posicao 2 esq e dir
int PWMswitchBITS = 0;

int cont_stop = 0; //contador para parar

/*****************************************
                WEB PAGE
 *****************************************/

const char webpage[] PROGMEM = R"=====(

<html>
  <head>
    <title>PROTOTIPO TRATOR</title>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <style>
      body { font-family: Arial; text-align: center; margin:0px auto; padding-top: 30px;}
      table { margin-left: auto; margin-right: auto; }
      td { padding: 8 px; }
      
      .button {
        background-color:  #14852C ;
        border: none;
        color: white;
        padding: 10px 20px;
        text-align: center;
        text-decoration: none;
        display: inline-block;
        font-size: 30px;
        margin: 6px 3px;
        cursor: pointer;
        -webkit-touch-callout: none;
        -webkit-user-select: none;
        -khtml-user-select: none;
        -moz-user-select: none;
        -ms-user-select: none;
        user-select: none;
        -webkit-tap-highlight-color: rgba(0,0,0,0);
      }

     button:focus{
      background-color:  #37B8CD ;
      border: none;
      color: white;
      padding: 10px 20px;
      text-align: center;
      text-decoration: none;
      display: inline-block;
      font-size: 30px;
      margin: 6px 3px;
      cursor: pointer;
      -webkit-touch-callout: none;
      -webkit-user-select: none;
      -khtml-user-select: none;
      -moz-user-select: none;
      -ms-user-select: none;
      user-select: none;
      -webkit-tap-highlight-color: rgba(0,0,0,0);
      }

    </style>

<link rel="shortcut icon" href="#">
    
  </head>
  <body>
    <h1>PROTOTIPO TRATOR</h1>
    <p> U&M </p>
    <table>
      <tr><td colspan="3" align="center">
      <a href="/UUP">
      <button class="button">&#8679</button>
      </a>
      </td>      
      </tr>
      
      <tr> 
      <td align="center"><a href="/LEFTUP"> <button class="button">&#8662</button></a></td>
      <td align="center"><a href="/UP"> <button class="button">&#8593</button></a></td> 
      <td align="center"><a href="/RIGHTUP"> <button class="button">&#8663</button></a></td>  
      </tr>
      
      <tr><td colspan="3" align="center">
      <a href="/O"> <button class="button">O</button> </a> </td>      
      </tr>
      
      <tr>
      <td align="center"><a href="/LEFTDOWN"> <button class="button">&#8665</button></a></td>       
      <td align="center"><a href="/DOWN"> <button class="button">&#x2193</button></a></td> 
      <td align="center"><a href="/RIGHTDOWN"> <button class="button">&#8664</button></a></td></tr>               
    
      <tr><td colspan="3" align="center">
      <a href="/DDOWN"> <button class="button">&#8681</button> </a>
      </td></tr>      
    
    </table>

  </body>
</html>

)=====";


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
  Serial.println("Endereço MAC: ");
  Serial.println(WiFi.macAddress());
  Serial.println();
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
            client.print(webpage);
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

          if (currentLine.endsWith("GET /UUP")) {
            Reto();
            definirpwmFORW("3");
            frente();
          }
          
          if (currentLine.endsWith("GET /UP")) {
            Reto();
            definirpwmFORW("2");
            frente();
          }

          if (currentLine.endsWith("GET /LEFTUP")) {
            Esquerda();
            definirpwmFORW("2");
            frente();          
          }
          
         if (currentLine.endsWith("GET /RIGHTUP")) {
            Direita();
            definirpwmFORW("2");
            frente();           
          }
          
         if (currentLine.endsWith("GET /DDOWN")) {
            Reto();
            definirpwmBACK("3");
            tras();        
          }

         if (currentLine.endsWith("GET /DOWN")) {
            Reto();
            definirpwmBACK("2");
            tras();          
          }
          
         if (currentLine.endsWith("GET /LEFTDOWN")) {
            Esquerda();
            definirpwmBACK("2");
            tras();          
          }

         if (currentLine.endsWith("GET /RIGHTDOWN")) {
            Direita();
            definirpwmBACK("2");
            tras();             
          }
         
         if (currentLine.endsWith("GET /O")) {
            Reto();
            parado();           
          }
        }
      }
    client.stop();
    Serial.println("Comando efetuado.");
    Serial.println();
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
  PrintSerial();
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
  PrintSerial();
}

void parado()
{ 
  if (dutycicleb == 0 && dutyciclef != 0){
  for (cont_stop = dutyciclef; cont_stop > 0; cont_stop = cont_stop-10){
  ledcWrite(ledChannel, cont_stop);
  }
  }

  if (dutyciclef == 0 && dutycicleb !=0){
  for (cont_stop = dutycicleb; cont_stop > 0 ; cont_stop = cont_stop-10){
  ledcWrite(ledChannel2, cont_stop);
  }
  }
  dutyciclef = 0;
  dutycicleb = 0;
  ledcWrite(ledChannel, 0);
  ledcWrite(ledChannel2, 0);
  ledcWrite(ledChannel3, myval);
  Serial.println();
  Serial.println("Ponte H:    ");
  Serial.println("stop");
  Serial.println();
  PrintSerial();
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
            dutycicleb = map(40, 0, 100, 0, VAL_MAX_pwm_br);
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
            dutyciclef = map(40, 0, 100, 0, VAL_MAX_pwm_fr);
          }
          if (currentLine.indexOf("3") != -1) {
            dutyciclef = map(70, 0, 100, 0, VAL_MAX_pwm_fr);
          }
          if (currentLine.indexOf("4") != -1) {
            dutyciclef = VAL_MAX_pwm_fr;
          }
}

void PrintSerial() 
{
            Serial.println("PWM motor:    ");
            Serial.print(dutycicleb);
            Serial.print(" ");
            Serial.print(dutyciclef);
            Serial.println();
            Serial.println("Servo:   ");
            Serial.print(angulo);
            Serial.println();    
}

void Direita()
{
            INCLINE = 2;
            PWMswitchBITS = map(PWMswitch2, 0, 100, 0, VAL_MED_pwm);
            pwmServo = VAL_MED_pwm + PWMswitchBITS*INCLINE;
            ValServo();
}

void Esquerda()
{
            INCLINE = 2;
            PWMswitchBITS = map(PWMswitch2, 0, 100, 0, VAL_MED_pwm);
            pwmServo = VAL_MED_pwm - PWMswitchBITS*INCLINE;
            ValServo();
}

void Reto()
{
            INCLINE = 0;
            PWMswitchBITS = map(PWMswitch1, 0, 100, 0, VAL_MED_pwm);
            pwmServo = VAL_MED_pwm;
            ValServo();
}

void ValServo(){
            myval = map(pwmServo, 0, 245, 165, 230); //165 e 230 foram obtidos manualmente
            angulo = map(myval, 165, 230, 0, 130);
}
