/**************************************************
    AUTHOR: Erivelto Silva.
    FOR: Barros.
    ISPB.
    CREATED AT: 28-05-2024.
*************************************************/

////////////////// Labraries Used  ////////////////
#include <WiFi.h>                             /////
#include "SPIFFS.h"                           /////
#include <AsyncTCP.h>                         /////
#include <ArduinoJson.h>                      /////
#include <ESPAsyncWebServer.h>                /////
#include <IOXhop_FirebaseESP32.h>             /////importa biblioteca para esp32 se comunicar com firebase
///////////////////////////////////////////////////

////////////////  PIN CONFIGURATIONS //////////////
#define LED 2                                                                /////
#define RXD2 16                                                              // RX da serial do ESP32      /////
#define TXD2 17                                                              // TX da serial do ESP32      /////
#define FIREBASE_HOST "https://parque-control-default-rtdb.firebaseio.com/"  // substitua "Link_do_seu_banco_de_dados" pelo link do seu banco de dados
#define FIREBASE_AUTH "AIzaSyDljBC-KlS1MTJTXcNzbxsK-ROP30dnqsU"              // substitua "Senha_do_seu_banco_de_dados" pela senha do seu banco de dados
#define TIME_RESERVATION 120000
///////////////////////////////////////////////////

/////////////  NETWORK CONFIGURATIONS /////////////
#define SSID "PARK"           /////
#define PASSWORD "123456789"  /////
///////////////////////////////////////////////////

////////// VARIABLES USED IN THE PROJECT //////////
bool flagAgend1 = false;                  /////
bool flagAgend2 = false;                  /////
String dataStored = "D*0*0*0*0*0*0*";     /////
String userReservation1 = "";             /////
String userReservation2 = "";             /////
unsigned long int timerReservation1 = 0;  /////
unsigned long int timerReservation2 = 0;  /////
unsigned long int timeDelay = 0;          /////
///////////////////////////////////////////////////

///////////////  OBJECTS DEFINITIONS  /////////////
AsyncWebServer server(80);  /////
HardwareSerial arduino(2);  /////
///////////////////////////////////////////////////

////////////// FUNCTION DEFINITIONS  //////////////
void wifiConfig();                           /////
void initConfig();                           /////
bool initMyFS();                             /////
bool isUser(String email, String password);  /////
void serverHandlers();                       /////
void saveUserFirebase(String email, String password);
///////////////////////////////////////////////////

////////////////////////////////////////////////////
void notFound(AsyncWebServerRequest *request) {
  request->send(404, "text/plain", "Not found");
}

////////////////////////////////////////////////////////////////////////////////
void setup() {
  initConfig();
  wifiConfig();

  if (initMyFS())
    Serial.println(" ## PARTIÇÃO SPIFFS MONTADA! ##");
  else
    Serial.println(" ## ERRO MONTANDO A PARTIÇÃO SPIFFS ##");

  // inicia comunicação com firebase definido anteriormente
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  serverHandlers();
  Serial.println(" ##-------SISTEMA DE DOMÓTICA!--------##");
}

///////////////////////////////////////////////////////////////////////////////
void loop() {
  verifyTimeReservation();
  if (millis() - timeDelay > 800) {
    timeDelay = millis();
    askData();
    String dataReceived = receiveData();
    if (dataReceived != "") 
    {
      dataStored = dataReceived;

      Serial.println("--> RECEBIDO DO ARDUINO:" + dataStored);
      unsigned long time1 = 0;
      unsigned long time2 = 0;
      if (flagAgend1)
        time1 = (TIME_RESERVATION - (millis() - timerReservation1)) / 1000;

      if (flagAgend2)
        time2 = (TIME_RESERVATION - (millis() - timerReservation2)) / 1000;

      dataStored.replace("\n", "");
      dataStored.replace("\r", "");
      dataStored += (flagAgend1) ? "1*" : "0*";
      dataStored += (flagAgend2) ? "1*" : "0*";
      dataStored += String(time1) + "*";
      dataStored += String(time2) + "*";
    }
    if (flagAgend1)
      Serial.println("ESPACO 1 AGENDADO POR:" + userReservation1);
    if (flagAgend2)
      Serial.println("ESPACO 2 AGENDADO POR:" + userReservation2);
    digitalWrite(LED, !digitalRead(LED));
  }
  delay(20);
}

/////////////////////////////////////////////////////////
void initConfig() {
  pinMode(LED, OUTPUT);
  Serial.begin(9600);
  delay(500);
  arduino.begin(9600, SERIAL_8N1, RXD2, TXD2);
  delay(500);
  Serial.println("CONFIGURAÇÕES INICIAS SETADAS!");
  delay(5000);
}

/////////////////////////////////////////////////////////
void wifiConfig() {
  if (WiFi.status() == WL_CONNECTED)
    return;
  Serial.println();
  Serial.print("CONECTANDO A WIFI:");
  Serial.println(SSID);
  Serial.print("PROCURANDO");
  WiFi.mode(WIFI_STA);
  WiFi.begin(SSID, PASSWORD);
  for (int i = 0; WiFi.status() != WL_CONNECTED; i++) {
    digitalWrite(LED, !digitalRead(LED));
    delay(150);
    Serial.print(".");
    if (i == 100)
      ESP.restart();
  }
  Serial.print("\nCONECTADO AO WIFI NO IP:");
  Serial.println(WiFi.localIP());
}

