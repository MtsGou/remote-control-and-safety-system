/****************************************************************************
       PROTOTYPE REMOTE CONTROLLER LOGIC [THIS IS ONLY A SKETCH]
*****************************************************************************/

#include <WiFi.h>
#include <WebServer.h>
#include <WebSocketsServer.h>
#include <Ticker.h>

//Network Parameters

char* ssid = "YOUR_SSD";
char* password = "YOUR_PASSWORD";
bool read_data = false;

//Baud Rate
long PARAMS_SERIAL = 115200; 

// Server port ws 81
WebServer server;
WebSocketsServer webSocket = WebSocketsServer(81); 

// Server port ws 80
WiFiServer server(80);

// resolução para PWM
const int resolution = 8; //8bits

int myval;
int angulo;

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

// SAÍDAS PWM => saídas GPIO. Corresponde a #define PONTEH_PWM pin_num
const int servoPin = 17;
const int pwm_forward = 16;
const int pwm_reverse = 18;
const int ledChannel = 0;
const int ledChannel2 = 2;
const int ledChannel3 = 4;

int cont_stop = 0; //contador para parar

// Digital inputs

const int RMMODE = 14; //MODO REMOTO/MANUAL
const int swrip = 12; // SWITCH RIPPER
const int lgat = 13; //GATILHO LAMINA
const int lpoe = 23; //POLEGAR ESQ LAMINA
const int lpod = 22; //POLEGAR DIR LAMINA
const int lbote = 21; //BOTAO ESQ LAMINA
const int lbotd = 19; //BOTAO DIR LAMINA
const int trava = 18; //TRAVA PARKING BRAKE
const int pbmode = 5; //PARKING BRAKE MODE
const int ups = 4; // SHIFTER CONTROL UPSHIFT

// ADC:

/*const int keystart = GPIO12;*/ // key start // X

// PWM:

const int decel = 34; //DESACELERADOR
const int brake = 35; //FREIO
const int ripio = 32; //RIPPER IN/OUT
const int ripud = 33; //RIPPER UP/DOWN
const int llfrg = 25; //LAMINA LEFT/RIGHT
const int lfwbk = 26; //LAMINA FORW/BACK
const int snsr = 27; //SENSOR DIRECAO PWM
/*const int brsnrl = GPIO20; //BRAKE SENSOR LEFT // X
const int brsnrr = GPIO21; //BRAKE SENSOR RIGHT*/ //X

Ticker timer;

// Página do Cliente Web Browser

char webpage[] PROGMEM = R"=====(
<!DOCTYPE html>
  <html>
    <head>
      <title> Transmissao Trator </title>
    </head>
    
<style>

div.firstcontent{
background-color:#ffffff;
height: 150px;
width: 240px;
border: 5px solid #000000;
float: left; 
}

div.secondcontent{
background-color:#ffffff;
height: 700px;
width: 340px;
border: 5px solid #000000;
float: left; 
}

div.thirdcontent{
background-color:#ffffff;
height: 400px;
width: 380px;
border: 5px solid #000000;
float: left; 
}

div.fourthcontent{
background-color:#ffffff;
height: 860px;
width: 740px;
border: 5px solid #000000;
float: down; 
}

#dadosmodorm {
  font-family: Arial, Helvetica, sans-serif;
  border-collapse: collapse;
  width: 80%;
}

#dadosmodorm td, #dadosmodorm th {
  border: 1px solid #ddd;
  padding: 12px;
}

#dadosmodorm tr:nth-child(even){background-color: #f2f2f2;}

#dadosmodorm tr:hover {background-color: #ddd;}

#dadosmodorm td{
text-align: center;
font-weight: bold;
} 

#dadosmodorm th {
  padding-top: 12px;
  padding-bottom: 12px;
  text-align: left;
  background-color: #04AA6D;
  color: white;
}


#dadoskeystart {
  font-family: Arial, Helvetica, sans-serif;
  border-collapse: collapse;
  width: 80%;
}

#dadoskeystart td, #dadoskeystart th {
  border: 1px solid #ddd;
  padding: 12px;
}

#dadoskeystart tr:nth-child(even){background-color: #f2f2f2;}

#dadoskeystart tr:hover {background-color: #ddd;}

