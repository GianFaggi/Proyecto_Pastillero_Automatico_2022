/*  ITES
 *  Cursada 2022
 *
 *  Código: Movimiento Motor Paso a Paso.
 *  
 *  Autores: Faggi Gianfranco, Funes Diego, Clarembaux Juan.
 *  Fecha: 04/10/2022
 *  
 *  Código: Paso a Paso Pastillero
 *  
 *  Codigo de funcionamiento dentro del dispensador. 
 *  El motor gira segun el valor que se ingrese por consola (en sentido horario y anti horario), 
 *  el buzzer debe prender cuando este realiza un movimiento hacia la derecha hasta que el usuario tome la pastilla
 *  un led (verde) debe estar encendido cuando el dispositivo esta sin movimiento, mientras un led (rojo) debe encenderse cuando la pastilla este servida.
 */

//PINES PASO A PASO
#define IN1  32
#define IN2  33
#define IN3  25
#define IN4  26

//INTERVALO DE ESPERA ENTRE PASO Y PASO
#define INTERVALO_ESPERA_MOTOR               2

//SECUENCIA DE PASOS (par máximo)
int paso [4][4] =
{
  {1, 0, 0, 0},
  {0, 1, 0, 0},
  {0, 0, 1, 0},
  {0, 0, 0, 1},
};

//INFORMACION PASO A PASO
int     Pasos_a_realizar_ida =               0;                                  //Cantidad de pasos a realizar por el motor paso a paso en sentido horario
int     Pasos_recibidos =                    0;                                  //Variable ingresada por el servidor, la cual manda la cantidad de vueltas a realizar por el motor
int     Pasos_a_realizar_vuelta =            0;                                  //Cantidad de pasos a realizar por el motor paso a paso en sentido anti-horario


//LED TEST
#define INTERVALO                            500                                 //Tiempo en ms
#define LED_TEST                             13                                  // LED USADO EN EL BLINK DE HABILITACION
#define CONFIG_LED_TEST                      pinMode(LED_TEST, OUTPUT)          
#define ACTUALIZAR_LED_TEST(X)               digitalWrite(LED_TEST, X)

//PINES BUZZER
#define PIN_BUZZER                           27
byte salida                                  = 230;
byte entrada                                 = 0;

//PINES BOTON
#define PIN_BOTON                            14
int EstaApretado = HIGH;

//LED ROJO
#define PIN_LED_ROJO                         19                                 // LED ROJO
#define CONFIG_LED_ROJO                      pinMode(PIN_LED_ROJO, OUTPUT)
#define ACTUALIZAR_LED_ROJO(X)               digitalWrite(PIN_LED_ROJO, X)
//LED VERDE
#define PIN_LED_VERDE                        18                                 // LED VERDE
#define CONFIG_LED_VERDE                     pinMode(PIN_LED_VERDE, OUTPUT)
#define ACTUALIZAR_LED_VERDE(X)              digitalWrite(PIN_LED_VERDE, X)

//----------------------------------------------------------------------------------------------------------------------------------------------------------------
 
void setup()
{
  Serial.begin(9600);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  CONFIG_LED_TEST;
  CONFIG_LED_VERDE;  
  CONFIG_LED_ROJO;  
  pinMode(PIN_BOTON, INPUT_PULLUP);



}
//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void loop()
  {
  MoverMotorDelante();
  MoverMotorAtras();
  RecibirPasosMonitorSerial();
  LedTest();  
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------
void RecibirPasosMonitorSerial()                                //RECIBE EL DATO ENVIADO POR EL MONITOR SERIAL
  {
  if(!Serial.available()) return;

  Pasos_recibidos = Serial.parseInt();

  Pasos_a_realizar_ida= Pasos_recibidos;
  Pasos_a_realizar_vuelta = Pasos_recibidos;
 
  while(Serial.available()) Serial.read();                       //VACIA EL BUFFER DE RECEPCION SERIE
  }

//----------------------------------------------------------------------------------------------------------------------------------------------------------------
void MoverMotorDelante()                                        //MOVIMIENTO DEL MOTOR EN SENTIDO HORARIO
{
  static unsigned long antMillis = 0;

  if(!Pasos_a_realizar_ida) return;

  if(millis() - antMillis < INTERVALO_ESPERA_MOTOR) return;     //Si no se cumple tal condicion, retorna
  antMillis = millis();

  AvanzarUnPaso();
  ACTUALIZAR_LED_ROJO(HIGH);
  ACTUALIZAR_LED_VERDE (LOW);
 
  Pasos_a_realizar_ida--;

  }

  //----------------------------------------------------------------------------------------------------------------------------------------------------------------
       
void MoverMotorAtras()                                          //MOVIMIENTO DEL MOTOR
{
  static unsigned long antMillis = 0;    
    RecibirEstadoBoton();

  if(Pasos_a_realizar_ida != 0 ) return;

  if(EstaApretado == HIGH) return;
 
  if(!Pasos_a_realizar_vuelta)return;

 
  if(millis() - antMillis < INTERVALO_ESPERA_MOTOR) return;     //Si no se cumple tal condicion, retorna
  antMillis = millis();
 
  RetrocederUnPaso();
  ACTUALIZAR_LED_ROJO(LOW);
  ACTUALIZAR_LED_VERDE(HIGH);

  Pasos_a_realizar_vuelta--;
  }
 

//----------------------------------------------------------------------------------------------------------------------------------------------------------------
 
 void AvanzarUnPaso()                                             //REALIZAR UN PASO EN SENTIDO HORARIO
  {  
     static unsigned char i=0;
    digitalWrite(IN1, paso[i][0]);
    digitalWrite(IN2, paso[i][1]);
    digitalWrite(IN3, paso[i][2]);
    digitalWrite(IN4, paso[i][3]);
    i++;
    if(i==4) i=0;
    EncenderBuzzer();
  }

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

  void RetrocederUnPaso()                                         //REALIZAR UN PASO EN SENTIDO ANTI-HORARIO
  {
    static unsigned char i=0;
    digitalWrite(IN1, paso[i][3]);
    digitalWrite(IN2, paso[i][2]);
    digitalWrite(IN3, paso[i][1]);
    digitalWrite(IN4, paso[i][0]);
    i++;
    if(i==4) i=0;
    ApagarBuzzer();
  }

//----------------------------------------------------------------------------------------------------------------------------------------------------------------
void LedTest()                                                    //BLINK QUE CAMBIA CON TIEMPO
  {
  static bool ledState=0;
  static unsigned long antMillis = 0;        

  //Si no se cumple tal condicion, retorna
  if(millis() - antMillis < INTERVALO) return;
  antMillis = millis();

  ledState = !ledState;
   
  ACTUALIZAR_LED_TEST(ledState);
  }

//----------------------------------------------------------------------------------------------------------------------------------------------------------------
void EncenderBuzzer()                                            //ENCIENDE EL BUZZER PARA QUE EMITA UN SONIDO
{
  analogWrite(PIN_BUZZER, salida);
 }

//----------------------------------------------------------------------------------------------------------------------------------------------------------------
void ApagarBuzzer()                                             //APAGA EL BUZZER PARA QUE DEJE DE EMITIR SONIDOS
{
  analogWrite(PIN_BUZZER, entrada);
    }

//----------------------------------------------------------------------------------------------------------------------------------------------------------------
void RecibirEstadoBoton() {                                     //RECIBE EL CAMBIO DE ESTADO DEL PULSADOR
  EstaApretado = digitalRead(PIN_BOTON);
  if(EstaApretado != HIGH) return;
  }
