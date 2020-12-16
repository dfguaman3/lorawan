#include "RAK811.h"
#include "SoftwareSerial.h"
#include <TimeLib.h>
#define WORK_MODE LoRaWAN   //  LoRaWAN or LoRaP2P
#define JOIN_MODE ABP    //  OTAA or ABP
#if JOIN_MODE == OTAA
String DevEui = "00781B9A84839F60";
String AppEui = "70B3D57ED003605B";
String AppKey = "DDDFB1023885FBFF74D3A55202EDF2B1";
#else JOIN_MODE == ABP
String NwkSKey = "95B68170A4B6A826559A83C99409814C";
String AppSKey = "616A51C0E66E94BDBC97227A3CD27116";
String DevAddr = "26021B00";
#endif
#define TXpin 11   // Set the virtual serial port pins
#define RXpin 10
#define DebugSerial Serial
SoftwareSerial ATSerial(RXpin,TXpin);    // Declare a virtual serial port
int valor;
int salida1 = 2;
int salida2 = 4;
String humedad;

bool InitLoRaWAN(void);
RAK811 RAKLoRa(ATSerial,DebugSerial);


void setup() {
  pinMode(salida1, OUTPUT);
  pinMode(salida2, OUTPUT);
  digitalWrite(salida1, HIGH);
  digitalWrite(salida2, HIGH);
  setTime(16,49,45,10,11,2020);//AJUSTAR LA FECHA Y HORA QUE SE COMPILA
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
  if (minute(t)==00||minute(t)%10==0){//ENVIO CADA 5 MINUTOS
    DebugSerial.println(F("Sending data"));
    ///////////LECTURA//////////
 
    ////////////////////////////
    if (RAKLoRa.rk_sendData(1, buffer)){    
      for (unsigned long start = millis(); millis() - start < 20000L;){//INTERVALO DE ENVIO
        String ret = RAKLoRa.rk_recvData();
        if(ret != ""){ 
          DebugSerial.println(ret);
        }
        
        if(ret.indexOf("31")>0){
          DebugSerial.println("OPEN");
          digitalWrite(salida1, LOW);
          delay(10000);
          digitalWrite(salida1, HIGH);
          }
        if(ret.indexOf("30")>0){
          DebugSerial.println("CLOSE");
          digitalWrite(salida2, LOW);
          delay(10000);
          digitalWrite(salida2, HIGH);
          }
          
      }
    }
  }
}
