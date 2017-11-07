//--------------------LIBRERIAS ----------------//
#include <SoftwareSerial.h>
#include <DHT.h>
#include <TM1637.h>
#include <RTClib.h>
#include <SD.h>
//--------------------DEFINICIONES--------------//
//-------VARIABLES------//
File Archivo; 
int HumMax = 50;                                           // VARIABLE DE HUMEDAD MAXIMA DEFINIDAS POR USUARIO ,PROGRAMA
int HumMin = 10;                                           // VARIABLE DE HUMEDAD MINIMA DEFINIDAS POR USUARIO ,PROGRAMA
long retardo = 300000;                                     // VARIABLES USADAS PARA DAR TIEMPO ENTRE MENSAJES DE ALERTA (5 MIN), PROGRAMA
long retardo2 = 300000;
long minimo;                                               // VARIABLES USADAS PARA PROCEDIMIENTOS DE SEPARAR MAXIMO Y MINIMO DEL "mensaje" , PROGRAMA
long maximo;
char msjCortoCH[11];                                       // VARIABLE USADA PARA PASAR DE STRING A CHAR EL MENSAJE CORTO,PROGRAMA
float h;                                                   // VARIABLE DONDE ALMACENAMOS LA HUMEDAD,PROGRAMA
char fecha[12];                                            // VARIABLE USADA PARA OBTENER FECHA Y HORA PARA EL GUARDADO EN LA SD
String aGrabar;                                            // VARIABLE QUE SE GRABA EN LA SD
char incoming_char = 0;                                    // VARIABLE QUE ALMACENA LOS CARACTERES QUE RECIBE EL SIM900
String mensaje = "";                                       // VARIABLE QUE ALMACENA TODOS LOS CARACTERES QUE RECIBE EL SIM900
String mensajecorto = "";                                  // VARIABLE DONDE GUARDAMOS UNA PARTE DEL "mensaje" SIM900
int8_t Digits[4];                                          // PARA DISPLAY
//---------PINES--------//
int sd = 10;                                               // PIN DE SD
TM1637 Display(4, 3);                                      // PINES DE CONEXION DE DISPLAY (CLK,DIO)
const int DHTPin = 5;                                      // PIN DE SENSOR DE HUMEDAD
//--------MODULOS-------//
SoftwareSerial SIM900(7, 8);                               // PINES DEL MODULO SIM900
#define DHTTYPE DHT22                                      // DEFINIMOS QUE MODELO DE DHT ES 
DHT dht(DHTPin, DHTTYPE);                                  // DEFINIMOS DHT (HUMEDAD)
RTC_DS1307 rtc;                                            // DEFINIMOS RTC (HORA)

//------------------------------------------------------------------//
//---------------------FORMATO DE GRABACION DE SD-------------------//
//-------bien------------23102017135757603045700--------------------//
//-------error-----------23102017135757603045701--------------------//
//-------fecha(8)hora(6)humMax(2)humMin(2)humAct(4)bandera(1)-------//
//------------------------------------------------------------------//

