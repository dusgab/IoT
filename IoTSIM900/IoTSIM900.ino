#include <SoftwareSerial.h>

SoftwareSerial SIM900(7, 8); // Configura el puerto serie para el SIM900
char incoming_char = 0; //Variable que guarda los caracteres que envia el SIM900
String mensaje = "";
String mensajecorto = "";
long minimo;
long maximo;
char msjCortoCH[11];

//----------------------------------------------//
//--------------------SETUP---------------------//
//----------------------------------------------//
void setup()
{
inicializaSIM900();
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
  }else{
    int bandera = mensaje.indexOf("MAX");
    if (bandera >= 0){
    leer_sms();}
  }
delay(100);
}

//-----------------------------------------------// 
//--------------------FUNCIONES------------------//
//-----------------------------------------------//
void inicializaSIM900()
{
  SIM900.begin(19200);                                    //Configura velocidad del puerto serie para el SIM900
  Serial.begin(19200);                                    //Configura velocidad del puerto serie del Arduino
  Serial.println("OK");
  delay (500);
  SIM900.println("AT + CPIN = \"1111\"");                 //Comando AT para introducir el PIN de la tarjeta
  delay(25000);                                           //Tiempo para que encuentre una RED
  Serial.println("PIN OK");
  SIM900.print("AT+CMGF=1\r");                            //Configura el modo texto para enviar o recibir mensajes
  delay(1000);
  SIM900.print("AT+CNMI=2,2,0,0,0\r");                    // Saca el contenido del SMS por el puerto serie del GPRS
  delay(1000);
  Serial.println("Done..");
}
//-----------------------------------------------// 
void leer_sms(){

  if (mensaje.substring(mensaje.length()-11,mensaje.length()-8) == "MAX"){
      mensajecorto = mensaje.substring(mensaje.length()-11,mensaje.length()); 
  }else if (mensaje.substring(mensaje.length()-12,mensaje.length()-9) == "MAX"){
       mensajecorto = mensaje.substring(mensaje.length()-12,mensaje.length());
    }else {
      Serial.println("ERROR EN MENSAJE CORTO");
      }
      
    /*Serial.print("mensajecorto : ");
    Serial.println(mensajecorto);
    Serial.print("longitud de mensajecorto : ");
    Serial.println(mensajecorto.length());                 //SOLO CONTROL*/
  
 mensajecorto.toCharArray(msjCortoCH,11);
 //Serial.println(msjCortoCH);                             //SOLO CONTROL
 
 char minimo1[1]="";
 char maximo1[1]="";
 minimo1[0]=msjCortoCH[8];
 minimo1[1]=msjCortoCH[9];
 maximo1[0]=msjCortoCH[3];
 maximo1[1]=msjCortoCH[4];
 caInt(maximo1,minimo1);
 /*Serial.print("MAX :");
 Serial.print(aux2[3]);
 Serial.println(aux2[4]); 
 Serial.print("MIN :");
 Serial.print(aux2[8]);
 Serial.println(aux2[9]);                   // SOLO CONTROL */
 Serial.print("NUEVO MAX  EN ENTERO:");
 Serial.println(maximo);
 Serial.print("NUEVO MIM  EN ENTERO:");
 Serial.println(minimo);
 mensaje_sms();
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
   /*Serial.print("maxx :");
      Serial.println(maxx);
      Serial.print("minn :");
       Serial.println(minn);
        Serial.println("");
       Serial.println("");                   // SOLO CONTROL */
    for (i = tam ; i > 0; --i) {
        digito = (int) (maxx[i - 1] - 48);
        maximo += digito * pow(10.0, tam - i);
        /* Serial.print("tam :");
        Serial.println(tam);
        Serial.print("digito :");
         Serial.println(digito);
         Serial.println("");
         Serial.println("");                   // SOLO CONTROL */
    }
     int tam1  = 2;
    int digito1;
    int j;
    for (j = tam1 ; j > 0; --j) {
        digito1 = (int) (minn[j - 1] - 48);
        minimo += digito1 * pow(10.0, tam1 - j);
       /* Serial.print("tam1 :");
        Serial.println(tam1);
         Serial.print("digito1 :");
         Serial.println(digito1);
          Serial.println("");
         Serial.println("");                   // SOLO CONTROL */
    }
}
//-----------------------------------------------// 
void mensaje_sms()
{
  Serial.println("Enviando SMS...");
  SIM900.println("AT+CMGS=\"3794039779\""); //Numero al que vamos a enviar el mensaje
  delay(1000);
  SIM900.println("mati avisame si te llega este msj.");// Texto del SMS
  delay(100);
  SIM900.println((char)26);//Comando de finalizacion ^Z
  delay(100);
  SIM900.println();
  delay(5000); // Esperamos un tiempo para que envíe el SMS
  Serial.println("SMS enviado");
}