#dadoskeystart td{
text-align: center;
font-weight: bold;
} 

#dadoskeystart th {
  padding-top: 12px;
  padding-bottom: 12px;
  text-align: left;
  background-color: #04AA6D;
  color: white;
}

#dadosdecelbrake {
  font-family: Arial, Helvetica, sans-serif;
  border-collapse: collapse;
  width: 30%;
}

#dadosdecelbrake td, #dadosdecelbrake th {
  border: 1px solid #ddd;
  padding: 12px;
}

#dadosdecelbrake tr:nth-child(even){background-color: #f2f2f2;}

#dadosdecelbrake tr:hover {background-color: #ddd;}

#dadosdecelbrake td{
text-align: center;
font-weight: bold;
} 

#dadosdecelbrake th {
  padding-top: 12px;
  padding-bottom: 12px;
  text-align: left;
  background-color: #04AA6D;
  color: white;
}

#dadosripper {
  font-family: Arial, Helvetica, sans-serif;
  border-collapse: collapse;
  width: 80%;
}

#dadosripper td, #dadosripper th {
  border: 1px solid #ddd;
  padding: 12px;
}

#dadosripper tr:nth-child(even){background-color: #f2f2f2;}

#dadosripper tr:hover {background-color: #ddd;}

#dadosripper td{
text-align: center;
font-weight: bold;
} 

#dadosripper th {
  padding-top: 12px;
  padding-bottom: 12px;
  text-align: left;
  background-color: #04AA6D;
  color: white;
}

#dadoslamina {
  font-family: Arial, Helvetica, sans-serif;
  border-collapse: collapse;
  width: 90%;
}

#dadoslamina td, #dadoslamina th {
  border: 1px solid #ddd;
  padding: 12px;
}

#dadoslamina tr:nth-child(even){background-color: #f2f2f2;}

#dadoslamina tr:hover {background-color: #ddd;}

#dadoslamina td{
text-align: center;
font-weight: bold;
} 

#dadoslamina th {
  padding-top: 12px;
  padding-bottom: 12px;
  text-align: left;
  background-color: #04AA6D;
  color: white;
}

#dadossensordirecao {
  font-family: Arial, Helvetica, sans-serif;
  border-collapse: collapse;
  width: 80%;
}

#dadossensordirecao td, #dadossensordirecao th {
  border: 1px solid #ddd;
  padding: 12px;
}

#dadossensordirecao tr:nth-child(even){background-color: #f2f2f2;}

#dadossensordirecao tr:hover {background-color: #ddd;}

#dadossensordirecao td{
text-align: center;
font-weight: bold;
} 

#dadossensordirecao th {
  padding-top: 12px;
  padding-bottom: 12px;
  text-align: left;
  background-color: #04AA6D;
  color: white;
}

</style>
    
 <body style = "background-color: coral;" onload = "javascript:init()">
       <h1> Transmissao do Trator </h1>
  <hr/>
  <br/>  
  <div class = "firstcontent">
  <table id="dadosmodorm">
      <thead>
        <caption><h3>MODO REMOTO</h3>
        <tr>
          <th>ESTADO</th>
          <td id = "valorRMMODE"> </td>
        </tr>
      </thead>
  </table> 
  </div>

  <div class = "firstcontent">
  <table id="dadoskeystart">
      <thead>
        <caption><h3>Chave de Partida</h3>
        <tr>
          <th>ESTADO</th>
          <td id = "valorkeystart"> </td>
        </tr>
      </thead>
  </table> 
  </div>
  
  <div class = "firstcontent">
  <table id="dadosdecelbrake">
      <thead>
        <caption><h3>Pedais</h3>
        <tr>
          <th>Desacelerador</th>
          <td id = "valordecel"> </td>
        </tr>
        <tr>
          <th>Freio</th>
          <td id = "valorbrake"> </td>
        </tr>
      </thead>
  </table>
  </div>

<div class = "fourthcontent">

