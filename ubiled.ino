#include <WiFi.h>

#include <SPI.h>

/********************************
 * Constants and objects
 *******************************/
namespace {
 char* WIFI_SSID = "dhdh"; // your network WIFI_SSID (name)
char* WIFI_PASSWORD = "asdfasdf"; // your network password
/* Assigns the Ubidots parameters */
char const * TOKEN = "A1E-lmx2XeCWuxmwyq1kik8PCms4BclM0R"; // Assign your Ubidots TOKEN
char const * DEVICE_LABEL = "edison";// Put here your Network password
  const char * SERVER = "things.ubidots.com";
   // Assign the device label to get the values of the variables
  const char * VARIABLE_LABEL = "led"; // Assign the variable label to get the last value
  const char * USER_AGENT = "ArduinoWifi";
  const char * VERSION = "1.0";
  const int PORT = 80;
  int status = WL_IDLE_STATUS;
  int LED = 4;
  // assign the pin where the led is connected
}


WiFiClient client;

/********************************
 * Auxiliar Functions
 *******************************/

/*
   this method makes a HTTP connection to the server
   and send request to get a data
*/

float getData(const char * variable_label) {
  /* Assigns the constans as global on the function */
  char* response; // Array to store parsed data
  char* serverResponse; // Array to store values
  float num;
  char resp_str[700]; // Array to store raw data from the server
  uint8_t j = 0;
  uint8_t timeout = 0; // Max timeout to retrieve data
  uint8_t max_retries = 0; // Max retries to make attempt connection

  /* Builds the request GET - Please reference this link to know all the request's structures https://ubidots.com/docs/api/ */
  char* data = (char *) malloc(sizeof(char) * 220);
  sprintf(data, "GET /api/v1.6/devices/%s/%s/lv", DEVICE_LABEL, variable_label);
  sprintf(data, "%s HTTP/1.1\r\n", data);
  sprintf(data, "%sHost: things.ubidots.com\r\n", data);
  sprintf(data, "%sUser-Agent: %s/%s\r\n", data, USER_AGENT, VERSION);
  sprintf(data, "%sX-Auth-Token: %s\r\n", data, TOKEN);
  sprintf(data, "%sConnection: close\r\n\r\n", data);

  /* Initial connection */
  client.connect(SERVER, PORT);
  
  /* Reconnect the client when is disconnected */
  while (!client.connected()) {
    Serial.println("Attemping to connect");
    if (client.connect(SERVER, PORT)) {
      break;
    }

    // Tries to connect five times as max
    max_retries++;
    if (max_retries > 5) {
      Serial.println("Could not connect to server");
      free(data);
      return NULL;
    }
    delay(5000);
  }

  /* Make the HTTP request to the server*/
  client.print(data);

  /* Reach timeout when the server is unavailable */
  while (!client.available() && timeout < 2000) {
    timeout++;
    delay(1);
    if (timeout >= 2000) {
      Serial.println(F("Error, max timeout reached"));
      client.stop();
      free(data);
      return NULL;
    }
  }

  /* Reads the response from the server */
  int i = 0;
  while (client.available()) {
    char c = client.read();
    //Serial.write(c); // Uncomment this line to visualize the response from the server
    if (c == -1) {
      Serial.println(F("Error reading data from server"));
      client.stop();
      free(data);
      return NULL;
    }
    resp_str[i++] = c;
  }

  /* Parses the response to get just the last value received */
  response = strtok(resp_str, "\r\n");
  while(response!=NULL) {
    j++;
    //printf("%s", response);
    response = strtok(NULL, "\r\n");
    if (j == 10) {
      if (response != NULL) {
        serverResponse = response;
      }
      j = 0;
    }
  }

  /* Converts the value obtained to a float */
  num = atof(serverResponse);
  free(data);
  /* Removes any buffered incoming serial data */
  client.flush();
  /* Disconnects the client */
  client.stop();
  /* Returns de last value of the variable */
  return num;
}

/* This methods print the wifi status */
void printWifiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}

/********************************
 * Main Functions
 *******************************/

void setup() {
  //Initialize serial and wait for port to open:
  pinMode(LED,OUTPUT);
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  // check for the presence of the shield:
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi shield not present");
    // don't continue:
    while (true);
  }

  String fv = WiFi.firmwareVersion();
  if (fv != "1.1.0") {
    Serial.println("Please upgrade the firmware");
  }

  // attempt to connect to Wifi network:
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(WIFI_SSID);
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    status = WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

    // wait 10 seconds for connection:
    delay(10000);
  }
  Serial.println("Connected to wifi");
  printWifiStatus();

}

void loop() {
  float value = getData(VARIABLE_LABEL);
  Serial.print("The value received form Ubidots is: ");
  Serial.println(value);
  if ( value == 1.0) {
    digitalWrite(LED, HIGH);   
  } else {
    digitalWrite(LED, LOW);
  }
  delay(1000);
}
