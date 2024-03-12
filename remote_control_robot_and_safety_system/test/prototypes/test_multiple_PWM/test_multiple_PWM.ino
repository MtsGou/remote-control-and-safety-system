// Testing PWM for robot control with H bridge

const int LED32 = 32;
const int LED33 = 33;
const int LED25 = 25;
const int LED26 = 26;
const int LED27 = 27;
const int LED14 = 14;
const int LED12 = 12;
const int LED13 = 13;
const int LED9 = 9;
const int LED10 = 10;
const int LED11 = 11;
const int LED19 = 19;
const int LED18 = 18;
const int LED2 = 2;
const int LED15 = 15;
const int LED0 = 0;
const int LED21 = 21;
const int LED22 = 22;
const int LED23 = 23;

const int CHAVE_4 = 4;
const int CHAVE_16 = 16;
const int CHAVE_17 = 17;
const int CHAVE_5 = 5;

const int channel[16] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};

const int freq_1 = 500;
const int freq_2 = 4700;

const long baud_rate = 115200;

const int resolution1 = 8;

const int resolution2 = 16;

void setup() {

  Serial.begin(baud_rate);
  pinMode(CHAVE_4, OUTPUT);
  pinMode(CHAVE_16, OUTPUT);
  pinMode(CHAVE_17, OUTPUT);
  pinMode(CHAVE_5, OUTPUT);
  
  ledcAttachPin(LED15, channel[0]);
  ledcSetup(channel_1, freq_1, resolution1);
  
  ledcAttachPin(LED2, channel[1]);
  ledcSetup(channel_2, freq_1, resolution1);

  ledcAttachPin(LED0, channel[2]);
  ledcSetup(channel_3, freq_1, resolution1);

  ledcAttachPin(LED18, channel[3]);
  ledcSetup(channel_4, freq_1, resolution1);

  ledcAttachPin(LED19, channel[4]);
  ledcSetup(channel_5, freq_1, resolution2);

  ledcAttachPin(LED21, channel[5]);
  ledcSetup(channel_6, freq_1, resolution2);

  ledcAttachPin(LED22, channel[6]);
  ledcSetup(channel_7, freq_1, resolution2);

  ledcAttachPin(LED23, channel[7]);
  ledcSetup(channel_8, freq_1, resolution2);
  
  ledcAttachPin(LED32, channel[8]);
  ledcSetup(channel_9, freq_2, resolution1);

  ledcAttachPin(LED33, channel[9]);
  ledcSetup(channel_10, freq_2, resolution1);

  ledcAttachPin(LED25, channel[10]);
  ledcSetup(channel_11, freq_2, resolution1);

  ledcAttachPin(LED26, channel[11]);
  ledcSetup(channel_12, freq_2, resolution1);
  
  ledcAttachPin(LED27, channel[12]);
  ledcSetup(channel_13, freq_2, resolution1);

  ledcAttachPin(LED14, channel[13]);
  ledcSetup(channel_14, freq_2, resolution2);

  ledcAttachPin(LED12, channel[14]);
  ledcSetup(channel_15, freq_2, resolution2);

  ledcAttachPin(LED13, channel[15]);
  ledcSetup(channel_16, freq_2, resolution2);
}

void loop() {

  for (int i = 0; i < sizeof(channel) / sizeof(int); i++) {
    ledcWrite(channel[i], 0);
    delay(500);
    ledcWrite(channel[i], 10);
    delay(500);
    ledcWrite(channel[i], 60);
    delay(500);
    ledcWrite(channel[i], 150);
    delay(500);
    ledcWrite(channel[i], 255);
    delay(1000);

    for (int j = 0; j < 255; j++) {
      ledcWrite(channel[i], i);
    }
    delay(200);
    ledcWrite(channel[i], 0);
    delay(1000);

    digitalWrite(CHAVE_4, bitRead(i + 1, 0));
    digitalWrite(CHAVE_16, bitRead(i + 1, 1));
    digitalWrite(CHAVE_17, bitRead(i + 1, 2));
    digitalWrite(CHAVE_5, bitRead(i + 1, 3));
  }
}
