//--------------------LIBRERIAS ----------------//
#include "DHT.h"
#include "TM1637.h"
#include "SD.h"
//--------------------DEFINICIONES--------------//
#define DHTTYPE DHT22   
TM1637 Display(8,9);
File Archivo;
int sd = 10;                                               // SD
float h;                                                   // HUMEDAD
const int DHTPin = 5;                                      // PING DIGITAL CONECTADO 
int ledLectura=13;                                         // LED DE LA LECTURA
boolean pinStateLectura = true;                            // ESTADO LED DE LA LECTURA
DHT dht(DHTPin, DHTTYPE);
int8_t Digits[4];                                          // PARA DISPLAY

//--------------------SETUP---------------------//
void setup() {
   Serial.begin(9600);
   Serial.println("INICIO:");
   pinMode(ledLectura,OUTPUT);                             // PIN QUE INDICA LECTURA
   pinMode(sd, OUTPUT);
   if (!SD.begin(sd)){                                    // COMPRUEBA QUE SE CONECTO A LA SD
    Serial.println("Se ha producido un fallo al iniciar la comunicación");
    return;
  }
  Serial.println("Se ha iniciado la comunicación correctamente");
   dht.begin();
   Display.set();
   Display.init();
   Display.point(POINT_ON); 
}

//--------------------LOOP-----------------------//
void loop() {
  lecturaHumedad();
  if ((h < 80) and (h > 30)){
    //SUPER
    //FALTA GRABAR EN SD
    }else {
    //PROBLEMAS
     //FALTA GRABAR EN SD Y GSM
    }
  escrituraSD();
  //Serial.print(dimeFecha());                                     // MUESTRA DIA Y HORA
  delay(2000);
   
}
//--------------------FUNCIONES------------------//

//-----------------------------------------------//
void lecturaHumedad(){                                             // FUNCION : LECTURA DE HUMEDAD
   h = dht.readHumidity();                                         // LEO HUMEDAD
   if (isnan(h)) {
      Serial.println("Failed to read from DHT sensor!");
      return;
   }
   digitalWrite(ledLectura, pinStateLectura);
   Serial.print("Humidity: ");
   Serial.print(h);
   MostrarHumedad(h);
   Serial.print(" %\n");
   pinStateLectura = !pinStateLectura;
  }
  
//-----------------------------------------------//
void MostrarHumedad( float Num2){                                 // FUNCION : MUESTRA EN DISPLAY LA HUMEDAD
int Num;
int decimal;
Num = int(Num2);
decimal = int((Num2 - Num) * 100);
int8_t Digit0 = Num %10 ;
int8_t Digit1 = (Num % 100) / 10;
int8_t Digit2 = decimal %10 ;
int8_t Digit3 = (decimal % 100) / 10;
//int8_t Digit2 = (Num % 1000) / 100;
//int8_t Digit3 = (Num / 1000);
int8_t Digits[] = {Digit1,Digit0,Digit3,Digit2};
Display.display(Digits);                                          // MUESTRA EN DISPLAY
}

//-----------------------------------------------//
void escrituraSD() {                                             // FUNCION ESCRITURA EN SD
  Archivo = SD.open("datos.txt", FILE_WRITE);
  if (Archivo) {
    Archivo.println("Esto es lo que se está escribiendo en el archivo");
  Archivo.close();
  }
  else {
    Serial.println("El archivo datos.txt no se abrió correctamente");           // ERROR
  }
}

//-----------------------------------------------//
/*String dimeFecha(){                                               // FUNCION : OBTENER DIA Y HORA
  char fecha[20];
  DateTime now = RTC.now(); //Obtener fecha y hora actual.

  int dia = now.day();
  int mes = now.month();
  int anio = now.year();
  int hora = now.hour();
  int minuto = now.minute();
  int segundo = now.second();

  sprintf( fecha, "%.2d.%.2d.%.4d %.2d:%.2d:%.2d", dia, mes, anio, hora, minuto, segundo);
  return String( fecha );
}  */


