/**************************************************
    AUTHOR: Erivelto Silva.
    FOR: Barros.
    ISPB.
    CREATED AT: 28-05-2024.
*************************************************/

////////////////// Libraries Used  ////////////////
#include <WiFi.h>                  /////
#include "SPIFFS.h"                /////
#include <AsyncTCP.h>              /////
#include <ArduinoJson.h>           /////
#include "esp_task_wdt.h"          /////
#include <ESPAsyncWebServer.h>     /////
#include <IOXhop_FirebaseESP32.h>  /////
///////////////////////////////////////////////////

////////////////  PIN CONFIGURATIONS //////////////
#define LED 2                                                                                              /////
#define RXD2 16                                                                                            // RX da serial do ESP32      /////
#define TXD2 17                                                                                            // TX da serial do ESP32      /////
#define FIREBASE_HOST "https://park-space-control-system-default-rtdb.europe-west1.firebasedatabase.app/"  // substitua "Link_do_seu_banco_de_dados" pelo link do seu banco de dados
#define FIREBASE_AUTH "oVTZYyRDQLbLFnYvYV1EIim5PqQTTLMpAvTUVvAa"                                           // substitua "Senha_do_seu_banco_de_dados" pela senha do seu banco de dados                                                                       /////
#define TIME_BETWEEN_READS_IN_FRIREBASE 5000 //5segundos
///////////////////////////////////////////////////

/////////////  NETWORK CONFIGURATIONS /////////////
// #define SSID "PARK"           /////
// #define PASSWORD "123456789"  /////
#define SSID "NETHOUSE"           /////
#define PASSWORD "Eduanara3130"  /////
///////////////////////////////////////////////////

////////// VARIABLES USED IN THE PROJECT //////////
bool flagReserve1 = false;                  /////
bool flagReserve2 = false;                  /////
String dataStored = "D*0*0*0*0*0*0*";       /////
String userReservation1 = "";               /////
String userReservation2 = "";               /////
unsigned long int timerReservation1 = 0;    /////
unsigned long int timerReservation2 = 0;    /////
unsigned long int endTimerReservation1 = 0;  /////
unsigned long int endTimerReservation2 = 0;  /////
unsigned long int timeMinute = 0;           /////
unsigned long int timeDelay = 0;            /////
///////////////////////////////////////////////////

////////// VARIABLES USED IN REGISTER /////////////
bool flagRegisterUser = false;
bool flagRegisterReserve1 = false;
bool flagRegisterReserve2 = false;

String registerUsername;
String registerLicensePlate;
String registerVehicleBrand;
String registerPassword;
int registerTimeReserve;
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

///////////////////////////////////////////////////
void notFound(AsyncWebServerRequest *request) {
  request->send(404, "text/plain", "Not found");
}

////////////////////////////////////////////////////
void setup() {
  initConfig();
  wifiConfig();

  if (initMyFS())
    Serial.println(" ## PARTIÇÃO SPIFFS MONTADA! ##");
  else
    Serial.println(" ## ERRO MONTANDO A PARTIÇÃO SPIFFS ##");

  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  serverHandlers();
  Serial.println(" ##-------SISTEMA DE DOMÓTICA!--------##");
}

/////////////////////////////////////////////////////////////////
void loop() {
  readReserveStatusInFirebase();
  verifyReserveExpiration();
  registerToFirebase();

  if (millis() - timeDelay > 800) {
    timeDelay = millis();
    askData();
    String dataReceived = receiveData();
    if (dataReceived != "") {
      dataStored = dataReceived;

      Serial.println("--> RECEBIDO DO ARDUINO:" + dataStored);
      long time1 = 0;
      long time2 = 0;
      if (flagReserve1)
      {
        time1 = (endTimerReservation1 - (millis() - timerReservation1)) / 1000;
        if(time1<=0) //tempo de reserva acabou
        {
          time1=0;
          Firebase.setInt("/reserve1/time", 0);
          flagReserve1=false;
        }
      }

      if (flagReserve2)
      {
        time2 = (endTimerReservation2 - (millis() - timerReservation2)) / 1000;
        if(time2<=0) //tempo de reserva acabou
        {
          time2=0;
          Firebase.setInt("/reserve2/time", 0);
          flagReserve2=false;
        }
      }  

      dataStored.replace("\n", "");
      dataStored.replace("\r", "");
      dataStored += (flagReserve1) ? "1*" : "0*";
      dataStored += (flagReserve2) ? "1*" : "0*";
      dataStored += String(time1) + "*";
      dataStored += String(time2) + "*";
    }
    if (flagReserve1)
      Serial.println("ESPACO 1 AGENDADO POR:" + userReservation1);
    if (flagReserve2)
      Serial.println("ESPACO 2 AGENDADO POR:" + userReservation2);
    digitalWrite(LED, !digitalRead(LED));
  }
  delay(20);
}

