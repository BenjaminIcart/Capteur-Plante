#include <NTPClient.h>
#include <Discord_WebHook.h>
#include <WiFiUdp.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

const char *SSID = "YOUR_SSID";
const char *PASSWORD = "YOUR_PASSWORD";

ESP8266WebServer server(80);

Discord_Webhook discord;
String DISCORD_WEBHOOK = "YOUR_DISCORD_WEBHOOK";

unsigned long epoch = 0;
String heure = "";
int pinMoistureSensor = D2;
int pinPhotoresistance = D7;

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "europe.pool.ntp.org", 7200, 60000);

void setup() {
  Serial.begin(115200);
  discord.begin(DISCORD_WEBHOOK);
  discord.addWiFi(SSID, PASSWORD);
  discord.connectWiFi();
  discord.send("Connecté");

  pinMode(pinMoistureSensor, OUTPUT);
  pinMode(pinPhotoresistance, OUTPUT);
  pinMode(A0, INPUT);

  WiFi.begin(SSID, PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  timeClient.begin();
  server.on("/", handleRoot);
  server.on("/on", handleOn); // Nouvelle route pour "/on"
  server.begin();
  Serial.println("Serveur HTTP démarré");
}

void loop() {
  timeClient.update();
  epoch = timeClient.getEpochTime();
  heure = timeClient.getFormattedTime();
  server.handleClient();
  envoyermessage();
}

void envoyermessage() {
  if (heure == "19:00:00") {
    String msgmoistureValue = "";
    String msgphotoresistanceValue = "";
    
    int moistureValue = readSensor(pinMoistureSensor);
    if (moistureValue > 110) {
      msgmoistureValue = " donc votre sol est extremement sec, il faut l'arroser au plus vite";
    } else if (moistureValue > 90 && moistureValue < 110) {
      msgmoistureValue = " donc votre sol est un peu sec, l'arrosage peut attendre un peu...";
    } else if (moistureValue < 90) {
      msgmoistureValue = " donc votre sol est parfaitement arrosé, bien joué !";
    }
    discord.send("L'humidité est de : " + String(moistureValue) + msgmoistureValue);

    int photoresistanceValue = readSensor(pinPhotoresistance);
    if (photoresistanceValue < 500) {
      msgphotoresistanceValue = " donc l'environnement est trop sombre, il faut déplacer la plante";
    } else if (photoresistanceValue >= 500) {
      msgphotoresistanceValue = " donc l'environnement est parfait !";
    }
    discord.send("La luminosité est de : " + String(photoresistanceValue) + msgphotoresistanceValue);
    discord.send("Vous voulez connaitre les infos de votre plante toute la journée ? http://" + WiFi.localIP().toString());
  }

    if (heure == "08:00:00") {
    String msgmoistureValue = "";
    String msgphotoresistanceValue = "";
    
    int moistureValue = readSensor(pinMoistureSensor);
    if (moistureValue > 110) {
      msgmoistureValue = " donc votre sol est extremement sec, il faut l'arroser au plus vite";
    } else if (moistureValue > 90 && moistureValue < 110) {
      msgmoistureValue = " donc votre sol est un peu sec, l'arrosage peut attendre un peu...";
    } else if (moistureValue < 90) {
      msgmoistureValue = " donc votre sol est parfaitement arrosé, bien joué !";
    }
    discord.send("L'humidité est de : " + String(moistureValue) + msgmoistureValue);

    int photoresistanceValue = readSensor(pinPhotoresistance);
    if (photoresistanceValue < 500) {
      msgphotoresistanceValue = " donc l'environnement est trop sombre, il faut déplacer la plante";
    } else if (photoresistanceValue >= 500) {
      msgphotoresistanceValue = " donc l'environnement est parfait !";
    }
    discord.send("La luminosité est de : " + String(photoresistanceValue) + msgphotoresistanceValue);
    discord.send("Vous voulez connaitre les infos de votre plante toute la journée ? http://" + WiFi.localIP().toString());
  }
}

void handleRoot() {
  int moistureValue = readSensor(pinMoistureSensor);
  int photoresistanceValue = readSensor(pinPhotoresistance);
  
  String html = "<html><body>";
  html += "<h1>Serveur Web Alocasia</h1>";
  html += "<p>Humidit&eacute;e du sol : " + String(moistureValue) + "</p>";
  html += "<p>Luminosit&eacute;e : " + String(photoresistanceValue) + "</p>";
  html += "</body></html>";
  
  server.send(200, "text/html", html);
}

void handleOn() {
      String msgmoistureValue = "";
    String msgphotoresistanceValue = "";
    
    int moistureValue = readSensor(pinMoistureSensor);
    if (moistureValue > 110) {
      msgmoistureValue = " donc votre sol est extremement sec, il faut l'arroser au plus vite";
    } else if (moistureValue > 90 && moistureValue < 110) {
      msgmoistureValue = " donc votre sol est un peu sec, l'arrosage peut attendre un peu...";
    } else if (moistureValue < 90) {
      msgmoistureValue = " donc votre sol est parfaitement arrosé, bien joué !";
    }
    discord.send("L'humidité est de : " + String(moistureValue) + msgmoistureValue);

    int photoresistanceValue = readSensor(pinPhotoresistance);
    if (photoresistanceValue < 500) {
      msgphotoresistanceValue = " donc l'environnement est trop sombre, il faut déplacer la plante";
    } else if (photoresistanceValue >= 500) {
      msgphotoresistanceValue = " donc l'environnement est parfait !";
    }
    discord.send("La luminosité est de : " + String(photoresistanceValue) + msgphotoresistanceValue);
    discord.send("Vous voulez connaitre les infos de votre plante toute la journée ? http://" + WiFi.localIP().toString());

}


int readSensor(int pin) {
  digitalWrite(pin, HIGH);
  delay(500); // Attendre que le capteur soit prêt
  int sensorValue = analogRead(A0);
  digitalWrite(pin, LOW);
  return sensorValue;
}
