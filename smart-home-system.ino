
#include <WiFi.h>
#include <DHTesp.h>

const char* ssid = "wifi-iot";
const char* password = "password-iot";

WiFiServer server(80);

String header;


const int LDR_DEPAN = 34;  
const int LDR_TAMAN = 35; 
const int DHT_PIN = 23;  
const int IR_SENSOR = 25;  

const int LAMPU_DEPAN = 26;  
const int LAMPU_RUANG = 27; 
const int AC_RUANG = 32;   
const int EXHAUST = 33;      
const int LAMPU_TAMAN = 14;  
const int PINTU = 12;       


DHTesp dht;


String output26State = "off";
String output27State = "off";  
String output32State = "off";  
String output33State = "off";  
String output34State = "off";  
String output35State = "off";  


bool autoModeLampuDepan = false;
bool autoModeExhaust = false;
bool autoModeLampuTaman = false;
bool autoModePintu = false;

int THRESHOLD_CAHAYA = 500;         
float THRESHOLD_SUHU = 28.0;        
float THRESHOLD_SUHU_LOWER = 27.0;  

void setup() {
  Serial.begin(115200);
  dht.setup(DHT_PIN, DHTesp::DHT22);

  pinMode(LDR_DEPAN, INPUT);
  pinMode(LDR_TAMAN, INPUT);
  pinMode(IR_SENSOR, INPUT);

  pinMode(LAMPU_DEPAN, OUTPUT);
  pinMode(LAMPU_RUANG, OUTPUT);
  pinMode(AC_RUANG, OUTPUT);
  pinMode(EXHAUST, OUTPUT);
  pinMode(LAMPU_TAMAN, OUTPUT);
  pinMode(PINTU, OUTPUT);

  digitalWrite(LAMPU_DEPAN, LOW);
  digitalWrite(LAMPU_RUANG, LOW);
  digitalWrite(AC_RUANG, LOW);
  digitalWrite(EXHAUST, LOW);
  digitalWrite(LAMPU_TAMAN, LOW);
  digitalWrite(PINTU, LOW);

  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  server.begin();
}

void checkSensors() {
  if (autoModeLampuDepan) {
    int nilaiLDRDepan = analogRead(LDR_DEPAN);
    if (nilaiLDRDepan > THRESHOLD_CAHAYA) {
      digitalWrite(LAMPU_DEPAN, HIGH);
      output26State = "on";
    } else {
      digitalWrite(LAMPU_DEPAN, LOW);
      output26State = "off";
    }
  }

  if (autoModeExhaust) {
    float suhu = dht.getTemperature();
    Serial.println(suhu);

    if (suhu > THRESHOLD_SUHU) {
      digitalWrite(EXHAUST, LOW);
      output33State = "on";
    } else {
      digitalWrite(EXHAUST, HIGH);
      output33State = "off";
    }
  }
  if (autoModeLampuTaman) {
    int nilaiLDRTaman = analogRead(LDR_TAMAN);
    if (nilaiLDRTaman < THRESHOLD_CAHAYA) {
      digitalWrite(LAMPU_TAMAN, HIGH);
      output34State = "on";
    } else {
      digitalWrite(LAMPU_TAMAN, LOW);
      output34State = "off";
    }
  }

  if (autoModePintu) {
    if (digitalRead(IR_SENSOR) == LOW) {  
      digitalWrite(PINTU, HIGH);
      output35State = "on";
      delay(5000); 
      digitalWrite(PINTU, LOW);
      output35State = "off";
    }
  }
}


