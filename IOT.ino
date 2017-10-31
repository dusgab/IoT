//--------------------LIBRERIAS ----------------//
#include "SoftwareSerial.h"
#include "Wire.h"
#include "RTClib.h"
#include "Time.h"
#include "DHT.h"
#include "TM1637.h"
#include <SD.h>
//--------------------DEFINICIONES--------------//
//-------VARIABLES------//
int retardo = 3000;                                        //RETARDO PARA MANDAR UN MSJ CADA 5 MINUTOS SIM900
int retardo2 = 3000;                                       //RETARDO PARA MANDAR UN MSJ CADA 5 MINUTOS SIM900
char incoming_char = 0;                                    //Variable que guarda los caracteres que envia el SIM900
String mensaje = "";                                       //USADA PARA CAPTURAR EL SERIAL PARA EL SIM900
String mensajecorto = "";                                  //PARA GUARDAR MSJ DE MAX Y MIN EN  SIM900
long minimo;                                               //OBTENER MINIMO EN INT SIM900
long maximo;                                               //OBTENER MAXIMO EN INT SIM 900
char msjCortoCH[11];                                       //AUXILIAR PARA GUARDAR MSJ  SIM900
//String HumAct;                                           // VARIABLE QUE GUARDA LA HUMEDAD ACTUAL PARA GUARDEN LA SD
String aGrabar;                                            // VARIABLE QUE SE GRABA EN LA SD
char fecha[12];                                            // VARIABLE USADA PARA OBTENER FECHA Y HORA PARA EL GUARDADO EN LA SD
File Archivo;                                              // VARIABLE ARCHIVO USADO CON LA SD
int HumMax = 60;                                           // VARIABLE DE HUMEDAD MAXIMA DEFINIDAS POR USUARIO ,PROGRAMA
int HumMin = 30;                                           // VARIABLE DE HUMEDAD MINIMA DEFINIDAS POR USUARIO ,PROGRAMA
float h;                                                   // VARIABLE HUMEDAD ,PROGRAMA

//---------PINES--------//
int sd = 10;                                               // SD                
TM1637 Display(3,4);                                       // CONEXION DE DISAPLAY                                
const int DHTPin = 5;                                      // DHT LECTOR DE HUMEDAD
int8_t Digits[4];                                          // PARA DISPLAY
//--------MODULOS-------//
SoftwareSerial SIM900(7, 8);                               // Configura el puerto serie para el SIM900
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
   
   Serial.begin(19200);                                     // INICIAMOS EL MONITOR SERIE PARA PRUEBAS
   inicializaSIM900();                                     // INICIALIZA SIM900
   /*pinMode(sd, OUTPUT);
   SD.begin(sd);
   Wire.begin();
   dht.begin();                                            // INICIA LECTOR DE HUMEDAD
   Display.set();
   Display.init();
   Display.point(POINT_ON); 
   if (!SD.begin(sd)){                                     // COMPRUEBA QUE SE CONECTO A LA SD
      Serial.println(F("Error de comunicación con la sd"));
   }else{
      Serial.println(F("Correcta comunicación con la sd"));
   }
  rtc.begin();
  if (!rtc.begin()) {                                     // COMPRUEBA QUE SE CONECTO EL MODULO RTC 
      Serial.println(F("Error en el modulo RTC"));
    }else {
      Serial.println(F("Conectado correctamente el modulo RTC"));
    }*/
   //rtc.adjust(DateTime(F(__DATE__),F(__TIME__)));      // LINEA COMENTADA SOLO SE OCUPA LA PRIMERA  VEZ PARA PONER EN HORA EL MODULO RTC
   Serial.println(F("INICIO MEDIDOR DE HUMEDAD:"));
   Serial.println(F("RANGO DE HUMEDAD :"));
   Serial.print(F("MAXIMA :"));
   Serial.println(HumMax);
   Serial.print(F("MINIMA :"));
   Serial.println(HumMin);
}

//----------------------------------------------//
//--------------------LOOP----------------------//
//----------------------------------------------//
void loop(){
  if (SIM900.available() > 0)
  {
    incoming_char = SIM900.read(); //Guardamos el carácter del GPRS
    //Serial.print(incoming_char); //Mostramos el carácter en el monitor serie
    mensaje = mensaje + incoming_char ; // Añadimos el carácter leído al mensaje
    delay(25);
  }else{Serial.println(SIM900.available());
    
    int bandera = mensaje.indexOf("MAX");
    if (bandera >= 0){
    leer_sms();}
  aGrabar = "";
  aGrabar += dimeFecha();
  aGrabar += HumMax;
  aGrabar += HumMin;  
  leerHumedad();                                           // GRABA EN H LA HUMEDAD
  
  comprobacionYescritura();                                // ACA ESTAN LOS DELAY
  mostrarHumedad(h);                                       // MUESTRA HUMEDAD POR DISPLAY
  }
}

