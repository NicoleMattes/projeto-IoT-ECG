import processing.serial.*;
 
Serial myPort;        // A porta serial
int xPos = 1;         // posição horizontal do gráfico
float height_old = 0;
float height_new = 0;
float inByte = 0;
int BPM = 0;
int beat_old = 0;
float[] beats = new float[500];  // Usado para calcular o BPM médio
int beatIndex;
float threshold = 620.0;  // Limite em que ocorre o cálculo de BPM
boolean belowThreshold = true;
PFont font;
 
void setup () {
  // definir o tamanho da janela:
  size(1000, 400);        
 
  // Lista todas as portas seriais disponíveis
  println(Serial.list());
  // Abra qualquer porta que você está usando.
  myPort = new Serial(this, Serial.list()[2], 9600);
  // não gere um serialEvent () a menos que você obtenha um caractere de nova linha:
  myPort.bufferUntil('\n');
  // definir fundo inicial:
  background(0xff);
  font = createFont("Ariel", 12, true);
}
 
 
void draw () {
     inByte = map(inByte, 0, 1023, 0, height);
     height_new = height - inByte; 
     line(xPos - 1, height_old, xPos, height_new);
     height_old = height_new;
      if (xPos >= width) {
        xPos = 0;
        background(0xff);
      } 
      else {
        xPos++;
      }    
      if (millis() % 128 == 0){
        fill(0xFF);
        rect(0, 0, 200, 20);
        fill(0x00);
        text("BPM: " + inByte, 15, 10);
      }
}
  
void serialEvent (Serial myPort) 
{
  String inString = myPort.readStringUntil('\n');
 
  if (inString != null) 
  {
    inString = trim(inString);
 
    if (inString.equals("!")) 
    { 
      stroke(0, 0, 0xff); 
      inByte = 512;  
    }
    else 
    {
      stroke(0xff, 0, 0); 
      inByte = float(inString); 
      if (inByte > threshold && belowThreshold == true)
      {
        calculateBPM();
        belowThreshold = false;
      }
      else if(inByte < threshold)
      {
        belowThreshold = true;
      }
    }
  }
}
  
void calculateBPM () 
{  
  int beat_new = millis();    
  int diff = beat_new - beat_old;   
  float currentBPM = 60000 / diff;   
  beats[beatIndex] = currentBPM;  
  float total = 0.0;
  for (int i = 0; i < 500; i++){
    total += beats[i];
  }
  BPM = int(total / 500);
  beat_old = beat_new;
  beatIndex = (beatIndex + 1) % 500;  
}

#include <ESP8266WiFi.h>
#include <PubSubClient.h>

const char* ssid = "gigante";
const char* password = "GG*98700";

const char* mqtt_server = "BROKER_IP";
const int mqtt_port = 1883;
const char* mqtt_user = "nicole";
const char* mqtt_password = "nicole123";    

WiFiClient espClient;
PubSubClient client(espClient);

void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Conectando a ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi conectado");
  Serial.println("Endereço IP: ");
  Serial.println(WiFi.localIP());
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Tentando conexão MQTT...");
    if (client.connect("ESP8266Client", mqtt_user, mqtt_password)) {
      Serial.println("conectado");
      client.subscribe("topico/teste");
    } else {
      Serial.print("falhou, rc=");
      Serial.print(client.state());
      Serial.println(" tentando novamente em 5 segundos");
      delay(5000);
    }
  }
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Mensagem recebida [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}

void setup() {
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  
  static unsigned long lastPublish = 0;
  if (millis() - lastPublish > 5000) {
    lastPublish = millis();
    String msg = "Olá do ESP8266";
    Serial.print("Publicando mensagem: ");
    Serial.println(msg);
    client.publish("topico/teste", msg.c_str());
  }
}
