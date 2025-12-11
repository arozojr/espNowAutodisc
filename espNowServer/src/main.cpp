#include <esp_now.h>
#include <WiFi.h>
#include <FastLED.h>

#define LED_PIN 48
#define LED_NUMS 1
CRGB leds[LED_NUMS];

const int ledPin = 48;
const int red = 0;
const int green = 1;
const int blue = 2; 
void led(int color, bool state);

// Structure example to receive data
// Must match the sender structure
typedef struct struct_message {
    int id;
    char ssid[16];
    char password[16];
    int readingId;
    int cmd;
    bool btn;
    float temp;
    float hum;
    //char mac[18];
} struct_message;

// Create a struct_message called myData
struct_message myData;

void blinkLED(int times, int delayTime) {
  for (int i = 0; i < times; i++) {
    led(blue, true);
    delay(delayTime);
    led(blue, false);
    delay(delayTime);
  }
}

esp_now_peer_info_t peerInfo;

// REPLACE WITH YOUR RECEIVER MAC Address
uint8_t broadcastAddress[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

void printMAC(const uint8_t * mac_addr){
  char macStr[18];
  snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
           mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
  Serial.println(macStr);
}

// callback function that will be executed when data is received
void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  memcpy(&myData, incomingData, sizeof(myData));
  //const uint8_t *pMac = (uint8_t *)myData.mac;

  // Exibe o MAC address do remetente
  //Serial.print("Msg Rec. from MAC: ");
  //for (int i = 0; i < 6; i++) {
  //  Serial.printf("%02X", mac[i]);
  //  if (i < 5) Serial.print(":");
 // }
  printMAC(mac);
  Serial.print("Bytes rec: ");
  Serial.print(len);
  Serial.print(" | Id: ");
  Serial.print(myData.id);
  Serial.print(" | Cmd: ");
  Serial.print(myData.cmd, HEX);
  Serial.println();
  //Serial.printf("MAC Address: %02X:%02X:%02X:%02X:%02X:%02X\n",
  //              myData.mac[0], myData.mac[1], myData.mac[2], myData.mac[3], myData.mac[4], myData.mac[5]);
  delay(100);

  memcpy(peerInfo.peer_addr, mac, 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;

  /* Checa cadastro */
  if( myData.cmd == 0xAA )
  {
    myData.cmd = 0x55;
    if( esp_now_is_peer_exist(&peerInfo.peer_addr[0]) == false )
    {
      if (esp_now_add_peer(&peerInfo) != ESP_OK)
      {
        Serial.println("Failed to add peer");
        return;
      }
    }
    else
    {
      Serial.println("Peer already Add!");
    }
    esp_now_send(mac, (uint8_t *) &myData, sizeof(myData));
    Serial.println("Reg. dev.");  
  }
  else if (myData.cmd == 0xBB)
  {
    Serial.print("Upd. Data -> ");
    
    Serial.print(" | Reads: ");
    Serial.print(myData.readingId);
    Serial.print(" | Temp: ");
    Serial.print(myData.temp);
    Serial.print(" | Hum: ");
    Serial.println(myData.hum);
    Serial.println("");
  }
  else
  {
    Serial.println("Cmd not found!");
  }
}

void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("Last Packet Send Status: ");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
} 

void led(int color, bool state) {
  if (state == false) {
    leds[0] = CRGB(0, 0, 0); // Off
    FastLED.show();
  }
  else
  {
    switch (color) {
      case 0:
        leds[0] = CRGB(255, 0, 0); // Red
        break;
      case 1:
        leds[0] = CRGB(0, 255, 0); // Green
        break;
      case 2:
        leds[0] = CRGB(0, 0, 255); // Blue
        break;
      case 3:
        leds[0] = CRGB(255, 255, 0); // Yellow
        break;
      case 4:
        leds[0] = CRGB(0, 255, 255); // Cyan
        break;
      case 5:
        leds[0] = CRGB(255, 0, 255); // Magenta
        break;
      case 6:
        leds[0] = CRGB(255, 255, 255); // White
        break;
      default:
        leds[0] = CRGB(0, 0, 0); // Off
        break;
    }
    FastLED.show();
  }
}

void setup() {
  uint8_t mac[6];
  // Initialize Serial Monitor
  Serial.begin(115200);
  delay(1000);
  
  pinMode(ledPin, OUTPUT);
  pinMode(0, INPUT_PULLUP); // botão de BOOT em nível alto quando sol
  FastLED.addLeds<NEOPIXEL, LED_PIN>(leds, LED_NUMS);
  led(red, false);

  for( int x = 0; x <= 2; x++ ) {
      digitalWrite(ledPin, HIGH);
      delay(200);
      digitalWrite(ledPin, LOW);
      delay(200);
  }
  Serial.println("-------------------------------------");
  Serial.println("Initializing..");

  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);
  //WiFi.begin();
  
  //WiFi.macAddress(mac);
  //Serial.printf("MAC Address: %02X:%02X:%02X:%02X:%02X:%02X\n",
  //              mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
    // Register peer
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;
  
  esp_now_register_send_cb(OnDataSent);
  esp_now_register_recv_cb(esp_now_recv_cb_t(OnDataRecv));

    // Add peer        
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }

}
 
void loop() {
  esp_now_peer_num_t pn;
  esp_now_peer_info_t slave;
  static unsigned long lastEventTime = millis();
  static const unsigned long EVENT_INTERVAL_MS = 2000;
  if ((millis() - lastEventTime) > EVENT_INTERVAL_MS) {
    blinkLED(1, 100);
    lastEventTime = millis();
    Serial.println("Status Server:");
    
    esp_now_get_peer_num(&pn);
    Serial.print("Total Peer Count: ");
    Serial.println(pn.total_num-1);
    
      //output peer list
    Serial.print("Current Peer List: ");
    Serial.println("");

if((pn.total_num) > 0){        
    boolean from_head = true;
    int i = 1;
    while (esp_now_fetch_peer(from_head, &slave) == ESP_OK){
        from_head = false;
        Serial.print("MAC ADDRESS ");
        Serial.print(i);
        Serial.print(": ");
        printMAC(slave.peer_addr);
        i++;
    }      
}
  led(red, false);
  }
  else if(!digitalRead(0)){
    led(red, true);
    Serial.println("Sent data...");
    esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &myData, sizeof(myData));
    if (result == ESP_OK) {
      Serial.println("Sent with success");
    }
    else {
      Serial.println("Error sending the data");
    }
    delay(1000);
  }
  else 
  {
    delay(10);
  }
}