//////////////////////////////////////////////////////////////////////////////
void askData() {
  static unsigned long int timeSend = 0;
  static bool myFlag = false;
  arduino.println('R');
  if ((millis() - timeSend > 5000)) {
    timeSend = millis();
    myFlag = !myFlag;
    if (myFlag)
      arduino.println((flagAgend1) ? 'Y' : 'y');
    else
      arduino.println((flagAgend2) ? 'Z' : 'z');
  }
}

//////////////////////////////////////////////////////////////////////////////
String receiveData() {
  if (arduino.available()) {
    String txt = "";
    while (arduino.available())
      txt = arduino.readStringUntil('\n');
    return txt;
  }
  return "";
}

////////////////////////////////////////////////////////////////////////
bool initMyFS() {
  return (SPIFFS.begin(true));
}

void verifyTimeReservation() {
  if ((millis() - timerReservation1) > 120000 && flagAgend1) {
    flagAgend1 = false;
    userReservation1 = "";
    arduino.println("y");
    Serial.println("################################TEMPO DA RESERVA 1 EXPIROU ########################################");
  }

  if ((millis() - timerReservation2) > 120000 && flagAgend2) {
    flagAgend2 = false;
    userReservation2 = "";
    arduino.println("z");
    Serial.println("################################TEMPO DA RESERVA 2 EXPIROU ########################################");
  }
}

bool isUser(String username, String password) {
  String pass = Firebase.getString("/users/" + username);
  return (pass.equals(password));
}

void saveUserFirebase(String username, String password) {
  Firebase.setString("/users/" + username, password);
}

