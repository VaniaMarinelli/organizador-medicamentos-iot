#include <WiFi.h>
#include <PubSubClient.h>
#include <time.h>

const int pinoLDR = 34;     
const int pinoBotao = 33;   
const int pinoLED = 2;     
const int pinoBuzzer = 4;   

const char* ssid = "Wokwi-GUEST"; 
const char* password = "";

const char* mqtt_server = "broker.hivemq.com";
const int mqtt_port = 1883;

WiFiClient espClient;
PubSubClient client(espClient);

bool alarmeAtivo = false;
bool caixaAberta = false;
int limiteLuz = 2000; 

unsigned long ultimoTempoBotao = 0;
unsigned long tempoDebounce = 250;

const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = -10800; 
const int   daylightOffset_sec = 0;

int horaRemedio = 15;
int minutoRemedio = 30;
bool remedioTomadoHoje = false;

void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("A ligar à rede Wi-Fi: ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWi-Fi ligado com sucesso!");
  Serial.print("Endereço IP: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
  String mensagem = "";
  for (int i = 0; i < length; i++) {
    mensagem += (char)payload[i];
  }
  
  Serial.print("Mensagem recebida no tópico [");
  Serial.print(topic);
  Serial.print("]: ");
  Serial.println(mensagem);

  if (String(topic) == "medicamento/comando" && mensagem == "acionar") {
    Serial.println("Comando remoto recebido: Forçando alarme!");
    acionarAlarme();
  }
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("A tentar ligação MQTT...");
    String clientId = "ESP32Client-Vania-";
    clientId += String(random(0, 1000));
    
    if (client.connect(clientId.c_str())) {
      Serial.println("Ligado ao Broker MQTT!");
      
      client.subscribe("medicamento/comando");
    } else {
      Serial.print("Falhou, rc=");
      Serial.print(client.state());
      Serial.println(" Tentando novamente em 5 segundos...");
      delay(5000);
    }
  }
}

void acionarAlarme() {
   alarmeAtivo = true;
   digitalWrite(pinoLED, HIGH);
   tone(pinoBuzzer, 1000); 

   client.publish("medicamento/alerta", "Alarme acionado! Hora de tomar a medicacao.");
   Serial.println("ALERTA: É hora do medicamento!");
}

void pararAlarme() {
   alarmeAtivo = false;
   digitalWrite(pinoLED, LOW);
   noTone(pinoBuzzer); 

   client.publish("medicamento/confirmacao", "Medicamento tomado. Alarme desligado pela utilizadora.");
   Serial.println("CONFIRMAÇÃO: Medicamento tomado.");
}


void setup() {
  Serial.begin(115200);

  pinMode(pinoBotao, INPUT_PULLUP);
  pinMode(pinoLED, OUTPUT);
  pinMode(pinoBuzzer, OUTPUT);

  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);

  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  Serial.println("A sincronizar o relógio online...");
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  struct tm timeinfo;
  if (getLocalTime(&timeinfo)) {
    if (timeinfo.tm_hour == horaRemedio && timeinfo.tm_min == minutoRemedio && !remedioTomadoHoje) {
      acionarAlarme();
      remedioTomadoHoje = true; 
    }
    if (timeinfo.tm_hour == 0 && timeinfo.tm_min == 0) {
      remedioTomadoHoje = false; 
    }
  }

  int leituraLDR = analogRead(pinoLDR);
 
  if (leituraLDR < limiteLuz && !caixaAberta) {
    caixaAberta = true;
    client.publish("medicamento/abertura", "Atencao: Compartimento de remedio foi aberto!");
    Serial.println("SENSOR: Abertura detetada no compartimento.");
    delay(500); 
  } 
  else if (leituraLDR >= limiteLuz && caixaAberta) {
    caixaAberta = false; 
  }

  int estadoBotao = digitalRead(pinoBotao);
  if (estadoBotao == LOW) { 
    if ((millis() - ultimoTempoBotao) > tempoDebounce) {
      if (alarmeAtivo) {
        pararAlarme();
      }
      ultimoTempoBotao = millis();
    }
  }

  delay(10); // <--- O RESPIRO ENTRA EXATAMENTE AQUI
}