<div class = "secondcontent">
<table id="dadosripper">
      <thead>
        <caption><h3>Ripper</h3>
        <tr>
          <th>Switch</th>
          <td id = "valorswrip"> </td>
        </tr>
        <tr>
          <th>PWM In/Out</th>
          <td id = "valorripio"> </td>
        </tr>
        <tr>
          <th>PWM Up/Down</th> 
          <td id = "valorripud"></td>
        </tr>
      </thead>
  </table>  
  
  <table id="dadossensordirecao">
      <thead>
        <caption><h3>Sensor de Direcao</h3>
        <tr>
          <th>PWM FWD/MED/BACK</th>
          <td id = "valorsnsr"> </td>
        </tr>
        <tr>
          <th>UPSHIFT</th>
          <td id = "valorups"> </td>
        </tr>
        <tr>
          <th>DOWNSHIFT</th> 
          <td id = "valordns"></td>
        </tr>
        <tr>
          <th>BRAKE SENSOR LEFT</th> 
          <td id = "valorbrsnrl"></td>
        </tr>
        <tr>
          <th>BRAKE SENSOR RIGHT</th> 
          <td id = "valorbrsnrr"></td>
        </tr>
        <tr>
          <th>PARKING BRAKE LOCK</th> 
          <td id = "valortrava"></td>
        </tr>
        <tr>
          <th>PARKING BRAKE P</th> 
          <td id = "valorpbmode"></td>
        </tr>
      </thead>
  </table>  
</div>

 <div class = "thirdcontent">
<table id="dadoslamina">
      <thead>
        <caption><h3>Lamina</h3>
        <tr>
          <th>PWM F/B</th>
          <td id = "valorlfwbk"> </td>
        </tr>
        <tr>
          <th>PWM L/R</th>
          <td id = "valorllfrg"> </td>
        </tr>
        <tr>
          <th>Trigger</th> 
          <td id = "valorlgat"></td>
        </tr>
        <tr>
          <th>SWITCH LEFT</th> 
          <td id = "valorlbote"></td>
        </tr>
        <tr>
          <th>SWITCH RIGHT</th> 
          <td id = "valorlbotd"></td>
        </tr>
        <tr>
          <th>KEY LEFT</th> 
          <td id = "valorlpoe"></td>
        </tr>
        <tr>
          <th>KEY RIGHT</th> 
          <td id = "valorlpod"></td>
        </tr>
      </thead>
  </table> 
 </div>
 
 </div>
