/*
   Hardware Pinout Connection
   Arduino Nano        SD Pin
        5v ------------ VCC
        GND ----------- GND
        D10 ----------- CS
        D11 ----------- MOSI
        D12 ----------- MISO
        D13 ----------- SCK
  ________________________________________
   Arduino Nano         MAX9814
        3.3v ----------- VDD
        GND ------------ GND
        A0 -------------  Out
        AR  ------------ GND
        GAIN ------------ GND
  ______________________________
    Микрофон 1
    A1 -------------  Out__________
  --------------------------------
  A/R
Trilevel Attack and Release Ratio Select. Controls the ratio of attack time to release time for the
AGC circuit.
A/R = GND: Attack/Release Ratio is 1:500
A/R = VDD: Attack/Release Ratio is 1:2000
A/R = Unconnected: Attack/Release Ratio is 1:4000
Trilevel Amplifier Gain Control.
GAIN = VDD, gain set to 40dB.
GAIN = GND, gain set to 50dB.
GAIN = Unconnected, uncompressed gain set to 60dB
   Arduino Nano D2 pin user for Led to notify that record is in process.
*/
/*
   use Link: https://www.arduino.cc/reference/en/libraries/tmrpcm/ TMRpcm library for recording audio using MAX9814
   Recording a WAV file to an SD card is an advanced feature of the TMRpcm library so you must edit the library configuration file in order to use it.
   It simply searches the file "pcmConfig.h" using File Explorer and disables a few lines of code (then saves it).
    1. On Uno or non-mega boards uncomment the line #define buffSize 128
    2. Also uncomment #define ENABLE_RECORDING and #define BLOCK_COUNT 10000UL
*/
#include <TMRpcm.h>
#include <SD.h>
#include <SPI.h>
#include <EEPROM.h>

TMRpcm audio;
//int file_number = 0;
int file_number = EEPROM.read(0);

char filePrefixname1[50] = "1mik";
char filePrefixname2[50] = "2mik";
char exten[10] = ".wav";
const int recordLed = 2; //пин индикаторый 
const int mic_pin1 = A1; //пин к которому подключен микрофон 1
const int mic_pin2 = A4; //пин к которому подключен микрофон 2
const int sample_rate = 8000; //16000
const int MaxAnalogPinValue = 380; //пороговое значение
#define SD_CSPin 10
// с помощью этой функции мы затем будем отмерять 2-х минутные интервалы

void setup() {
  // put your setup code here, to run once:
  //initialises the serial connection (инициализируем последовательную связь) between the arduino and any connected serial device(e.g. computer, phone, raspberry pi...)
  Serial.begin(9600);
  //задаем режимы работы используемых контактов
  pinMode(mic_pin1, INPUT);
  pinMode(mic_pin2, INPUT);
  pinMode(recordLed, OUTPUT);
  Serial.println("loading... SD card");
  if (!SD.begin(SD_CSPin)) {
    Serial.println("An Error has occurred while mounting SD");
  }
  while (!SD.begin(SD_CSPin)) {
    Serial.print(".");
    delay(500);
  }
  audio.CSPin = SD_CSPin;
}
void loop() {
  Serial.println("####################################################################################");
  //формируем имя для первого микрофофна
  char fileSlNum[20] = "";
  itoa(file_number, fileSlNum, 10);
  char file_name1[70] = "";
  strcat(file_name1, filePrefixname1);
  strcat(file_name1, fileSlNum);
  strcat(file_name1, exten);
  // формируем имя для второго микрофона
  char fileSlNum2[20] = "";
  itoa(file_number, fileSlNum2, 10);
  char file_name2[70] = "";
  strcat(file_name2, filePrefixname2);
  strcat(file_name2, fileSlNum2);
  strcat(file_name2, exten);
  int AnR1 = analogRead(mic_pin1);
  int AnR2 = analogRead(mic_pin2);
   Serial.println(AnR1);
   Serial.println(AnR2);
  if (AnR1 >= MaxAnalogPinValue) {
   digitalWrite(recordLed, HIGH);
   Serial.println("startRecording1");
   audio.startRecording(file_name1, sample_rate, mic_pin1);
   delay(60000);
   audio.stopRecording(file_name1);
   digitalWrite(recordLed, LOW);
   file_number++;
   if (file_number > 32766)file_number = 0;
   EEPROM.write(0, file_number);
  }
   if (AnR2 >= MaxAnalogPinValue) {
   digitalWrite(recordLed, HIGH);
   Serial.println("startRecording2");
   audio.startRecording(file_name2, sample_rate, mic_pin2);
   delay(60000);
   audio.stopRecording(file_name2);
   digitalWrite(recordLed, LOW);
   file_number++;
   if (file_number > 32766)file_number = 0;
   EEPROM.write(0, file_number);
  }
}