//----------------------------------------------//
//--------------------SETUP---------------------//
//----------------------------------------------//
void setup()
{
  inicializarTODO();
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
void loop()
{
  if (SIM900.available() > 0)
  {
    incoming_char = SIM900.read(); //Guardamos el carácter del GPRS
    //Serial.print(incoming_char); //Mostramos el carácter en el monitor serie
    mensaje = mensaje + incoming_char ; // Añadimos el carácter leído al mensaje
  } else {
      int bandera = mensaje.indexOf("MAX");
      if (bandera >= 0) {
        leer_sms();
        bandera = -1;
      }
    aGrabar = "";
    aGrabar += dimeFecha();
    aGrabar += HumMax;
    aGrabar += HumMin;  
    leerHumedad();
    mostrarHumedad(h);                                       // MUESTRA HUMEDAD POR DISPLAY
    comprobacionYescritura();
  }
}

//-----------------------------------------------//
//--------------------FUNCIONES------------------//
//-----------------------------------------------//

void inicializarTODO()
{
  Serial.begin(19200);                                    //PUERTO SERIE PARA ARDUINO
  //-------INICIALIZAMOS SIM900--------//
  SIM900.begin(19200);                                    //PUERTO SERIE PARA SIM900
  Serial.println(F("OK"));
  SIM900.println("AT + CPIN = \"1111\"");                 //COMANDO AT PARA INTRODUCIR PIN DE TARJETA SIM
  delay(25000);                                           //TIEMPO DE ESPERA PARA BUSQUEDA DE RED
  Serial.println(F("PIN OK"));
  SIM900.print(F("AT+CMGF=1\r"));                         //COMANDO AT QUE CONFIGURA EL SIM900 PARA RECIVIR Y MANDAR MENSAJES
  delay(1000);
  SIM900.print("AT+CNMI=2,2,0,0,0\r");                    //COMANDO AT PARA EXTRAER LOS MENSAJES DEL SIN900
  delay(1000);
  Serial.println(F("Done.."));
  //-------INICIALIZAMOS SD--------//
  //SD.begin(sd);
  if (!SD.begin(sd)){                                     // COMPRUEBA QUE SE CONECTO A LA SD
      Serial.println(F("Error de comunicación con la sd"));
   }else{
      Serial.println(F("Correcta comunicación con la sd"));}
  //-------INICIALIZAMOS RTC--------//
  rtc.begin();
  rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));     // LINEA COMENTADA SOLO SE OCUPA LA PRIMERA  VEZ PARA PONER EN HORA EL MODULO RTC
  if (!rtc.begin()) {                                     // COMPRUEBA QUE SE CONECTO EL MODULO RTC
    Serial.println(F("Error en el modulo RTC"));
  } else {
    Serial.println(F("Conectado correctamente el modulo RTC"));}
  //-------INICIALIZAMOS DISPLAY--------//
  Display.set();
  Display.init();
  Display.point(POINT_ON);
}

//-----------------------------------------------//

String dimeFecha()                                               // FUNCION : OBTENER DIA Y HORA         ----Serial.print(dimeFecha());
{
  DateTime now = rtc.now(); //Obtener fecha y hora actual.
  sprintf( fecha, "%.2d%.2d%.4d%.2d%.2d%.2d", now.day(), now.month(), now.year(), now.hour(), now.minute(), now.second());
  return String( fecha );
}

//-----------------------------------------------//

void mostrarHumedad( float Num2)                          // FUNCION : MUESTRA EN DISPLAY LA HUMEDAD
{
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
  int8_t Digit0 = Num % 10 ;
  int8_t Digit1 = (Num % 100) / 10;
  int8_t Digit2 = decimal % 10 ;
  int8_t Digit3 = (decimal % 100) / 10;
  int8_t Digits[] = {Digit1, Digit0, Digit3, Digit2};
  Display.display(Digits);                                    // MUESTRA EN DISPLAY
}

//-----------------------------------------------//

