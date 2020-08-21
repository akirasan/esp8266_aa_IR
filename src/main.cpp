// Dispositivo para enviar via infrarrojos códigos RAW
// Capturados previsamente mediante IRrecvDumpV2.ino
// @akirasan Agosto 2020

#include <Arduino.h>
#include <IRremoteESP8266.h>
#include <IRsend.h>

#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#include <Adafruit_NeoPixel.h>

// Configuración WiFi y servidor MQTT
#include "configuration.h" // Configura tus datos de conexión

#define topic_hola "casa/aire/habitacion"             // "Hola" al conectar al servidor MQTT
#define topic_escenario "casa/aire/habitacion/estado" // Enviamos estado/escenario pregrabado
#define topic_codigoIR "casa/aire/habitacion/codIR"   // Enviamos una array RAW con el código que queremos emitir por IR

WiFiClient espClient;
PubSubClient client(espClient);

const uint16_t PIN_LED_IR = 3; // ESP8266 GPIO pin to use. ESP8266-01 GPIO3 / RX
                               // Si quieres usar el GPIO2 recuerda montar con un transistor 2N222 por ejemplo
IRsend irsend(PIN_LED_IR);
int khz = 38;

// LED WS2812 - Ayuda visual para estado del dispositivo
#define PIN 0
#define NUMPIXELS 1
Adafruit_NeoPixel led_RGB(NUMPIXELS, PIN, NEO_RGB + NEO_KHZ800);

// Datos capturados con IRrecvDumpV2.ino
// Estos serán los escenarios previamente capturados y configurados
uint16_t AUTO27[] = {2950, 3100, 3000, 4400, 500, 1700, 550, 550, 500, 1700, 550, 550, 550, 550, 550, 1650, 550, 550, 550, 1650, 550, 1650, 550, 550, 550, 1650, 550, 1650, 550, 550, 550, 550, 550, 1650, 550, 1650, 550, 550, 550, 550, 550, 1650, 550, 1700, 500, 600, 500, 1700, 500, 1700, 500, 1650, 550, 600, 500, 600, 500, 1700, 550, 550, 500, 600, 500, 600, 550, 550, 550, 1650, 500, 600, 500, 600, 500, 600, 550, 550, 550, 1650, 550, 1650, 550, 550, 550, 550, 550, 550, 550, 550, 550, 550, 550, 600, 550, 550, 500, 600, 500, 600, 500, 600, 500, 600, 500, 1700, 500, 600, 500, 600, 550, 550, 550, 550, 550, 550, 550, 550, 550, 550, 550, 550, 550, 550, 550, 550, 550, 550, 550, 600, 500, 600, 500, 600, 500, 1700, 500, 1700, 500, 1700, 500, 1700, 500, 1700, 500, 1700, 550, 550, 550, 550, 550};