////////////////////////////////////////////////////////////////////////
void serverHandlers() {
  // Route to load bootstrap.min.css file
  server.on("/bootstrap.min.css", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/bootstrap.min.css", "text/css");
  });
  // Route to load bootstrap.bundle.min.js file
  server.on("/bootstrap.bundle.min.js", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/bootstrap.bundle.min.js", "text/javascript");
  });
  // Route to load image-park.jpeg file
  server.on("/image-park.jpeg", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/image-park.jpeg", "image/jpeg");
  });

  // Route to load vaga-livre.png file
  server.on("/vaga-livre.png", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/vaga-livre.png", "image/png");
  });
  // Route to load im.png file
  server.on("/im.jpg", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/im.jpg", "image/jpg");
  });


  // Route to load vaga-ocupada.png file
  server.on("/vaga-ocupada.png", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/vaga-ocupada.png", "image/png");
  });
  // Route to load index.css file
  server.on("/index.css", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/index.css", "text/css");
  });
  // Route to load index.js file
  server.on("/index.js", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/index.js", "text/javascript");
  });
  // Route to load index.css file
  server.on("/login.css", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/login.css", "text/css");
  });
  // Route to load login.js file
  server.on("/login.js", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/login.js", "text/javascript");
  });

  /*--------------------------ENDPOINS---------------------------*/
  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    Serial.println("-------->redirecionando para login.html");
    request->redirect("/login");
  });

  // Route for login / web page
  server.on("/login", HTTP_GET, [](AsyncWebServerRequest *request) {
    Serial.println("--------> login.html");
    request->send(SPIFFS, "/login.html");
  });

  // POST request para login
  server.on("/getin", HTTP_POST, [](AsyncWebServerRequest *request) {
    Serial.println("------> Req de /getin do login.html");
    if (!(request->hasParam("username", true) && request->hasParam("password", true))) {
      Serial.println("---> Erro... Parametros de Login Incompletos!\nRedirecionando para ----> login.html");
      request->redirect("/login");
    } else {
      String username = request->getParam("username", true)->value();
      String password = request->getParam("password", true)->value();
      if (username.equals("admin") && password.equals("otlevire")) {
        Serial.println("--->Username e Senha de Admin Válidos!\nRedirecionando para ----> register.html");
        request->redirect("/register");
      } else {
        if (!isUser(username, password)) {
          Serial.println("--->Username e Senha Inválidos!\nRedirecionando para ---> login.html");
          request->redirect("/login");
        } else {
          Serial.println("--->Username e Senha Válidos!\nRedirecionando para ---> index.html");
          request->redirect("/home");
        }
      }
    }
  });

  // POST request para Cadastro de Usuario
  server.on("/make-register", HTTP_POST, [](AsyncWebServerRequest *request) {
    Serial.println("------> Req de /register do register.html");
    if (!(request->hasParam("username", true) && request->hasParam("passwordUser", true) && request->hasParam("passwordAdmin", true))) {
      Serial.println("---> Erro... Parametros de Login Incompletos!\nRedirecionando para ----> login.html");
      request->redirect("/login");
    } else {
      String username = request->getParam("username", true)->value();
      String passwordUser = request->getParam("passwordUser", true)->value();
      String passwordAdmin = request->getParam("passwordAdmin", true)->value();

      if (passwordAdmin.equals("otlevire")) {
        Serial.println("--->Admin Válido! Salvando dados do novo usuario no Firebase...");
        Firebase.setString("/users/" + username, passwordUser);
        //saveUserFirebase(username, passwordUser);
        Serial.println("Dados do Usuario Salvos!");
        request->redirect("/login");
      } else {
        Serial.println("--->Admin Inválido!Redirecionando para register.html");
        request->redirect("/register");
      }
    }
  });

  // Route for root / web page
  server.on("/register", HTTP_GET, [](AsyncWebServerRequest *request) {
    Serial.println("--------> register.html");
    request->send(SPIFFS, "/register.html");
  });
  // Route for root / web page
  server.on("/home", HTTP_GET, [](AsyncWebServerRequest *request) {
    Serial.println("--------> Index.html");
    request->send(SPIFFS, "/index.html");
  });

  server.on("/dados", HTTP_GET, [](AsyncWebServerRequest *request) {
    //dataStored = "D*1*1*1*0*0*0*0*0*";

    Serial.println("--------> Dados:" + dataStored);
    String resp = "{\"status\":\"success\", \"data\":\"" + dataStored + "\"}";
    request->send(200, "application/json", resp);
  });

  server.on("/openReserve1", HTTP_GET, [](AsyncWebServerRequest *request) {
    Serial.println("--------> openReserve1:");
    if (flagAgend1) {
      String username = request->getParam("username")->value();
      if (username.equals(userReservation1)) {
        Serial.println("======= ABRINDO=============================");
        arduino.println("A");
        request->send(200, "application/json", "{\"status\":\"success\"}");
      } else
        request->send(200, "application/json", "{\"status\":\"error\", \"message\": \"NÃO HÁ RESERVAS REGISTRADAS PARA ESTE USUÁRIO\"}");
    } else
      request->send(200, "application/json", "{\"status\":\"error\", \"message\": \"NÃO HÁ RESERVAS REGISTRADAS PARA O PARQUE 1\"}");
  });

  server.on("/openReserve2", HTTP_GET, [](AsyncWebServerRequest *request) {
    Serial.println("--------> openReserve2:");
    if (flagAgend2) {
      String username = request->getParam("username")->value();
      if (username.equals(userReservation2)) {
        Serial.println("======= ABRINDO=============================");
        arduino.println("D");
        request->send(200, "application/json", "{\"status\":\"success\"}");
      } else
        request->send(200, "application/json", "{\"status\":\"error\", \"message\": \"NÃO HÁ RESERVAS REGISTRADAS PARA ESTE USUÁRIO\"}");
    } else
      request->send(200, "application/json", "{\"status\":\"error\", \"message\": \"NÃO HÁ RESERVAS REGISTRADAS PARA O PARQUE 2\"}");
  });

  // POST request para Cadastro de Usuario
  server.on("/reservation1", HTTP_GET, [](AsyncWebServerRequest *request) {
    Serial.println("------> reservation1");
    String username = request->getParam("username")->value();
    String password = request->getParam("password")->value();
    if (!isUser(username, password)) {
      Serial.println("######## Username e Senha Inválidos!\n ########");
      //request->redirect("/home");
      request->send(200, "application/json", "{\"status\":\"error\", \"message\":\"Usuário Não Encontrado!\"}");
    } else {
      if (flagAgend1 == false) {
        flagAgend1 = true;
        userReservation1 = username;
        timerReservation1 = millis();
        arduino.println("Y");
        Serial.println("--->Reserva agendada!\nUsuario:" + userReservation1 + "\nRedirecionando para ---> index.html");
        request->send(200, "application/json", "{\"status\":\"success\", \"message\":\"RESERVADO COM SUCESSO!\"}");
      } else
        request->send(200, "application/json", "{\"status\":\"error\", \"message\":\"UPS!O ESPACO JÁ SE ENCONTRA RESERVADO!\"}");
      //request->redirect("/home");
    }
  });

  server.on("/reservation2", HTTP_GET, [](AsyncWebServerRequest *request) {
    Serial.println("------> reservation2");
    String username = request->getParam("username")->value();
    String password = request->getParam("password")->value();
    if (!isUser(username, password)) {
      Serial.println("######## Username e Senha Inválidos!\n ########");
      request->send(200, "application/json", "{\"status\":\"error\", \"message\":\"Usuário Não Encontrado!\"}");
    } else {
      if (flagAgend2 == false) {
        flagAgend2 = true;
        userReservation2 = username;
        timerReservation2 = millis();
        arduino.println("Z");
        Serial.println("--->Reserva agendada!\nUsuario:" + userReservation2 + "\nRedirecionando para ---> index.html");
        request->send(200, "application/json", "{\"status\":\"success\", \"message\":\"RESERVADO COM SUCESSO!\"}");
      } else
        request->send(200, "application/json", "{\"status\":\"error\", \"message\":\"UPS! O ESPACO JÁ SE ENCONTRA RESERVADO!\"}");
    }
  });
  server.onNotFound(notFound);
  server.begin();
}