//-----------------------------------------------// 
//--------------------FUNCIONES------------------//
//-----------------------------------------------//
void leerHumedad(){                                        // FUNCION : LECTURA DE HUMEDAD
   h = dht.readHumidity();                                 // LEO HUMEDAD
   if (isnan(h)) {
      Serial.println(F("ERROR al leer el lector DHT"));
      return;
   }else {
   Serial.print(F("Humedad Actual: "));
   Serial.print(h);
   Serial.print(F(" %\n"));
  }
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
  if ((HumMax > h) and (h > HumMin)){                               // COMPRUEBA SI LA HUMEDAD ESTA EN EL RANGO ACEPTABLE
    //SUPER
    Serial.println(F("HUMEDAD ACEPTABLE"));                         // SI LO ESTA
    aGrabar += "0";                                                 // TERMINA DE ARMAR EL STRING PARA GRABAR 
    escrituraSD(aGrabar);                                           // GRABA LA SD 
    if (retardo >= retardo2){retardo2 = retardo2 + 20000;}          // CONTADOR DE TIEMPO PARA REENVIAR MSJ
    delay(20000);
    }else{
      //PROBLEMAS
      aGrabar += "1";                                               // TERMINA DE ARMAR EL STRING PARA GRABAR 
      escrituraSD(aGrabar);                                         // GRABA EN LA SD
      if (HumMax < h){                                              // SI LA HUMEDAD ES MAYOR A MAXIMO ?
       Serial.println(F("HUMEDAD POR ENCIMA DEL MAXIMO  "));        // SI LO ES NOS MUESTRA
      if (retardo2 != 0){                                     // COMPRUEBA SI EL TIEMPO DE RETARDO DE REENVIO DE MSJ
        String msj="HUMEDAD POR ENCIMA DEL MAXIMO";
        mensaje_sms(msj);
        retardo2 = 0;
        }
      } else {                                                     // SI NO ES MAYOR , ES MENOR
      Serial.println(F("HUMEDAD POR DEBAJO DEL MINIMO "));         // NOS MUESTRA
      if (retardo <= retardo2){                                    // COMPRUEBA SI EL TIEMPO DE RETARDO DE REENVIO DE MSJ
       String msj="HUMEDAD POR DE BAJO DEL MINIMO";
       mensaje_sms(msj);
       retardo2 = 0;
        }
      }
      if (retardo >= retardo2){retardo2 = retardo2 + 5000;}        // RETARDO
    
    delay(5000);
    }
}
//-----------------------------------------------//
void inicializaSIM900()
{
  SIM900.begin(19200);                                    //Configura velocidad del puerto serie para el SIM900
  Serial.println(F("SIM900 OK"));
  delay (500);
  SIM900.println("AT + CPIN = \"1111\"");                 //Comando AT para introducir el PIN de la tarjeta
  delay(25000);                                           //Tiempo para que encuentre una RED
  Serial.println(F("PIN OK"));
  SIM900.print("AT+CMGF=1\r");                            //Configura el modo texto para enviar o recibir mensajes
  delay(1000);
  SIM900.print("AT+CNMI=2,2,0,0,0\r");                    // Saca el contenido del SMS por el puerto serie del GPRS
  delay(1000);
  Serial.println(F("INICIACION DE SIM 900 LISTA.."));
}
//-----------------------------------------------// 
void leer_sms(){

  if (mensaje.substring(mensaje.length()-11,mensaje.length()-8) == "MAX"){
      mensajecorto = mensaje.substring(mensaje.length()-11,mensaje.length()); 
  }else if (mensaje.substring(mensaje.length()-12,mensaje.length()-9) == "MAX"){
       mensajecorto = mensaje.substring(mensaje.length()-12,mensaje.length());
    }else {
      Serial.println(F("ERROR EN MENSAJE CORTO"));
      } 
 mensajecorto.toCharArray(msjCortoCH,11);
 //Serial.println(msjCortoCH);                             //SOLO CONTROL
 
 char minimo1[1]="";
 char maximo1[1]="";
 minimo1[0]=msjCortoCH[8];
 minimo1[1]=msjCortoCH[9];
 maximo1[0]=msjCortoCH[3];
 maximo1[1]=msjCortoCH[4];
 caInt(maximo1,minimo1);
 HumMax= maximo;
 HumMin= minimo;
 Serial.print(F("NUEVA HUMEDAD MAXIMA :"));
 Serial.println(HumMax);
 Serial.print(F("NUEVO HUMEDAD MINIMA :"));
 Serial.println(HumMin);
 String msj="CAMBIOS REALIZADOS CON EXITO";
 mensaje_sms(msj);
 mensaje= "";
 }
 //-----------------------------------------------// 
void caInt(const char *maxx,const char *minn)
{ 
  maximo=0;
  minimo=0;
   int tam  = 2;
    int digito;
    int i;
    for (i = tam ; i > 0; --i) {
        digito = (int) (maxx[i - 1] - 48);
        maximo += digito * pow(10.0, tam - i);  
    }
     int tam1  = 2;
    int digito1;
    int j;
    for (j = tam1 ; j > 0; --j) {
        digito1 = (int) (minn[j - 1] - 48);
        minimo += digito1 * pow(10.0, tam1 - j);
    }
}
//-----------------------------------------------// 
void mensaje_sms(String msj)
{
  Serial.println(F("Enviando SMS..."));
  SIM900.println("AT+CMGS=\"3794039779\""); //Numero al que vamos a enviar el mensaje
  delay(1000);
  SIM900.println(msj);// Texto del SMS
  delay(100);
  SIM900.println((char)26);//Comando de finalizacion ^Z
  delay(100);
  SIM900.println();
  delay(5000); // Esperamos un tiempo para que envíe el SMS
  Serial.println(F("SMS enviado"));
}