uint16_t FAN1[] = {950, 3100, 2950, 4450, 500, 1700, 500, 600, 500, 1700, 500, 600, 500, 600, 500, 1700, 500, 600, 500, 1700, 500, 1700, 500, 600, 500, 600, 500, 600, 550, 550, 550, 600, 500, 1700, 500, 1650, 550, 550, 550, 600, 500, 1700, 500, 600, 500, 1700, 500, 1700, 500, 600, 500, 1700, 500, 600, 500, 600, 500, 1700, 500, 600, 500, 1700, 500, 1700, 500, 600, 500, 600, 500, 600, 500, 600, 500, 600, 500, 600, 550, 1650, 550, 1700, 500, 600, 500, 600, 500, 1700, 500, 1700, 500, 600, 500, 600, 500, 600, 500, 600, 500, 600, 500, 600, 500, 1700, 500, 600, 500, 600, 500, 600, 500, 600, 500, 600, 550, 550, 550, 550, 550, 550, 550, 600, 500, 600, 500, 600, 500, 600, 500, 600, 500, 600, 500, 600, 500, 1700, 500, 1700, 500, 600, 500, 600, 500, 1700, 500, 1700, 500, 600, 550, 1650, 500};
uint16_t FAN2[] = {2950, 3100, 2950, 4450, 500, 1700, 500, 600, 500, 1700, 500, 600, 500, 600, 500, 1700, 500, 600, 500, 1700, 500, 1700, 500, 600, 500, 600, 500, 600, 500, 600, 500, 600, 550, 1700, 500, 1650, 500, 600, 500, 600, 550, 1700, 500, 600, 500, 1700, 500, 1700, 500, 600, 500, 1700, 500, 600, 500, 600, 500, 1700, 500, 600, 500, 1700, 500, 1700, 500, 600, 500, 1700, 500, 600, 500, 600, 500, 600, 500, 600, 500, 1700, 500, 1700, 500, 600, 550, 550, 500, 1700, 550, 550, 550, 600, 500, 600, 500, 600, 500, 600, 500, 600, 500, 600, 500, 1700, 500, 600, 500, 600, 500, 600, 500, 600, 500, 600, 500, 600, 500, 600, 500, 600, 500, 600, 500, 600, 550, 600, 500, 600, 500, 600, 500, 600, 500, 600, 500, 1700, 500, 600, 500, 600, 500, 600, 500, 1700, 500, 1700, 500, 1700, 500, 600, 500};
uint16_t FAN3[] = {2950, 3050, 2950, 4450, 550, 1700, 500, 600, 500, 1700, 500, 600, 500, 600, 500, 1700, 500, 600, 550, 1650, 500, 1700, 500, 600, 500, 600, 500, 600, 500, 600, 500, 600, 500, 1700, 500, 1700, 550, 550, 550, 550, 550, 1650, 550, 550, 500, 1700, 500, 1700, 550, 600, 500, 1650, 550, 550, 500, 600, 500, 1700, 550, 600, 500, 1700, 500, 1700, 500, 600, 500, 1700, 500, 600, 500, 600, 500, 600, 500, 600, 500, 1700, 500, 1700, 500, 600, 500, 600, 500, 600, 500, 1700, 500, 600, 550, 550, 500, 600, 500, 600, 550, 600, 500, 550, 550, 1700, 500, 600, 500, 600, 500, 600, 500, 600, 500, 600, 500, 600, 500, 600, 500, 600, 500, 600, 500, 600, 500, 600, 500, 600, 500, 600, 550, 550, 550, 550, 550, 550, 550, 1650, 550, 600, 500, 600, 500, 1700, 500, 1700, 500, 1700, 500, 600, 500};

uint16_t OFF[] = {2950, 3100, 2950, 4450, 500, 1700, 500, 600, 550, 1650, 500, 600, 500, 600, 500, 1700, 500, 600, 500, 1700, 500, 1700, 500, 600, 500, 600, 500, 600, 500, 600, 550, 550, 550, 600, 500, 550, 550, 600, 500, 600, 500, 1700, 500, 600, 500, 1700, 500, 1700, 500, 600, 500, 1700, 500, 600, 500, 600, 500, 1700, 500, 600, 500, 1700, 500, 1700, 500, 1700, 500, 600, 500, 600, 500, 600, 550, 550, 500, 600, 550, 1700, 500, 1700, 500, 600, 500, 600, 500, 1700, 500, 1700, 500, 600, 500, 600, 500, 600, 500, 600, 550, 550, 500, 600, 500, 1700, 500, 600, 550, 550, 500, 600, 500, 600, 550, 600, 500, 600, 500, 600, 500, 600, 500, 600, 500, 600, 500, 600, 500, 600, 550, 550, 500, 600, 500, 600, 500, 1700, 500, 1700, 550, 550, 500, 600, 550, 1650, 500, 1700, 550, 550, 550, 600, 500};

void led_parpadeo(byte R, byte G, byte B)
{
  for (byte i = 0; i < 3; i++)
  {
    led_RGB.setPixelColor(0, led_RGB.Color(R, G, B));
    led_RGB.show();
    delay(100);
    led_RGB.setPixelColor(0, led_RGB.Color(0, 0, 0));
    led_RGB.show();
    delay(50);
  }
  led_RGB.clear();
}

