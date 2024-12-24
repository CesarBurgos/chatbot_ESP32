#include "CTBot.h"
#include "Utilities.h"
#include "WiFi.h"

CTBot chatBot;
CTBotInlineKeyboard opciones_bot;

// Nombre ssid de la red a la que deseamos conectarnos
const char* ssid = "";
// Contraseña de la red
const char* password = "";
// Token del Chatbot con el cual vamos a comunicarnos
const String token = "";
// Pin del led incorporando en el ESP32
const int led_incorporado = 14;

//Bandera de intentos de conexión con la red
int conectd = 0;

//Bandera de conexión
int conectado = 0;

// Parpadeo de led
int parpadeo = 0;

void setup() {
  // Indicando frecuencia de comunicación Serial del ESP32-Para visualizar mensajes en el Monitor Serial
  Serial.begin(115200);

  // Configurando led incorporado del ESP32 como SALIDA
  pinMode(led_incorporado, OUTPUT);
  digitalWrite(led_incorporado,LOW);

  // Mensaje
  Serial.println("\n\n\nIniciando ESP32...");
  //Estableciendo conexiones
  conectado = conexion_red();
}

void loop() {
  // Si existe una conexión
  if (WiFi.status() == WL_CONNECTED and conectado == 1) {

    //Almacenando mensaje recibido desde telegram
    TBMessage msg_telegram;

    //Si hay un mensaje enviado de Telegram
    if (chatBot.getNewMessage(msg_telegram)) {
      Serial.println("\n\n --- Mensaje recibido desde Telegram ---");
      Serial.println(" > Nombre: "+String(msg_telegram.sender.firstName)+"  "+String(msg_telegram.sender.lastName));
      Serial.println(" > Usuario: "+String(msg_telegram.sender.username));
      //Serial.println(" > ID: "+int64ToAscii(msg_telegram.sender.id));
        
      Serial.print("\nTipo de mensaje: ");
      Serial.println(msg_telegram.messageType);

      Serial.print("Mensaje: ");
      Serial.println(msg_telegram.text);

      Serial.print("Query: ");
      Serial.println(msg_telegram.callbackQueryData);

      Serial.println("\n");

      // Verificando SI un usuario dentro del grupo envio un MENSAJE...
      if (msg_telegram.messageType == CTBotMessageText) { // Tipo 1
        if (msg_telegram.text.equalsIgnoreCase("opciones") or msg_telegram.text.equalsIgnoreCase("/opciones")) {
          chatBot.sendMessage(msg_telegram.group.id, "Opciones:", opciones_bot);
        }else if (msg_telegram.text.equalsIgnoreCase("/opc1")) {
          // Ha seleccionado el segundo comando
          Serial.println(" Ha sido seleccionado desde el menú de Telegram el comando 'opc1' ");

          chatBot.sendMessage(msg_telegram.group.id, " OPCIÓN 1: Envio de un mensaje");
          chatBot.sendMessage(msg_telegram.group.id, " \n\n ¡Hola! que tal te saluda un ChatBot...");
        }else if (msg_telegram.text.equalsIgnoreCase("/opc2")) {
          // Ha seleccionado el tercer comando
          Serial.println(" Ha sido seleccionado desde el menú de Telegram el comando 'opc2' ");
          chatBot.sendMessage(msg_telegram.group.id, " OPCIÓN 2: Parpadeo del led incorporado en el ESP32");

          parpadeo = 0;
          while(parpadeo < 10){
            digitalWrite(led_incorporado,HIGH);
            delay(250);
            digitalWrite(led_incorporado,LOW);
            delay(250);
            ++parpadeo;
          }
          digitalWrite(led_incorporado,LOW);
        }else if (msg_telegram.text.equalsIgnoreCase("/opc3")) {
          // Ha seleccionado el tercer comando
          Serial.println(" Ha sido seleccionado desde el menú de Telegram el comando 'opc3' ");
          chatBot.sendMessage(msg_telegram.group.id, " OPCIÓN 3: Llamada a una función");
          funcion_opc3();
        }else if (msg_telegram.text.equalsIgnoreCase("/start")) {
          // Ha seleccionado el tercer comando
          Serial.println(" Ha sido seleccionado desde el menú de Telegram el comando 'start' ");
          chatBot.sendMessage(msg_telegram.group.id, " OPCIÓN INICIO: Ha inicializado el Chatbot");
        }else{
          // Ingreso un comando incorrecto o desconocido
          chatBot.sendMessage(msg_telegram.group.id, " Oh vaya... al parecer no reconozco la acción que deseas que realice... Intente nuevamente...");
        }
      }else if (msg_telegram.messageType == CTBotMessageQuery) { // Tipo 2
        // Verificando SI un usuario dentro del grupo SELECCIONO UNA OPCIÓN (COMANDO)...
        if (msg_telegram.callbackQueryData.equals("opciones")) {
          // Ha seleccionado el primer comando
          chatBot.sendMessage(msg_telegram.group.id, " Hola, para ejecutar las acciones que puedo realizar puede consultarlas escribiendo el caracter '/'");
          chatBot.sendMessage(msg_telegram.group.id, " \n Oh si lo desea envie alguno de los sig. comando y vea las acciones que puedo realizar");
          chatBot.sendMessage(msg_telegram.group.id, " 1) '/' + opc1:");
          chatBot.sendMessage(msg_telegram.group.id, " 2) '/' + opc2:");
          chatBot.sendMessage(msg_telegram.group.id, " 3) '/' + opc3:");
        }else if (msg_telegram.callbackQueryData.equals("opc1")) {
          // Ha seleccionado el segundo comando
          Serial.println(" Ha sido seleccionado el comando 'opc1' ");

          chatBot.sendMessage(msg_telegram.group.id, " OPCIÓN 1: Envio de un mensaje");
          chatBot.sendMessage(msg_telegram.group.id, " \n\n ¡Hola! que tal te saluda un ChatBot...");

        } else if (msg_telegram.callbackQueryData.equals("opc2")) {
          // Ha seleccionado el tercer comando
          Serial.println(" Ha sido seleccionado el comando 'opc2' ");
          chatBot.sendMessage(msg_telegram.group.id, " OPCIÓN 2: Parpadeo del led incorporado en el ESP32");

          parpadeo = 0;
          while(parpadeo < 10){
            digitalWrite(led_incorporado,HIGH);
            delay(250);
            digitalWrite(led_incorporado,LOW);
            delay(250);
            parpadeo++;
          }
          digitalWrite(led_incorporado,LOW);
        } else if (msg_telegram.callbackQueryData.equals("opc3")) {
          
          // Ha seleccionado el tercer comando
          Serial.println(" Ha sido seleccionado el comando 'opc3' ");
          chatBot.sendMessage(msg_telegram.group.id, " OPCIÓN 3: Llamada a una función");
          funcion_opc3();
        } else {
          // Ingreso un comando incorrecto o desconocido
          chatBot.sendMessage(msg_telegram.group.id, " Oh vaya... al parecer ingresaste un comando desconocido... Intente nuevamente...");
        }
      }
    }
  } else {
    //Estableciendo nuevamente conexiones
    conectado = conexion_red();
  }

  delay(500);
}