//////////////////////////////////////////////////////////////////////////////
bool initMyFS() {
  return (SPIFFS.begin(true));
}

//////////////////////////////////////////////////////////////////////////////
void initConfig() {
  pinMode(LED, OUTPUT);
  Serial.begin(9600);
  delay(500);
  arduino.begin(9600, SERIAL_8N1, RXD2, TXD2);
  delay(500);
  Serial.println("CONFIGURAÇÕES INICIAS SETADAS!");
  delay(5000);
}

//////////////////////////////////////////////////////////////////////////////
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
void readReserveStatusInFirebase() {
  if (millis() - timeMinute > TIME_BETWEEN_READS_IN_FRIREBASE) {
    timeMinute = millis();
    endTimerReservation1 = Firebase.getInt("/reserve1/time");
    endTimerReservation2 = Firebase.getInt("/reserve2/time");
    flagReserve1 = endTimerReservation1 > 0? true: false; 
    flagReserve2 = endTimerReservation2 > 0? true: false; 
  }
}

//////////////////////////////////////////////////////////////////////////////
void registerToFirebase() {
  if (flagRegisterReserve1) {
    Serial.println("&&&&&&&&&&&&&&&&&&&&&& Registering Reservation1 &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&");
    flagRegisterReserve1 = false;
    flagReserve1 = true;
    Firebase.setString("/reserve1/user", registerUsername);
    Firebase.setInt("/reserve1/time", registerTimeReserve);
    Firebase.setString("/reserve1/vehicleBrand", registerVehicleBrand);
    Firebase.setString("/reserve1/licensePlate", registerLicensePlate);

    String reserve = registerUsername + "|" + registerLicensePlate+"|" +registerTimeReserve; 
    Firebase.pushString("/reserves/", reserve);
    
    userReservation1 = registerUsername;
    timerReservation1 = millis();
    endTimerReservation1 = registerTimeReserve;
    cleanRegisterFieldS();
    return;
  }
  if (flagRegisterReserve2) {
    Serial.println("&&&&&&&&&&&&&&&&&&&&&&&&& Registering Reservation2 &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&");
    flagRegisterReserve2 = false;
    flagReserve2 = true;
    Firebase.setString("/reserve2/user", registerUsername);
    Firebase.setInt("/reserve2/time", registerTimeReserve);
    Firebase.setString("/reserve2/vehicleBrand", registerVehicleBrand);
    Firebase.setString("/reserve2/licensePlate", registerLicensePlate);

    String reserve = registerUsername + "|" + registerLicensePlate+"|" + registerTimeReserve; 
    Firebase.pushString("/reserves/", reserve);

    userReservation2 = registerUsername;
    timerReservation2 = millis();
    endTimerReservation2 = registerTimeReserve;
    cleanRegisterFieldS();
    return;
  }

  if(flagRegisterUser) {
    Serial.println("&&&&&&&&&&&&&&&&&&&&&&&&&&&&& Registering User &&&&&&&&&&&&&&&&&&&&&&&&&&&&");
    flagRegisterUser = false;
    Firebase.setString("/users/" + registerUsername + "/password", registerPassword);
    Firebase.setString("/users/" + registerUsername + "/licensePlate", registerLicensePlate);
    Firebase.setString("/users/" + registerUsername + "/vehicleBrand", registerVehicleBrand);
    cleanRegisterFieldS();
  }
}

