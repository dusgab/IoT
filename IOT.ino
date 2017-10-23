//--------------------LIBRERIAS ----------------//
#include "Wire.h"
#include "RTClib.h"
#include "Time.h"
#include "DHT.h"
#include "TM1637.h"
#include "SD.h"
//--------------------DEFINICIONES--------------//
//-------VARIABLES------//
//String HumAct;                                              // VARIABLE QUE GUARDA LA HUMEDAD ACTUAL PARA GUARDEN LA SD
int HumMax = 60;                                           // VARIABLE DE HUMEDAD MAXIMA
int HumMin = 30;                                           // VARIABLE DE HUMEDAD MINIMA
String aGrabar;                                            // VARIABLE QUE SE GRABA EN LA SD
//String aGrabar2;                                            // VARIABLE QUE SE GRABA EN LA SD
int error;                                                 // VARIABLE QUE DETERMINA SI ES MAYOR(1) O MENOR(2) LA HUMEDAD
int delay2 = 0;                                            // VARIABLE USADA PARA DETERMINAR EL TIEMPO ENTRE CADA LECTURA DE HUMEDAD
char fecha[20];                                            // VARIABLE USADA PARA OBTENER FECHA Y HORA PARA EL GUARDADO EN LA SD
float h;                                                   // VARIABLE HUMEDAD
File Archivo;                                              // VARIABLE DE SD
//---------PINES--------//
int ledLectura=13;                                         // LED DE LA LECTURA
int sd = 10;                                               // SD                
TM1637 Display(8,9);                                       // CONEXION DE DISAPLAY                                
const int DHTPin = 5;                                      // DHT LECTOR DE HUMEDAD
boolean pinStateLectura = true;                            // ESTADO LED DE LA LECTURA
int8_t Digits[4];                                          // PARA DISPLAY
//--------MODULOS-------//
#define DHTTYPE DHT22 
DHT dht(DHTPin, DHTTYPE);
RTC_DS1307 rtc;

//------------------------------------------------------------------//
//--------------------FORMATO DE GRABACION DE SD--------------------//
//-------bien---------23102017135757603045700000--------------------//
//-------error--------23102017135757603045700011--------------------//
//---fecha(8)hora(6)humMax(2)humMin(2)humAct(4)error(3)bandera(1)---//
//------------------------------------------------------------------//

//----------------------------------------------//
//--------------------SETUP---------------------//
//----------------------------------------------//
void setup() {
   rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));      // LINEA COMENTADA SOLO SE OCUPA LA PRIMERA  VEZ PARA PONER EN HORA EL MODULO RTC
   Serial.begin(9600);                                     // INICIAMOS EL MONITOR SERIE PARA PRUEBAS
   Serial.println("INICIO MEDIDOR DE HUMEDAD:");
    Serial.print("TEMPERATURA MAX :");
     Serial.print(HumMax);
      Serial.print("  ");
     Serial.print("TEMPERATURA MIN:");
      Serial.println(HumMin);
   pinMode(ledLectura,OUTPUT);                             // PIN QUE INDICA LECTURA
   pinMode(sd, OUTPUT);
   SD.begin(sd);
   Wire.begin();
   dht.begin();                                            // INICIA LECTOR DE HUMEDAD
   Display.set();
   Display.init();
   Display.point(POINT_ON); 
   if (!SD.begin(sd)){                                     // COMPRUEBA QUE SE CONECTO A LA SD
      Serial.println("Se ha producido un fallo al iniciar la comunicación con la sd");
      return;
   }else{
      Serial.println("Se ha iniciado la comunicación correctamente con la sd");
   }
   rtc.begin();
  if (!rtc.begin()) {                                     // COMPRUEBA QUE SE CONECTO EL MODULO RTC 
      Serial.println("Se ha producido un fallo en el modulo RTC");
    }else {
      Serial.println("Se ha conectado correctamente el modulo RTC");
    }
   
}