<script>

         function ParaPerc(param) {
            
            var PWM = (param*100)/4096;
            PWM = Math.round(PWM);
            return PWM;
            
         }
         
         function ParaQualit(param){
         
           if (param == "0"){
              var VALUE = "OFF";
            }
            else if (param == "1"){
              var VALUE = "ON";
            }
            return VALUE;
         }
            
         var Socket;
         function init() {
            Socket = new WebSocket('ws://' + window.location.hostname + ':81/');
            Socket.onmessage = function(event) { 
            var data = JSON.parse(event.data);
            console.log(data);

           // ENTRADAS ANALOGICAS
            
            var decel = data.decel;
            var brake = data.brake;
            var ripio = data.ripio;
            var ripud = data.ripud;
            var llfrg = data.llfrg;
            var lfwbk = data.lfwbk;
            var snsr = data.snsr;
            /*var brsnrl = data.brsnrl; // X
            var brsnrr = data.brsnrr;   // X
            var keystart = data.keystart;*/ //X
            
            VALORdecel = ParaPerc(decel);
            VALORbrake = ParaPerc(brake);
            VALORripio = ParaPerc(ripio);
            VALORripud = ParaPerc(ripud);
            VALORllfrg = ParaPerc(llfrg);
            VALORlfwbk = ParaPerc(lfwbk);
            VALORsnsr = ParaPerc(snsr);
            /*VALORbrsnrl = ParaPerc(brsnrl); X
            VALORbrsnrr = ParaPerc(brsnrr);   X
            VALORkeystart = ParaPerc(keystart);*/ //X

            document.getElementById("valordecel").innerHTML = VALORdecel;
            document.getElementById("valorbrake").innerHTML = VALORbrake;
            document.getElementById("valorripio").innerHTML = VALORripio;
            document.getElementById("valorripud").innerHTML = VALORripud;
            document.getElementById("valorllfrg").innerHTML = VALORllfrg;
            document.getElementById("valorlfwbk").innerHTML = VALORlfwbk;
            document.getElementById("valorsnsr").innerHTML = VALORsnsr;
            /*document.getElementById("valorbrsnrl").innerHTML = VALORbrsnrl; X
            document.getElementById("valorbrsnrr").innerHTML = VALORbrsnrr;   X
            document.getElementById("valorkeystart").innerHTML = VALORkeystart;*/ //X
            
            // ENTRADAS DIGITAIS
            
            var RMMODE = data.RMMODE;
            var swrip = data.swrip;
            var lgat = data.lgat;
            var lpoe = data.lpoe;
            var lpod = data.lpod;
            var lbote = data.lbote;
            var lbotd = data.lbotd;
            var trava = data.trava;
            
            VALORRMMODE = ParaQualit(RMMODE);
            VALORswrip = ParaQualit(swrip);
            VALORlgat = ParaQualit(lgat);
            VALORlpoe = ParaQualit(lpoe);
            VALORlpod = ParaQualit(lpod);
            VALORlbote = ParaQualit(lbote);
            VALORlbotd = ParaQualit(lbotd);
            VALORtrava = ParaQualit(trava);
            
            if (data.pbmode == "0"){
              var VALORpbmode = "-";
            }
            else if (data.pbmode == "1"){
              var VALORpbmode = "LOCKED";
            }
            
            if (data.ups == "0"){
              var VALORups = "OPEN";
            }
            else if (data.ups == "1"){
              var VALORups = "CLOSED";
            }
            
            /*if (data.dns == "0"){
              var VALORdns = "OPEN";
            }
            else if (data.dns == "1"){
              var VALORdns = "CLOSED";
            }*/
            
            document.getElementById("valorRMMODE").innerHTML = VALORRMMODE;
            document.getElementById("valorswrip").innerHTML = VALORswrip;
            document.getElementById("valorlgat").innerHTML = VALORlgat;
            document.getElementById("valorlpoe").innerHTML = VALORlpoe;
            document.getElementById("valorlpod").innerHTML = VALORlpod;
            document.getElementById("valorlbote").innerHTML = VALORlbote;
            document.getElementById("valorlbotd").innerHTML = VALORlbotd;
            document.getElementById("valortrava").innerHTML = VALORtrava;
            document.getElementById("valorpbmode").innerHTML = VALORpbmode;
            document.getElementById("valorups").innerHTML = VALORups;
            //document.getElementById("valordns").innerHTML = VALORdns; X
                        
            }
         }
  </script>
    </body>
  </html>
)=====";

void setup()
{

  pinMode(RMMODE, INPUT);
  pinMode(swrip, INPUT);
  pinMode(lgat, INPUT);
  pinMode(lpoe, INPUT);
  pinMode(lpod, INPUT);
  pinMode(lbote, INPUT);
  pinMode(lbotd, INPUT);
  pinMode(trava, INPUT);
  pinMode(pbmode, INPUT); 
  pinMode(ups, INPUT);
  //pinMode(dns, INPUT); X
  //pinMode(keystart, INPUT); X
  pinMode(decel, INPUT);
  pinMode(brake, INPUT);
  pinMode(ripio, INPUT);
  pinMode(ripud, INPUT);
  pinMode(llfrg, INPUT);
  pinMode(lfwbk, INPUT);
  pinMode(snsr, INPUT);
  //pinMode(brsnrl, INPUT); X
  //pinMode(brsnrr, INPUT); X
  
  WiFi.begin(ssid,password);
  Serial.begin(PARAMS_SERIAL);
  Serial.print("Conectando à WiFi  ");
  Serial.print(ssid);
  while(WiFi.status()!=WL_CONNECTED)
  {
    Serial.print(".");
    delay(500);
  }
  Serial.println();
  Serial.print("Endereço IP da rede:  ");
  Serial.println(WiFi.localIP());

  //seleciona o endereço e as especificações HTTP para enviar os dados - 200 OK
  server.on("/",[](){
    server.send_P(200, "text/html", webpage);
  });
  server.begin();
  webSocket.begin();

  //inicializar o timer e os ADC
  timer.attach(/*rate seg*/ 0.1, /*callback function*/ readData);

  //criar uma instância WebSocket
  webSocket.onEvent(webSocketEvent);
}

// num é o número ID do cliente/conexão atual; type é o tipo de resposta ex.: 0 - 
// WStype_ERROR, 1 - WStype_DISCONNECTED, 2 - WStype_CONNECTED, 3 - WStype_TEXT, etc 
// payload são dados recebidos, se houver, e length é o tamanho dos dados

