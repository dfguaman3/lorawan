#include "SoftwareSerial.h"
#include <TimeLib.h>
SoftwareSerial mySerial(10,11);
String response;
String arreglo;
String flujo ="0";
volatile int NumPulsos; //variable para la cantidad de pulsos recibidos
int PinSensor = 2;    //Sensor conectado en el pin 2
float factor_conversion=4.8; //para convertir de frecuencia a caudal
float volumen=0;
long dt=0; //variación de tiempo por cada bucle
long t0=0; //millis() del bucle anterior

//---Función que se ejecuta en interrupción---------------
void ContarPulsos (){ 
  NumPulsos++;  //incrementamos la variable de pulsos
} 
//---Función para obtener frecuencia de los pulsos--------
int ObtenerFrecuecia(){
  int frecuencia;
  NumPulsos = 0;   //Ponemos a 0 el número de pulsos
  interrupts();    //Habilitamos las interrupciones
  delay(5000);   //muestra de 1 segundo
  noInterrupts(); //Deshabilitamos  las interrupciones
  frecuencia=NumPulsos; //Hz(pulsos por segundo)
  return frecuencia;
}
///////////////////////////SETUP/////////////////////////
void setup() {
  Serial.begin(9600);
  mySerial.begin(9600);
  setTime(20,43,50,7,12,2020);//AJUSTAR LA FECHA Y HORA QUE SE COMPILA
  pinMode(PinSensor, INPUT);
  attachInterrupt(0,ContarPulsos,RISING);//(Interrupción 0(Pin2),función,Flanco de subida)
  t0=millis();
  
  delay(10000);
  // waiting to WisNode to join to the network
  sendCommand("at+join\r\n");
  
  delay(20000);//wait for 10 sec until join network LoRa

}
/////////////////////////LOOP//////////////////////////
void loop() {
    time_t t = now();
//  readLora();
    float frecuencia=ObtenerFrecuecia(); //obtenemos la frecuencia de los pulsos en Hz
  float caudal_L_m=frecuencia/factor_conversion; //calculamos el caudal en L/m
  dt=millis()-t0; //calculamos la variación de tiempo
  t0=millis();
  volumen=volumen+(caudal_L_m/60)*(dt/1000); // volumen(L)=caudal(L/s)*tiempo(s) 
  flujo=int(caudal_L_m)+2;
  //3A3312=0
  if (int(caudal_L_m) >0){
    if (second(t)==00){//ENVIO CADA MINUTO
      if (flujo.length()<2){
          String flujo2 = "3A331";
          flujo2.concat(flujo);
          sendData(1,flujo2);
          delay(10000);
          Serial.println(flujo2);
      }else{
        String flujo3 = "3A33";
        flujo3.concat(flujo);
        sendData(1,flujo3);
        delay(10000);
        Serial.println(flujo3);
        }
    }
   }  
     
}


void medicion_flujo (){
    float frecuencia=ObtenerFrecuecia(); //obtenemos la frecuencia de los pulsos en Hz
    float caudal_L_m=frecuencia/factor_conversion; //calculamos el caudal en L/m
    dt=millis()-t0; //calculamos la variación de tiempo
    t0=millis();
    volumen=volumen+(caudal_L_m/60)*(dt/1000); // volumen(L)=caudal(L/s)*tiempo(s) 
    flujo=int(caudal_L_m)+2;
    //3A3312=0
    if (flujo.length()<2){
        String flujo2 = "3A331";
        flujo2.concat(flujo);
        Serial.println(flujo2);
    }else{
      String flujo3 = "3A33";
      flujo3.concat(flujo);
      Serial.println(flujo3);
      }
}

void sendData(int port, String data){
  String command = "at+send=lora:" + (String)port + ":" + data;
  sendCommand(command);
 
}

void sendCommand(String atComm){
  Serial.println(atComm);//data send to gateway loora
  mySerial.println(atComm);//response from module LoRa
  readLora();
}

String readLora() {
  response = "";
  while(mySerial.available()){
  char ch = mySerial.read();
  response += ch;
  arreglo=response;
  }
  
  Serial.println(response);
  return response;
}
