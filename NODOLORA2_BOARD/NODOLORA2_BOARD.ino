#include "RAK811.h"
#include "SoftwareSerial.h"
#include <TimeLib.h>
#define WORK_MODE LoRaWAN   //  LoRaWAN or LoRaP2P
#define JOIN_MODE ABP    //  OTAA or ABP
#if JOIN_MODE == OTAA
String DevEui = "0088055B0DFB7577";
String AppEui = "70B3D57ED003605B";
String AppKey = "DDDFB1023885FBFF74D3A55202EDF2B1";
#else JOIN_MODE == ABP
String NwkSKey = "F4C01AB4BCA125783CBEE0F4DB401EBF";
String AppSKey = "6E9B8945D3E61E2BDFFF9891644559F2";
String DevAddr = "260211DD";
#endif
#define TXpin 11   // Set the virtual serial port pins
#define RXpin 10
#define DebugSerial Serial
SoftwareSerial ATSerial(RXpin,TXpin);    // Declare a virtual serial port
int valor;
char control;
char valvula;
char data;
char flujo;
String humedad;

bool InitLoRaWAN(void);
RAK811 RAKLoRa(ATSerial,DebugSerial);


void setup() {
  pinMode(2, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);
  digitalWrite(2, HIGH);
  digitalWrite(4, HIGH);
  digitalWrite(5, HIGH);
  setTime(16,44,45,24,11,2020);//AJUSTAR LA FECHA Y HORA QUE SE COMPILA
  DebugSerial.begin(9600);

  while(DebugSerial.available())
  {
    DebugSerial.read(); 
  }
  ATSerial.begin(9600); //set ATSerial baudrate:This baud rate has to be consistent with  the baud rate of the WisNode device.
  while(ATSerial.available())
  {
    ATSerial.read(); 
  }
while(!RAKLoRa.rk_joinLoRaNetwork(60))  //Joining LoRaNetwork timeout 60s
  {
    DebugSerial.println();
    DebugSerial.println(F("Rejoin again after 5s..."));
    delay(5000);
  }
  DebugSerial.println(F("Join LoRaWAN success"));
  
}



void loop() {
  char buffer[]= "";
  time_t t = now();
  const int h = 10+analogRead(A0)/10;
  if (second(t)==00){//ENVIO CADA 5 MINUTOS
    DebugSerial.println(F("Sending data"));
    if (RAKLoRa.rk_sendData(1, buffer)){    
      for (unsigned long start = millis(); millis() - start < 20000L;){
        String ret = RAKLoRa.rk_recvData();
        if(ret != ""){ 
          DebugSerial.println(ret);
        }
        if(ret.indexOf("33")>0){
            control='3';//CONTROL MANUAL
            Serial.println("MODO MANUAL");
            }
        if(ret.indexOf("32")>0){
          control='2';//CONTROL AUTOMATICO
          Serial.println("MODO AUTOMATICO");
          }
        if(ret.indexOf("30")>0){
          valvula='0';//CERRAR VALVULA
          Serial.println("CERRAR VALVULA");
          }
        if(ret.indexOf("31")>0){
          valvula='1';//ABRIR VALVULA
          Serial.println("ABRIR VALVULA");
          }
           /////////////CONTROL MANUAL//////////////
          if(control=='3'&&valvula=='1'){
            digitalWrite(5, LOW);
            digitalWrite(2, LOW);//ACIVA LA SALIDA 2 POR 15 SEG
            delay(15000);
            digitalWrite(2, HIGH);
            digitalWrite(5, HIGH);
            valvula = '8';
            }
          if(control=='3'&&valvula=='0'){
            digitalWrite(5, LOW);
            digitalWrite(4, LOW);//ACIVA LA SALIDA 4 POR 15 SEG
            delay(15000);
            digitalWrite(4, HIGH);
            digitalWrite(5, HIGH);
            valvula = '9';
            }
             /////////AUTOMATICO//////////////
          if(control=='2'){//MODO AUTOMATICO
            if (h<=30){//Si hay flujo y si la humedad es menor que 50
            digitalWrite(5, LOW);
            digitalWrite(2, LOW);//ABRE LA VALVULA
            delay(15000);
            digitalWrite(2, HIGH);
            digitalWrite(5, HIGH);
              }
            if(h>=50){
              digitalWrite(5, LOW);
              digitalWrite(4, LOW);//CIERRA LA VALVULA
              delay(15000);
              digitalWrite(4, HIGH);
              digitalWrite(5, HIGH);
              }
            }
      }
    }
  }
}