void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) 
{
  // vamos printar no monitor serial os dados que estão sendo enviados
  if (type == WStype_TEXT)
  {
    Serial.printf("payload [%u]: %s\n", num, payload);
  }
}

void remote_control(WiFiServer server){
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

void readData()
{
  // funcao callback, set flag
  read_data = true;
}

void loop()
{
  webSocket.loop();
  server.handleClient();

  // LEITURA DAS ENTRADAS

  if (read_data){

  //DIGITAIS:

  bool modoremotomanual = digitalRead(RMMODE);
  bool switchripper = digitalRead(swrip);
  bool gatilholamina = digitalRead(lgat);
  bool polegaresq = digitalRead(lpoe);
  bool polegardir = digitalRead(lpod);
  bool botaoesql = digitalRead(lbote);
  bool botaodirl = digitalRead(lbotd);
  bool travaparking = digitalRead(trava);
  bool parkingbrake = digitalRead(pbmode);
  bool upshift = digitalRead(ups);
  //bool downshift = digitalRead(dns); X

  //PWM:
  
  uint32_t desacelerador = analogRead(decel);
  uint32_t freio = analogRead(brake);
  uint32_t ripperinout = analogRead(ripio);
  uint32_t ripperupdown = analogRead(ripud);
  uint32_t laminalr = analogRead(llfrg);
  uint32_t laminafb = analogRead(lfwbk);
  uint32_t sensordir = analogRead(snsr);
  //uint32_t brakesensorl = analogRead(brsnrl); X
  //uint32_t brakesensorr = analogRead(brsnsr); X
  
  // ADC:

  //uint16_t chavepartida = analogRead(keystart); X

  // string json para armazenar os dados
  
  String Datamodorm = String(modoremotomanual);
  String Dataswitchr = String(switchripper);
  String Datagatilho = String(gatilholamina);
  String Datapolesq = String(polegaresq);
  String Datapoldir = String(polegardir);
  String Databotesq = String(botaoesql);
  String Databotdir = String(botaodirl);
  String Datatrava = String(travaparking);
  String Datapark = String(parkingbrake);
  String Dataup = String(upshift);
  //String Datadown = String(downshift); X
  String Datadesa = String(desacelerador);
  String Datafrei = String(freio);
  String Datainout = String(ripperinout);
  String Dataupdown = String(ripperupdown);
  String Datalr = String(laminalr);
  String Datafb = String(laminafb);
  String Datasnr = String(sensordir);
  //String Databksnrl = String(brakesensorl); X
  //String Databksnrr = String(brakesensorr); X
  //String Datachave = String(chavepartida);  X

  String json = "{\"RMMODE\":";
  json += Datamodorm;
  json += ", \"swrip\":";
  json += Dataswitchr;
  json += ", \"lgat\":";
  json += Datagatilho;
  json += ", \"lpoe\":";
  json += Datapolesq;
  json += ", \"lpod\":";
  json += Datapoldir;
  json += ", \"lbote\":";
  json += Databotesq;
  json += ", \"lbotd\":";
  json += Databotdir;
  json += ", \"trava\":";
  json += Datatrava;
  json += ", \"pbmode\":";
  json += Datapark;
  json += ", \"ups\":";
  json += Dataup;
  //json += ", \"dns\":"; 
  //json += Datadown; X
  json += ", \"decel\":";
  json += Datadesa;
  json += ", \"brake\":";
  json += Datafrei;
  json += ", \"ripio\":";
  json += Datainout;
  json += ", \"ripud\":";
  json += Dataupdown;
  json += ", \"llfrg\":";
  json += Datalr;
  json += ", \"lfwbk\":";
  json += Datafb;
  json += ", \"snsr\":";
  json += Datasnr;
  //json += ", \"brsnrl\":";
  //json += Databksnrl; X
  //json += ", \"brsnrr\":";
  //json += Databksnrr; X
  //json += ", \"keystart\":";
  //json += Datachave; X
  json += " }";

  Serial.println(json);
  webSocket.broadcastTXT(json.c_str(), json.length());
  
  // reset flag
  read_data = false;
  }
}
