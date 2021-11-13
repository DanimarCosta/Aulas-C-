#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <FS.h>
#include <ArduinoJson.h>

// Definicoes da rede Wifi a ser criada
const char* ssid = "Carrinho";
const char* password = "grupox";

unsigned long timestamp;
bool flag = false;

//Definicoes pinos Arduino ligados a entrada da Ponte H
uint8_t m_pinA1 = 0; // Marrom D3
uint8_t m_pinA2 = 2; // Vermelho D4
uint8_t m_pinB1 = 4; // Laranja D2
uint8_t m_pinB2 = 5; // Amarelo D1

const int led = 16;

// A Esquerdo
// B Direito

// Função para o carrinho ir para frente
void goForward(uint16_t pwm) { 
  analogWrite(m_pinA1, pwm);
  digitalWrite(m_pinA2, HIGH);
  analogWrite(m_pinB1, pwm);
  digitalWrite(m_pinB2, HIGH); 
}

// Função para o carrinho ir para trás
void goBackward(uint16_t pwm) { 
  analogWrite(m_pinA1, pwm);
  digitalWrite(m_pinA2, LOW);
  analogWrite(m_pinB1, pwm);
  digitalWrite(m_pinB2, LOW);
}

// Função para o carrinho ir para a direita
void turnRight(uint16_t pwm) { 
  analogWrite(m_pinA1, pwm);
  digitalWrite(m_pinA2, HIGH);
  analogWrite(m_pinB1, pwm);
  digitalWrite(m_pinB2, LOW);
}

//Função para o carrinho ir para a esquerda
void turnLeft(uint16_t pwm) { 
  analogWrite(m_pinA1, pwm);
  digitalWrite(m_pinA2, LOW);
  analogWrite(m_pinB1, pwm);
  digitalWrite(m_pinB2, HIGH);
}

//Função para o carrinho parar
void stop() {
  analogWrite(m_pinA1, 0);
  digitalWrite(m_pinA2, 0);
  analogWrite(m_pinB1, 0);
  digitalWrite(m_pinB2, 0);
}
 
ESP8266WebServer server(80);
 
void setup() {
  pinMode(led, OUTPUT);

  digitalWrite(led, HIGH);

  //Define os pinos como saida
  pinMode(m_pinA1, OUTPUT);
  pinMode(m_pinA2, OUTPUT);
  pinMode(m_pinB1, OUTPUT);
  pinMode(m_pinB2, OUTPUT);
 
  Serial.begin(115200);
  startWiFi();
  SPIFFS.begin();

  digitalWrite(led, LOW);
    
  startServer();
  Serial.println();
}
 
void loop() {
  server.handleClient();

  if(timestamp < millis() - 2000 && flag) {
     flag = !flag;
     stop();
  }
  
  yield();
}

// Função para iniciar o wifi
void startWiFi() {
  WiFi.softAP(ssid);
  Serial.print("Server IP address: ");
  Serial.println(WiFi.softAPIP());
}

//Função para iniciar o servidor
void startServer() {
  // aguarda por requisições na URL raiz "/" ou "/jsdata"
  server.on("/", handleIndexFile);
  server.on("/code.min.js", handleJsFile);  
  server.on("/jsdata", handleJsData);
  server.begin();
}

// Função para lidar com o codigo em JavaScript(controle)
void handleJsFile() {
  File file = SPIFFS.open("/code.min.js", "r");
  server.streamFile(file, "application/javascript");
  file.close();
}

// Função para lidar com o codigo HTML(controle)
void handleIndexFile() {
  File file = SPIFFS.open("/index.html", "r");
  server.streamFile(file, "text/html");
  file.close();
}
 
// recebe os dados do Joystick, parseia dados JSON e chama função de movimentação dos motores
void handleJsData() {
  digitalWrite(led, HIGH);
  
  String data = server.arg("plain");
  server.send(204, "");
  
  DynamicJsonDocument doc(512);
  deserializeJson(doc, data);

  String state = doc["state"];
  String angle = doc["direction"];
  int dist = doc["distance"];
   
  moveMotors(dist, angle, state);
  
  digitalWrite(led, LOW);
}
 
// move motores para determinada posição
void moveMotors(int dist, String angle, String state) {
  if(state == "end") {
    stop();
  } else {      
    if(angle == "up") goBackward(map(dist, 0, 100, 0, 1024));
    if(angle == "down") goForward(map(dist, 0, 100, 0, 1024));
    if(angle == "left") turnLeft(map(dist, 0, 100, 0, 1024));
    if(angle == "right") turnRight(map(dist, 0, 100, 0, 1024));
    
    flag = true;
    timestamp = millis();
  }   
}