void setup_wifi()
{
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED)
  {
    led_parpadeo(0, 0, 255);
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

// Lectura de mensajes MQTT suscritos
void mensaje_recibido(char *topic, byte *payload, unsigned int length)
{

  led_parpadeo(0, 00, 50);

  Serial.print("Mensaje recibido [");
  Serial.print(topic);
  Serial.print("] ");

  String topicStr = topic;

  if (topicStr == topic_escenario)
  {

    String payloadStr;
    for (unsigned int i = 0; i < length; i++)
    {
      payloadStr += (char)payload[i];
    }

    if (payloadStr == "AUTO27")
    {
      irsend.sendRaw(AUTO27, sizeof(AUTO27) / sizeof(uint16_t), khz); // Send a raw data capture at 38kHz.
    }

    if (payloadStr == "OFF")
    {
      irsend.sendRaw(OFF, sizeof(OFF) / sizeof(uint16_t), khz); // Send a raw data capture at 38kHz.
    }

    if (payloadStr == "FAN1")
    {
      irsend.sendRaw(FAN1, sizeof(FAN1) / sizeof(uint16_t), khz); // Send a raw data capture at 38kHz.
    }

    if (payloadStr == "FAN2")
    {
      irsend.sendRaw(FAN2, sizeof(FAN2) / sizeof(uint16_t), khz); // Send a raw data capture at 38kHz.
    }

    if (payloadStr == "FAN3")
    {
      irsend.sendRaw(FAN3, sizeof(FAN3) / sizeof(uint16_t), khz); // Send a raw data capture at 38kHz.
    }
  }

  if (topic == topic_codigoIR)
  {
    // Pendiente implementación
  }
}

// Conexión al servidor MQTT
void conectarMQTT()
{
  // Loop hasta conectarnos a MQTT server y publicar/suscribir a mensajes
  while (!client.connected())
  {
    led_parpadeo(55, 0, 0);

    Serial.print("MQTT connection...");
    // Creamos un nombre de cliente aleatorio a partir de este prefijo
    String clientId = "ESP8266_AireA-";
    clientId += String(random(0xffff), HEX);
    // Intentamos conectarnos al servidor MQTT
    if (client.connect(clientId.c_str()))
    {
      Serial.println("conectado!!!");
      // Una vez conectado publicamos un mensaje de "HOLA!!!! estoy aquí!!!" al topic
      client.publish(topic_hola, "ON");
      // ...y nos suscribimos a los mensajes que queremos leer
      client.subscribe(topic_escenario); //Enviamos estado/escenario pregrabado
      client.subscribe(topic_codigoIR);  //Enviamos una array RAW con el código que queremos emitir por IR
      led_parpadeo(0, 55, 0);
    }
    else
    {
      led_RGB.setPixelColor(0, led_RGB.Color(255, 0, 0));
      led_RGB.show();
      Serial.print("error, rc=");
      Serial.print(client.state());
      Serial.println(" ...5 segundos (reintento)");
      delay(5000);
    }
  }
}

void setup() 
{
  pinMode(PIN, OUTPUT);
  pinMode(PIN_LED_IR, OUTPUT);

  digitalWrite(PIN, LOW);
  digitalWrite(PIN_LED_IR, LOW);

  irsend.begin();
  Serial.begin(115200, SERIAL_8N1, SERIAL_TX_ONLY);

  led_RGB.begin(); //LED WS2812
  led_RGB.clear();

  // Configuación conexión WiFi
  setup_wifi();

  // Configuración conexión server MQTT y mensajes
  client.setServer(mqtt_server, mqtt_server_port);
  client.setCallback(mensaje_recibido);
}

void loop()
{
  // Verificamos si estamos conectados al servidor MQTT
  if (!client.connected())
  {
    conectarMQTT(); // Reintentos de conexión
  }
  client.loop(); // Por si hemos recibido algún mensaje de los suscritos
  led_RGB.clear();

  // Aquí cualquier otra cosa que quieras hacer...
}