//----------------------------------------------//
//--------------------LOOP----------------------//
//----------------------------------------------//
void loop(){
  aGrabar = "";
  aGrabar += dimeFecha();
  aGrabar += HumMax;
  aGrabar += HumMin;  
  leerHumedad();                                           // GRABA EN H LA HUMEDAD
  mostrarHumedad(h);                                       // MUESTRA HUMEDAD POR DISPLAY
  escrituraSD(aGrabar);
  //comprobacionYescritura();                                //
  // delayy();
   Serial.println("");Serial.println("");Serial.println("");Serial.println("");
  delay(5000);
}

//-----------------------------------------------// 
//--------------------FUNCIONES------------------//
//-----------------------------------------------//
void leerHumedad(){                                        // FUNCION : LECTURA DE HUMEDAD
   h = dht.readHumidity();                                 // LEO HUMEDAD
   if (isnan(h)) {
      Serial.println("Failed to read from DHT sensor!");
      return;
   }
   digitalWrite(ledLectura, pinStateLectura);
   Serial.print("Humedad Actual: ");
   Serial.print(h);
   Serial.print(" %\n");
   pinStateLectura = !pinStateLectura;
  }
  
//-----------------------------------------------//
void mostrarHumedad( float Num2){                          // FUNCION : MUESTRA EN DISPLAY LA HUMEDAD
int Num;
int decimal;
Num = int(Num2);
decimal = int((Num2 - Num) * 100);
aGrabar += Num;
aGrabar += decimal;
//aGrabar2 += HumAct;

int8_t Digit0 = Num %10 ;
int8_t Digit1 = (Num % 100) / 10;
int8_t Digit2 = decimal %10 ;
int8_t Digit3 = (decimal % 100) / 10;
//int8_t Digit2 = (Num % 1000) / 100;
//int8_t Digit3 = (Num / 1000);
int8_t Digits[] = {Digit1,Digit0,Digit3,Digit2};
Display.display(Digits);                                    // MUESTRA EN DISPLAY
}

//-----------------------------------------------//
void escrituraSD(String texto) {                            // FUNCION ESCRITURA EN SD
  Archivo = SD.open("datos.txt", FILE_WRITE);
  if (Archivo) {
    Serial.print("Esto se va a grabar : ");
    Serial.println(texto);
  Archivo.println(texto);
  Archivo.close();
  }
  else {
    Serial.println("El archivo datos.txt no se abrió correctamente");        // ERROR
  }
}

//-----------------------------------------------//
String dimeFecha(){   
   DateTime now = rtc.now(); //Obtener fecha y hora actual.
 
 // FUNCION : OBTENER DIA Y HORA         ----Serial.print(dimeFecha());
 
  int dia = now.day();
  int mes = now.month();
  int anio = now.year();
  int hora = now.hour();
  int minuto = now.minute();
  int segundo = now.second();
  sprintf( fecha, "%.2d%.2d%.4d%.2d%.2d%.2d", dia, mes, anio, hora, minuto, segundo);
  return String( fecha );
}  

//------------------------------------------------//
void comprobacionYescritura(){
  if ((HumMax > h) and (h > HumMin)){
    //SUPER
    //Serial.print(dimeFecha());                            // MUESTRA DIA Y HORA
    //FALTA GENERAR EL STRING PARA GRABAR
    escrituraSD("string formado por todo");
    delay2 = 0;
    }else{
      if (HumMax < h){error = 1;} else {error = 2;}         // DETERMINA SI ES MAYOR O MENOR LA HUMEDAD
    //PROBLEMAS
    //FALTA GENERAR EL STRING PARA GRABAR Y GSM
    //Serial.print(dimeFecha()); */                         // MUESTRA DIA Y HORA
    escrituraSD("string formado por todo");
    delay2 = 1;
    }
}

//------------------------------------------------//
void delayy(){                                              // DETERMINA CON QUE FRECUENCIA SE HACE EL LOOP , POR 
  if (delay2 == 0){                                         // ENDE LA FRECUENCIA DE LA LECTURA Y CONTROL DE HUMEDAD
    delay(10000);
  }else{
    delay(2000);
  }
}


