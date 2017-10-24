//--------------------LIBRERIAS ----------------//
#include "Wire.h"
#include "RTClib.h"
#include "Time.h"
#include "DHT.h"
#include "TM1637.h"
#include "SD.h"
//--------------------DEFINICIONES--------------//
//-------VARIABLES------//
//String HumAct;                                           // VARIABLE QUE GUARDA LA HUMEDAD ACTUAL PARA GUARDEN LA SD
int HumMax = 60;                                           // VARIABLE DE HUMEDAD MAXIMA
int HumMin = 30;                                           // VARIABLE DE HUMEDAD MINIMA
String aGrabar;                                            // VARIABLE QUE SE GRABA EN LA SD
char fecha[12];                                            // VARIABLE USADA PARA OBTENER FECHA Y HORA PARA EL GUARDADO EN LA SD
float h;                                                   // VARIABLE HUMEDAD
File Archivo;                                              // VARIABLE DE SD
//---------PINES--------//
int sd = 10;                                               // SD                
TM1637 Display(8,9);                                       // CONEXION DE DISAPLAY                                
const int DHTPin = 5;                                      // DHT LECTOR DE HUMEDAD
int8_t Digits[4];                                          // PARA DISPLAY
//--------MODULOS-------//
#define DHTTYPE DHT22 
DHT dht(DHTPin, DHTTYPE);
RTC_DS1307 rtc;

//------------------------------------------------------------------//
//---------------------FORMATO DE GRABACION DE SD-------------------//
//-------bien------------23102017135757603045700--------------------//
//-------error-----------23102017135757603045701--------------------//
//-------fecha(8)hora(6)humMax(2)humMin(2)humAct(4)bandera(1)-------//
//------------------------------------------------------------------//

//----------------------------------------------//
//--------------------SETUP---------------------//
//----------------------------------------------//
void setup() {
   
   Serial.begin(9600);                                     // INICIAMOS EL MONITOR SERIE PARA PRUEBAS
   Serial.println(F("INICIO MEDIDOR DE HUMEDAD:"));
   Serial.print(F("HUMEDAD MAXIMA :"));
   Serial.println(HumMax);
   Serial.print(F("HUMEDAD MINIMA :"));
   Serial.println(HumMin);
   pinMode(sd, OUTPUT);
   SD.begin(sd);
   Wire.begin();
   dht.begin();                                            // INICIA LECTOR DE HUMEDAD
   Display.set();
   Display.init();
   Display.point(POINT_ON); 
   if (!SD.begin(sd)){                                     // COMPRUEBA QUE SE CONECTO A LA SD
      Serial.println(F("Error de comunicación con la sd"));
      return;
   }else{
      Serial.println(F("Correcta comunicación con la sd"));
   }
  rtc.begin();
  if (!rtc.begin()) {                                     // COMPRUEBA QUE SE CONECTO EL MODULO RTC 
      Serial.println(F("Error en el modulo RTC"));
    }else {
      Serial.println(F("Conectado correctamente el modulo RTC"));
    }
   //rtc.adjust(DateTime(F(__DATE__),F(__TIME__)));      // LINEA COMENTADA SOLO SE OCUPA LA PRIMERA  VEZ PARA PONER EN HORA EL MODULO RTC
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
  comprobacionYescritura();                                //
  Serial.println(F("\n \n"));
  
}

//-----------------------------------------------// 
//--------------------FUNCIONES------------------//
//-----------------------------------------------//
void leerHumedad(){                                        // FUNCION : LECTURA DE HUMEDAD
   h = dht.readHumidity();                                 // LEO HUMEDAD
   if (isnan(h)) {
      Serial.println(F("Failed to read from DHT sensor!"));
      return;
   }
   Serial.print(F("Humedad Actual: "));
   Serial.print(h);
   Serial.print(F(" %\n"));
  }
  
//-----------------------------------------------//
void mostrarHumedad( float Num2){                          // FUNCION : MUESTRA EN DISPLAY LA HUMEDAD
int Num;
int decimal;
Num = int(Num2);
decimal = int((Num2 - Num) * 100);
aGrabar += Num;
if (decimal == 0){
   aGrabar += "00";
  }else{
    aGrabar += decimal;
    }
int8_t Digit0 = Num %10 ;
int8_t Digit1 = (Num % 100) / 10;
int8_t Digit2 = decimal %10 ;
int8_t Digit3 = (decimal % 100) / 10;
int8_t Digits[] = {Digit1,Digit0,Digit3,Digit2};
Display.display(Digits);                                    // MUESTRA EN DISPLAY
}

//-----------------------------------------------//
void escrituraSD(String texto) {                            // FUNCION ESCRITURA EN SD
  Archivo = SD.open("datos.txt", FILE_WRITE);
  if (Archivo) {
    Serial.print(F("Esto se va a grabar : "));
    Serial.println(texto);
    Archivo.println(texto);
    Archivo.close();
  }
  else {
    Serial.print(F("Esto NO SE grabo : "));
    Serial.println(texto);
    Serial.println(F("El archivo datos.txt no se abrió correctamente"));        // ERROR
  }
}

//-----------------------------------------------//
String dimeFecha(){                                               // FUNCION : OBTENER DIA Y HORA         ----Serial.print(dimeFecha());
  DateTime now = rtc.now(); //Obtener fecha y hora actual.
  sprintf( fecha, "%.2d%.2d%.4d%.2d%.2d%.2d", now.day(),now.month(), now.year(), now.hour(), now.minute(), now.second());
  return String( fecha );
}  

//------------------------------------------------//
void comprobacionYescritura(){
  if ((HumMax > h) and (h > HumMin)){
    //SUPER
                               // MUESTRA DIA Y HORA
    //FALTA GENERAR EL STRING PARA GRABAR
    Serial.println(F("HUMEDAD ACEPTABLE"));
    aGrabar += "0"; 
    escrituraSD(aGrabar);
    delay(2000);
    }else{
     if (HumMax < h){
      Serial.println(F("HUMEDAD POR ENCIMA DEL MAXIMO  "));
      } else {
      Serial.println(F("HUMEDAD POR DEBAJO DEL MINIMO "));}         // DETERMINA SI ES MAYOR O MENOR LA HUMEDAD
    //PROBLEMAS
    //FALTA GENERAR EL STRING PARA GRABAR Y GSM
    
    aGrabar += "1";                         // MUESTRA DIA Y HORA
    escrituraSD(aGrabar);
    delay(500);
    }
}

//------------------------------------------------//



