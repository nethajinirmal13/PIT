#include <WiFi.h>
#include <SPI.h>

/********************************
 * Constants and objects
 *******************************/
/* Assigns the network parameters */
char* WIFI_SSID = "dhdh"; // your network WIFI_SSID (name)
char* WIFI_PASSWORD = "asdfasdf"; // your network password
/* Assigns the Ubidots parameters */
char const * TOKEN = "A1E-lmx2XeCWuxmwyq1kik8PCms4BclM0R"; // Assign your Ubidots TOKEN
char const * DEVICE_LABEL = "edison"; // Assign the unique device label 
char const * VARIABLE_LABEL_1 = "temperature"; // Assign the unique variable label to publish data to Ubidots (1) 
 // Assign the unique variable label to publish data to Ubidots (2)

/* Parameters needed for the requests */
char const * USER_AGENT = "EDISON";
char const * VERSION = "1.0";
char const * SERVER = "things.ubidots.com";
int PORT = 80;
char topic[700];
char payload[300];

/* initialize the library instance */
WiFiClient client;

/********************************
 * Auxiliar Functions
 *******************************/

void SendToUbidots(char* payload) {

  int i = strlen(payload); 
  /* Builds the request POST - Please reference this link to know all the request's structures https://ubidots.com/docs/api/ */
  sprintf(topic, "POST /api/v1.6/devices/%s/?force=true HTTP/1.1\r\n", DEVICE_LABEL);
  sprintf(topic, "%sHost: things.ubidots.com\r\n", topic);
  sprintf(topic, "%sUser-Agent: %s/%s\r\n", topic, USER_AGENT, VERSION);
  sprintf(topic, "%sX-Auth-Token: %s\r\n", topic, TOKEN);
  sprintf(topic, "%sConnection: close\r\n", topic);
  sprintf(topic, "%sContent-Type: application/json\r\n", topic);
  sprintf(topic, "%sContent-Length: %d\r\n\r\n", topic, i);
  sprintf(topic, "%s%s\r\n", topic, payload);
  
  /* Connecting the client */
  client.connect(SERVER, PORT); 

  if (client.connected()) {
    /* Sends the request to the client */
    client.print(topic);
    Serial.println("Connected to Ubidots - POST");
  } else {
    Serial.println("Connection Failed ubidots - Try Again"); 
  }  
    
  /* Reads the response from the server */
  while (client.available()) {
    char c = client.read();
    //Serial.print(c); // Uncomment this line to visualize the response on the Serial Monitor
  }

  /* Disconnecting the client */
  client.stop();
}

/********************************
 * Main Functions
 *******************************/
 
void setup() {
  Serial.begin(115200);
  Serial.print("Connecting to ");
  Serial.println(WIFI_SSID); 
  /* Connects to AP */
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
    
  Serial.println(F("WiFi connected"));
  Serial.println(F("IP address: "));
  Serial.println(WiFi.localIP());
}

void loop() {
  /* Reads sensors values */
  float temperature = analogRead(A0);
  
  
  /* Builds the payload - {"temperature":25.00,"humidity":50.00} */
  sprintf(payload, "{\"");
  sprintf(payload, "%s%s\":%f", payload, VARIABLE_LABEL_1, temperature);
  
  sprintf(payload, "%s}", payload);
  
  /* Calls the Ubidots Function POST */
  SendToUbidots(payload);
  /* Prints the data posted on the Serial Monitor */
  Serial.println("Posting data to Ubidots");
  Serial.print("Temperature: ");
  Serial.println(temperature);
  

  delay(5000); 
}
