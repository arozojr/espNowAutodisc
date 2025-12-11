#include <esp_now.h>
#include <WiFi.h>

// REPLACE WITH YOUR RECEIVER MAC Address
uint8_t broadcastAddress[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

// Structure example to send data
// Must match the receiver structure
typedef struct struct_message {
  int id;
  char ssid[16];
  char pass[16];
  char readingId;
  int cmd;
  bool btn;
  float temp;
  float hum;
  //char mac[18];
} struct_message;

// Create a struct_message called myData
struct_message myData;
esp_now_peer_info_t peerInfo;
bool isConnected = false;
// callback when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}

void OnDataRecv(const uint8_t* mac, const uint8_t *inData, int len)
{
  memcpy(&myData, inData, sizeof(myData));
  Serial.println("Rec... ESP-NOW");
  Serial.print("Bytes rec:");
  Serial.println(len);
  Serial.print("Cmd: ");
  Serial.println(myData.cmd, HEX);

  if( myData.cmd == 0x55 )
  {
    Serial.println("Reply");
    memcpy(peerInfo.peer_addr, mac, 6);
    peerInfo.channel = 0;
    peerInfo.encrypt = false;
    if( esp_now_add_peer(&peerInfo) != ESP_OK )
    {
      Serial.println("Error add dev.");
    }
    Serial.println("Registred in server!");
    isConnected = true;      
  }
  else
  {
    Serial.print("Cmd not found");
  }

}
 
void setup() {
  // Init Serial Monitor
  Serial.begin(115200);
 
  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Once ESPNow is successfully Init, we will register for Send CB to
  // get the status of Trasnmitted packet
  esp_now_register_send_cb(OnDataSent);
  esp_now_register_recv_cb(esp_now_recv_cb_t(OnDataRecv));
  // Register peer
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;
  
  // Add peer        
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }
  Serial.println("");
  Serial.println("---------------------------");
  Serial.println("Initializing Client ESP-NOW");
}

void loop() {
  if( !digitalRead(0) && !isConnected)
  {
    /* Send broadcast message */
    myData.temp = random(25,30);
    myData.hum = random(50,100);
    myData.cmd = 0xAA;
    myData.readingId = 0;
    
    // Send message via ESP-NOW
    if (esp_now_send(broadcastAddress, (uint8_t *) &myData, sizeof(myData)) == ESP_OK) {
      Serial.println("Sent with success");
    }
    else {
      Serial.println("Error sending the data");
    }
    delay(2000);
  }
  if( isConnected )
  {
        /* Send broadcast message */
    myData.temp = random(25,30);
    myData.hum = random(50,100);
    myData.cmd = 0xBB;
    myData.readingId++;

    // Send message via ESP-NOW
    esp_err_t result = esp_now_send(peerInfo.peer_addr, (uint8_t *) &myData, sizeof(myData));
    
    if (result == ESP_OK) {
      Serial.println("Sent with success");
    }
    else {
      Serial.println("Error sending the data");
    }
    delay(50000);
  }
  delay(100);
}