// Función para establecer conexiones de red y con el chatbot
int conexion_red() {
  // Ingresando los datos para conectarnos a la red...
  WiFi.begin(ssid, password);
  //Serial.println("\n Estableciendo conexión con la red: " + String(ssid));
  // Bucle para la espera de la conexión con la red
  // 250 * 60 = 15,000 ---> 15seg
  while ((WiFi.status() != WL_CONNECTED) and (conectd < 60)) {
    delay(250);
    ++conectd;
  }

  Serial.print("\n > Conexión establecida con (" + String(ssid) + ") con: ");
  if (conectd < 60) {
    Serial.println(" -- Ok");
    chatBot.setTelegramToken(token);
    Serial.print("\n > Conexión con ChatBot (Telegram): ");
    if (chatBot.testConnection()) {
      Serial.println(" -- Ok");
      opciones_bot.addButton("Ayuda", "opciones", CTBotKeyboardButtonQuery);
      opciones_bot.addButton(" 1) Mensaje", "opc1", CTBotKeyboardButtonQuery);
      opciones_bot.addRow();
      opciones_bot.addButton(" 2) Parpadeo Led", "opc2", CTBotKeyboardButtonQuery);
      opciones_bot.addButton(" 3) Llamada a función", "opc3", CTBotKeyboardButtonQuery);
      opciones_bot.addRow();

      // Conectado
      return 1;
    } else {
      Serial.println(" -- No conectado.");
      // No Conectado
      return 0;
    }
  } else {
    Serial.println(" -- No conectado.");
    // No Conectado
    return 0;
  }
}

// Función que será llamada al indicar el comando 3
void funcion_opc3(){
  parpadeo = 0;
  
  while(parpadeo < 10){
    Serial.print("*");
    digitalWrite(led_incorporado,HIGH);
    delay(250);
    digitalWrite(led_incorporado,LOW);
    delay(250);
    parpadeo++;
  }
  digitalWrite(led_incorporado,LOW);
}