void comprobacionYescritura() 
{
  if ((HumMax > h) and (h > HumMin)){                              // COMPRUEBA SI LA HUMEDAD ESTA EN EL RANGO ACEPTABLE
    //------FUNCIONAMIENTO CORRECTO-----//
    Serial.println(F("HUMEDAD ACEPTABLE"));                         // SI LO ESTA
    aGrabar += "0";                                                 // TERMINA DE ARMAR EL STRING PARA GRABAR
    escrituraSD(aGrabar);                                           // GRABA LA SD
      if (retardo >= retardo2) {
        retardo2 = retardo2 + 20000;                                  // CONTADOR DE TIEMPO PARA REENVIAR MSJ
      }  
    delay(5000);
  } else {
    //------FUNCIONAMIENTO CORRECTO-----//
    aGrabar += "1";                                                 // TERMINA DE ARMAR EL STRING PARA GRABAR
    escrituraSD(aGrabar);                                           // GRABA EN LA SD
      if (HumMax < h) {                                               // SI LA HUMEDAD ES MAYOR A MAXIMO ?
        Serial.println(F("HUMEDAD POR ENCIMA DEL MAXIMO  "));         // SI LO ES NOS MUESTRA
        if (retardo2 >= retardo) {
          String msj = "HUMEDAD POR ENCIMA DEL MAxIMO";
          mensaje_sms(msj);
          retardo2 = 0;
        }
      } else {                                                     // SI NO ES MAYOR , ES MENOR
        Serial.println(F("HUMEDAD POR DEBAJO DEL MINIMO "));
        if (retardo2 >= retardo) {
          String msj = "HUMEDAD POR POR DEBAJO DEL MINIMO";
          mensaje_sms(msj);
          retardo2 = 0;
        }
      }
    if (retardo >= retardo2) {
      retardo2 = retardo2 + 5000;
    }
    delay(5000);
  }
}

//-----------------------------------------------//

void leerHumedad()                                        // FUNCION : LECTURA DE HUMEDAD
{  
  h = dht.readHumidity();                                  // LEO HUMEDAD
  if (isnan(h)) {
    Serial.println(F("ERROR al leer el lector DHT"));
  } else {
      Serial.print(F("Humedad Actual: "));
      Serial.print(h);
      Serial.print(F(" %\n"));
  }
}

//-----------------------------------------------//

void escrituraSD(String aGrabar)                             // FUNCION ESCRITURA EN SD
{
Archivo = SD.open("datos.txt", FILE_WRITE);
  if (Archivo) { 
    Archivo.println(aGrabar);
    Archivo.close();
  } else {
      Serial.println(F("ERROR AL GRABAR"));        // ERROR
    }
}

//-----------------------------------------------//

void leer_sms()                                               // FUNCION LEE MSJ
{
  if (mensaje.substring(mensaje.length() - 11, mensaje.length() - 8) == "MAX") {
    mensajecorto = mensaje.substring(mensaje.length() - 11, mensaje.length());
  } else if (mensaje.substring(mensaje.length() - 12, mensaje.length() - 9) == "MAX") {
      mensajecorto = mensaje.substring(mensaje.length() - 12, mensaje.length());
    } else {
      Serial.println(F("ERROR EN MENSAJE CORTO"));
      }
  mensajecorto.toCharArray(msjCortoCH, 11);
  char minimo1[1] = "";
  char maximo1[1] = "";
  minimo1[0] = msjCortoCH[8];
  minimo1[1] = msjCortoCH[9];
  maximo1[0] = msjCortoCH[3];
  maximo1[1] = msjCortoCH[4];
  caInt(maximo1, minimo1);
  HumMax = maximo;
  HumMin = minimo;
  Serial.print(F("NUEVO MAX  EN ENTERO:"));
  Serial.println(HumMax);
  Serial.print(F("NUEVO MIM  EN ENTERO:"));
  Serial.println(HumMin);
  String msj = "RANGO DE HUMEDAD CAMBIADO CON EXITO";
  mensaje_sms(msj);
  mensaje = "";
}
//-----------------------------------------------//
void caInt(const char *maxx, const char *minn)
{
  maximo = 0;
  minimo = 0;
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
  SIM900.println("AT+CMGS=\"3794000826\"");                            //COMANDO AT DEFINE EL NUMERO A MANDAR EL MSJ
  delay(1000);
  SIM900.println(msj);                                                 //TEXTO DEL SMS
  SIM900.println((char)26);                                            //COMANDO DE FINALIZACION ^Z
  delay(100);
  SIM900.println();
  delay(5000);                                                         //TIEMPO DE ESPERA PARA MANDAR EL SMS
  Serial.println(F("SMS enviado"));
}