//////////////////////////////////////////////////////////////////////////////
void verifyReserveExpiration() {
  if (flagReserve1 && ((millis() - timerReservation1) > endTimerReservation1)) {
    flagReserve1 = false;
    userReservation1 = "";
    arduino.println("y");
    Firebase.setInt("reserve1/time", 0);
    Serial.println("################################TEMPO DA RESERVA 1 EXPIROU ########################################");
  }

  if (flagReserve2 && ((millis() - timerReservation2) > endTimerReservation2)) {
    flagReserve2 = false;
    userReservation2 = "";
    arduino.println("z");
    Firebase.setInt("reserve2/time", 0);
    Serial.println("################################TEMPO DA RESERVA 2 EXPIROU ########################################");
  }
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
      arduino.println((flagReserve1) ? 'Y' : 'y');
    else
      arduino.println((flagReserve2) ? 'Z' : 'z');
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

//////////////////////////////////////////////////////////////////////////////
bool isUser(String username, String password) {
  String passwordReturned = Firebase.getString("/users/" + username + "/password");
  return (passwordReturned.equals(password));
}

String getUsernameByReserveNumber(int num){
  switch(num){
    case 1: return Firebase.getString("reserve1/user");break;
    case 2: return Firebase.getString("reserve2/user");break;
  }
  return "";
}
//////////////////////////////////////////////////////////////////////////////
bool isCarOwner(String username, String licensePlate) {
  String licensePlateReturned = Firebase.getString("/users/" + username + "/licensePlate");
  return (licensePlateReturned.equals(licensePlate));
}

//////////////////////////////////////////////////////////////////////////////
void cleanRegisterFieldS() {
  registerUsername = "";
  registerVehicleBrand = "";
  registerLicensePlate = "";
  registerPassword = "";
  registerTimeReserve = 0;
}

//////////////////////////////////////////////////////////////////////////////
void serverHandlers() 
{
  server.on("/bootstrap.min.css", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/bootstrap.min.css", "text/css");
  });

  server.on("/chart.min.js", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/chart.min.js", "text/javascript");
  });

  // Routes
  server.on("/dashboard", HTTP_GET, [](AsyncWebServerRequest *request) {
    Serial.println("--------> dashboard.html");
    request->send(SPIFFS, "/dashboard.html");
  });

  server.on("/dashboard.js", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/dashboard.js", "text/javascript");
  });

  server.on("/home", HTTP_GET, [](AsyncWebServerRequest *request) {
    Serial.println("--------> home.html");
    request->send(SPIFFS, "/home.html");
  });

  server.on("/home.js", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/home.js", "text/javascript");
  });

  server.on("/login", HTTP_GET, [](AsyncWebServerRequest *request) {
    Serial.println("--------> login.html");
    request->send(SPIFFS, "/login.html");
  });

  server.on("/main.js", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/main.js", "text/javascript");
  });

  server.on("/register", HTTP_GET, [](AsyncWebServerRequest *request) {
    Serial.println("--------> register.html");
    request->send(SPIFFS, "/register.html");
  });

  server.on("/style.css", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/style.css", "text/css");
  });

  server.on("/user.png", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/user.png", "image/png");
  });

  server.on("/vaga-livre.png", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/vaga-livre.png", "image/png");
  });

  server.on("/vaga-ocupada.png", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/vaga-ocupada.png", "image/png");
  });


  /*--------------------------ENDPOINS---------------------------*/
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    Serial.println("-------->redirecionando para login.html");
    request->redirect("/login");
  });

  server.on("/admin", HTTP_GET, [](AsyncWebServerRequest *request) {
    Serial.println("-------->redirecionando para dashboard.html");
    request->redirect("/dashboard");
  });
  
  server.on("/get-in", HTTP_POST, [](AsyncWebServerRequest *request) {
    esp_task_wdt_delete(NULL);  // Remove o watchdog do loopTask
    Serial.println("------> Req de /get-in do login.html");
    if (!(request->hasParam("username", true) && request->hasParam("password", true))) {
      Serial.println("---> Erro... Parametros de Login Incompletos!\nRedirecionando para ----> login.html");
      request->redirect("/login");
    } else {
      String username = request->getParam("username", true)->value();
      String password = request->getParam("password", true)->value();
      if (username.equals("admin") && password.equals("otlevire")) {
        Serial.println("--->Username e Senha de Admin Válidos!\nRedirecionando para ----> dashboard.html");
        request->redirect("/dashboard");
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

  server.on("/make-register", HTTP_POST, [](AsyncWebServerRequest *request) {
    esp_task_wdt_delete(NULL);  // Remove o watchdog do loopTask
    Serial.println("---- post --> Req de /register do register.html");
    if (!(request->hasParam("username", true) && request->hasParam("userPassword", true) && request->hasParam("adminPassword", true) && request->hasParam("vehicleBrand", true) && request->hasParam("licensePlate", true))) {
      Serial.println("---> Erro... Parametros de Registro Incompletos!\nRedirecionando para ----> register.html");
      request->redirect("/register");
    } else {
      registerUsername = request->getParam("username", true)->value();
      registerVehicleBrand = request->getParam("vehicleBrand", true)->value();
      registerLicensePlate = request->getParam("licensePlate", true)->value();
      registerPassword = request->getParam("userPassword", true)->value();
      String adminPassword = request->getParam("adminPassword", true)->value();

      if (adminPassword.equals("otlevire")) {
        Serial.println("--->Admin Válido! Salvando dados do novo usuário no Firebase...");
        flagRegisterUser = true;
        request->redirect("/login");
      } else {
        flagRegisterUser = false;
        Serial.println("--->Admin Inválido!Redirecionando para register.html");
        request->redirect("/register");
      }
    }
  });
  
  server.on("/dados", HTTP_GET, [](AsyncWebServerRequest *request) {
    Serial.println("--------> Dados:" + dataStored);
    String resp = "{\"status\":\"success\", \"data\":\"" + dataStored + "\"}";
    request->send(200, "application/json", resp);
  });

  server.on("/openReserve1", HTTP_GET, [](AsyncWebServerRequest *request) {
    esp_task_wdt_delete(NULL);  // Remove o watchdog do loopTask
    Serial.println("--------> openReserve1:");
    if (flagReserve1) {
      String username = request->getParam("username")->value();
      if (username.equals(getUsernameByReserveNumber(1))) {
        Serial.println("======= ABRINDO=============================");
        arduino.println("A");
        request->send(200, "application/json", "{\"status\":\"success\"}");
      } else
        request->send(200, "application/json", "{\"status\":\"error\", \"message\": \"NÃO HÁ RESERVAS REGISTRADAS PARA ESTE USUÁRIO\"}");
    } else
      request->send(200, "application/json", "{\"status\":\"error\", \"message\": \"NÃO HÁ RESERVAS REGISTRADAS PARA O PARQUE 1\"}");
  });

  server.on("/openReserve2", HTTP_GET, [](AsyncWebServerRequest *request) {
    esp_task_wdt_delete(NULL);  // Remove o watchdog do loopTask
    Serial.println("--------> openReserve2:");
    if (flagReserve2) {
      String username = request->getParam("username")->value();
      if (username.equals(getUsernameByReserveNumber(2))) {
        Serial.println("======= ABRINDO=============================");
        arduino.println("D");
        request->send(200, "application/json", "{\"status\":\"success\"}");
      } else
        request->send(200, "application/json", "{\"status\":\"error\", \"message\": \"NÃO HÁ RESERVAS REGISTRADAS PARA ESTE USUÁRIO\"}");
    } else
      request->send(200, "application/json", "{\"status\":\"error\", \"message\": \"NÃO HÁ RESERVAS REGISTRADAS PARA O PARQUE 2\"}");
  });

  server.on("/reservation1", HTTP_GET, [](AsyncWebServerRequest *request) {
    esp_task_wdt_delete(NULL);  // Remove o watchdog do loopTask
    Serial.println("------> reservation1");

    registerUsername = request->getParam("username")->value();
    registerPassword = request->getParam("password")->value();
    registerLicensePlate = request->getParam("licensePlate")->value();
    registerTimeReserve = (request->getParam("timeReserve")->value()).toInt();

    if (!isUser(registerUsername, registerPassword)) {
      Serial.println("######## Username e Senha Inválidos!\n ########");
      request->send(200, "application/json", "{\"status\":\"error\", \"message\":\"Usuário Não Encontrado!\"}");
      return;
    } else if (!isCarOwner(registerUsername, registerLicensePlate)) {
      request->send(200, "application/json", "{\"status\":\"error\", \"message\":\"Este veiculo não pertence a este Usuário!\"}");
      return;
    }
    else if (!flagReserve1) {
      flagRegisterReserve1 = true;
      arduino.println("Y");
      Serial.println("--->Reserva agendada!\nUsuario:" + registerUsername + "\nRedirecionando para ---> index.html");
      request->send(200, "application/json", "{\"status\":\"success\", \"message\":\"RESERVADO COM SUCESSO!\"}");
    } else {
      flagRegisterReserve1 = false;
      Serial.println("Firebase diz que reserve1 time != 0");
      request->send(200, "application/json", "{\"status\":\"error\", \"message\":\"UPS!O ESPACO JÁ SE ENCONTRA RESERVADO!\"}");
    }
  });

  server.on("/reservation2", HTTP_GET, [](AsyncWebServerRequest *request) {
    esp_task_wdt_delete(NULL);  // Remove o watchdog do loopTask
    Serial.println("------> reservation2");

    registerUsername = request->getParam("username")->value();
    registerPassword = request->getParam("password")->value();
    registerLicensePlate = request->getParam("licensePlate")->value();
    registerTimeReserve = (request->getParam("timeReserve")->value()).toInt();

    if (!isUser(registerUsername, registerPassword)) {
      Serial.println("######## Username e Senha Inválidos!\n ########");
      request->send(200, "application/json", "{\"status\":\"error\", \"message\":\"Usuário Não Encontrado!\"}");
      return;
    } else if (!isCarOwner(registerUsername, registerLicensePlate)) {
      request->send(200, "application/json", "{\"status\":\"error\", \"message\":\"Este veiculo não pertence a este Usuário!\"}");
      return;
    }

    else if (!flagReserve2) {
      flagRegisterReserve2 = true;
      arduino.println("Y");
      Serial.println("--->Reserva agendada!\nUsuario:" + registerUsername + "\nRedirecionando para ---> index.html");
      request->send(200, "application/json", "{\"status\":\"success\", \"message\":\"RESERVADO COM SUCESSO!\"}");
    } else {
      flagRegisterReserve2 = false;
      Serial.println("Firebase diz que reserve1 time != 0");
      request->send(200, "application/json", "{\"status\":\"error\", \"message\":\"UPS!O ESPACO JÁ SE ENCONTRA RESERVADO!\"}");
    }
  });

  server.onNotFound(notFound);
  server.begin();
}