void loop() {
  checkSensors();                       
  delay(dht.getMinimumSamplingPeriod());  

  WiFiClient client = server.available();

  if (client) {
    String currentLine = "";
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        Serial.write(c);
        header += c;
        if (c == '\n') {
          if (currentLine.length() == 0) {
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();

            if (header.indexOf("GET /26/on") >= 0) {
              output26State = "on";
              if (!autoModeLampuDepan) {
                digitalWrite(LAMPU_DEPAN, HIGH);
              }

            } else if (header.indexOf("GET /26/off") >= 0) {
              output26State = "off";
              if (!autoModeLampuDepan) {
                digitalWrite(LAMPU_DEPAN, LOW);
              }

            } else if (header.indexOf("GET /26/auto") >= 0) {


              autoModeLampuDepan = !autoModeLampuDepan;
            } else if (header.indexOf("GET /27/on") >= 0) {
              output27State = "on";
              digitalWrite(LAMPU_RUANG, HIGH);
            } else if (header.indexOf("GET /27/off") >= 0) {
              output27State = "off";
              digitalWrite(LAMPU_RUANG, LOW);
            } else if (header.indexOf("GET /32/on") >= 0) {
              output32State = "on";
              digitalWrite(AC_RUANG, HIGH);
            } else if (header.indexOf("GET /32/off") >= 0) {
              output32State = "off";
              digitalWrite(AC_RUANG, LOW);


            } else if (header.indexOf("GET /33/on") >= 0) {
              output33State = "on";
              if (!autoModeExhaust) {
                digitalWrite(EXHAUST, HIGH);
              }

            } else if (header.indexOf("GET /33/off") >= 0) {
              output33State = "off";
              if (!autoModeExhaust) {
                digitalWrite(EXHAUST, LOW);
              }

            } else if (header.indexOf("GET /33/auto") >= 0) {

              autoModeExhaust = !autoModeExhaust;
            } else if (header.indexOf("GET /34/on") >= 0) {
              output34State = "on";
              if (!autoModeLampuTaman) {
                digitalWrite(LAMPU_TAMAN, HIGH);
              }

            } else if (header.indexOf("GET /34/off") >= 0) {
              output34State = "off";
              if (!autoModeLampuTaman) {
                digitalWrite(LAMPU_TAMAN, LOW);
              }

            } else if (header.indexOf("GET /34/auto") >= 0) {


              autoModeLampuTaman = !autoModeLampuTaman;
            } else if (header.indexOf("GET /35/on") >= 0) {
              output35State = "on";
              if (!autoModePintu) {
                digitalWrite(PINTU, HIGH);
              }

            } else if (header.indexOf("GET /35/off") >= 0) {
              output35State = "off";
              if (!autoModePintu) {
                digitalWrite(PINTU, LOW);
              }

            } else if (header.indexOf("GET /35/auto") >= 0) {


              autoModePintu = !autoModePintu;
            }

            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<link rel=\"icon\" href=\"data:,\">");

            client.println("<style>");

            client.println("html { background: linear-gradient(135deg, #1e3c72 0%, #2a5298 100%); font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center; color: white; min-height: 100vh; }");
            client.println("body { margin: 0; padding: 20px; }");
            client.println("h1 { color: #ffffff; text-shadow: 2px 2px 4px rgba(0,0,0,0.3); margin-bottom: 30px; }");

            client.println(".device-container { background: rgba(255, 255, 255, 0.1); margin: 15px auto; padding: 20px; border-radius: 15px; max-width: 600px; backdrop-filter: blur(10px); box-shadow: 0 4px 6px rgba(0, 0, 0, 0.1); }");
            client.println(".status-container { display: flex; justify-content: center; align-items: center; gap: 10px; margin-bottom: 15px; }");

            client.println(".status-text { font-size: 24px; margin: 10px 0; color: #ffffff; }");
            client.println(".auto-status { font-weight: bold; color: #FFD700; padding: 5px 10px; border-radius: 5px; background: rgba(255, 215, 0, 0.1); }");

            client.println(".button-container { display: flex; justify-content: center; gap: 10px; flex-wrap: wrap; }");
            client.println(".button { background-color: #4CAF50; border: none; color: white; padding: 15px 30px; text-decoration: none; font-size: 20px; margin: 2px; cursor: pointer; border-radius: 8px; transition: all 0.3s ease; box-shadow: 0 2px 4px rgba(0,0,0,0.2); }");
            client.println(".button:hover { transform: translateY(-2px); box-shadow: 0 4px 8px rgba(0,0,0,0.2); }");
            client.println(".button2 { background-color: #b22222; }");
            client.println(".button3 { background-color: #808080; }");
            client.println(".auto-active { background-color: #FFD700; color: black; }");

            client.println(".sensor-container { margin-top: 30px; }");
            client.println(".sensor-value { background: rgba(255, 255, 255, 0.15); padding: 10px; border-radius: 8px; margin: 10px 0; font-size: 18px; }");
            client.println("</style></head>");

            client.println("<body>");
            client.println("<h1>Smart Home Control System</h1>");

            client.println("<div class=\"device-container\">");
            client.println("<div class=\"status-container\">");
            client.println("<p class=\"status-text\">Lampu Depan - " + output26State + (autoModeLampuDepan ? " <span class=\"auto-status\">AUTO MODE</span>" : "") + "</p>");
            client.println("</div>");
            client.println("<div class=\"button-container\">");
            if (output26State == "off") {
              client.println("<a href=\"/26/on\"><button class=\"button\">ON</button></a>");
            } else {
              client.println("<a href=\"/26/off\"><button class=\"button button2\">OFF</button></a>");
            }
            client.println("<a href=\"/26/auto\"><button class=\"button button3" + String(autoModeLampuDepan ? " auto-active" : "") + "\">AUTO</button></a>");
            client.println("</div></div>");

            client.println("<div class=\"device-container\">");
            client.println("<div class=\"status-container\">");
            client.println("<p class=\"status-text\">Lampu Ruangan - " + output27State + "</p>");
            client.println("</div>");
            client.println("<div class=\"button-container\">");
            if (output27State == "off") {
              client.println("<a href=\"/27/on\"><button class=\"button\">ON</button></a>");
            } else {
              client.println("<a href=\"/27/off\"><button class=\"button button2\">OFF</button></a>");
            }
            client.println("</div></div>");

            client.println("<div class=\"device-container\">");
            client.println("<div class=\"status-container\">");
            client.println("<p class=\"status-text\">AC Ruangan - " + output32State + "</p>");
            client.println("</div>");
            client.println("<div class=\"button-container\">");
            if (output32State == "off") {
              client.println("<a href=\"/32/on\"><button class=\"button\">ON</button></a>");
            } else {
              client.println("<a href=\"/32/off\"><button class=\"button button2\">OFF</button></a>");
            }
            client.println("</div></div>");

            client.println("<div class=\"device-container\">");
            client.println("<div class=\"status-container\">");
            client.println("<p class=\"status-text\">Exhaust Fan - " + output33State + (autoModeExhaust ? " <span class=\"auto-status\">AUTO MODE</span>" : "") + "</p>");
            client.println("</div>");
            client.println("<div class=\"button-container\">");
            if (output33State == "off") {
              client.println("<a href=\"/33/on\"><button class=\"button\">ON</button></a>");
            } else {
              client.println("<a href=\"/33/off\"><button class=\"button button2\">OFF</button></a>");
            }
            client.println("<a href=\"/33/auto\"><button class=\"button button3" + String(autoModeExhaust ? " auto-active" : "") + "\">AUTO</button></a>");
            client.println("</div></div>");

            client.println("<div class=\"device-container\">");
            client.println("<div class=\"status-container\">");
            client.println("<p class=\"status-text\">Lampu Taman - " + output34State + (autoModeLampuTaman ? " <span class=\"auto-status\">AUTO MODE</span>" : "") + "</p>");
            client.println("</div>");
            client.println("<div class=\"button-container\">");
            if (output34State == "off") {
              client.println("<a href=\"/34/on\"><button class=\"button\">ON</button></a>");
            } else {
              client.println("<a href=\"/34/off\"><button class=\"button button2\">OFF</button></a>");
            }
            client.println("<a href=\"/34/auto\"><button class=\"button button3" + String(autoModeLampuTaman ? " auto-active" : "") + "\">AUTO</button></a>");
            client.println("</div></div>");

            client.println("<div class=\"device-container\">");
            client.println("<div class=\"status-container\">");
            client.println("<p class=\"status-text\">Pintu - " + output35State + (autoModePintu ? " <span class=\"auto-status\">AUTO MODE</span>" : "") + "</p>");
            client.println("</div>");
            client.println("<div class=\"button-container\">");
            if (output35State == "off") {
              client.println("<a href=\"/35/on\"><button class=\"button\">BUKA</button></a>");
            } else {
              client.println("<a href=\"/35/off\"><button class=\"button button2\">TUTUP</button></a>");
            }
            client.println("<a href=\"/35/auto\"><button class=\"button button3" + String(autoModePintu ? " auto-active" : "") + "\">AUTO</button></a>");
            client.println("</div></div>");

            if (autoModeLampuDepan || autoModeExhaust || autoModeLampuTaman || autoModePintu) {
              client.println("<div class=\"device-container sensor-container\">");
              client.println("<h2>Status Sensor</h2>");

              if (autoModeLampuDepan) {
                int nilaiLDRDepan = analogRead(LDR_DEPAN);
                client.println("<div class=\"sensor-value\">Intensitas Cahaya Depan: " + String(nilaiLDRDepan) + "</div>");
              }

              if (autoModeExhaust) {
                float suhu = dht.getTemperature();
                if (!isnan(suhu)) {
                  client.println("<div class=\"sensor-value\">Suhu Ruangan: " + String(suhu, 1) + "°C</div>");
                }
              }

              if (autoModeLampuTaman) {
                int nilaiLDRTaman = analogRead(LDR_TAMAN);
                client.println("<div class=\"sensor-value\">Intensitas Cahaya Taman: " + String(nilaiLDRTaman) + "</div>");
              }

              client.println("</div>");
            }

            client.println("</body></html>");

            client.println();
            break;
          } else {  
            currentLine = "";
          }
        } else if (c != '\r') {
          currentLine += c;
        }
      }
    }

    header = "";

    client.stop();
    Serial.println("Client disconnected.");
    Serial.println("");
  }
}
