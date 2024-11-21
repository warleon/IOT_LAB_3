#include <WiFi.h>
#include <PubSubClient.h>

const char* ssid = "Warleon wifi";       // Cambia por el nombre de tu red Wi-Fi
const char* password = "12345678";       // Cambia por la contraseña de tu red Wi-Fi
const char* mqttServer = "192.168.229.226"; // Dirección IP de tu broker Mosquitto
const int mqttPort = 1883;               // Puerto de Mosquitto (por defecto es 1883)
const char* mqttUser = "";               // Usuario MQTT (si aplica)
const char* mqttPassword = "";           // Contraseña MQTT (si aplica)

WiFiClient espClient;
PubSubClient client(espClient);

int pinAnalogico = 36;  // GPIO36 que corresponde a ADC0

void setup() {
  Serial.begin(115200);
  
  // Conectar al Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Conectando al WiFi...");
  }
  Serial.println("Conectado al WiFi");

  // Configurar el cliente MQTT
  client.setServer(mqttServer, mqttPort);

  // Conectar al broker MQTT
  while (!client.connected()) {
    Serial.println("Conectando al broker MQTT...");
    if (client.connect("ESP32Client", mqttUser, mqttPassword)) {
      Serial.println("Conectado al broker MQTT");
    } else {
      Serial.print("Error de conexión: ");
      Serial.print(client.state());
      delay(2000);
    }
  }
}

void loop() {
  if (!client.connected()) {
    // Reconectar si se desconecta
    while (!client.connected()) {
      Serial.println("Reconectando al broker MQTT...");
      if (client.connect("ESP32Client", mqttUser, mqttPassword)) {
        Serial.println("Reconectado al broker MQTT");
      } else {
        Serial.print("Error de reconexión: ");
        Serial.print(client.state());
        delay(2000);
      }
    }
  }

  client.loop(); // Mantener la conexión al broker MQTT

  // Leer el valor de humedad del sensor
  int valorHumedad = analogRead(pinAnalogico); // Lee el valor analógico (0-4095)
  int porcentajeHumedad = map(valorHumedad, 0, 4095, 100, 0); // Convertir a porcentaje (0-100)

  // Publicar el valor en el tema MQTT
  char msg[50];
  snprintf(msg, 50, "Humedad del suelo: %d%%", porcentajeHumedad);
  client.publish("test/topic", msg);

  Serial.print("Humedad del suelo: ");
  Serial.print(porcentajeHumedad);
  Serial.println("%");

  delay(2000); // Espera 2 segundos antes de la próxima lectura
}