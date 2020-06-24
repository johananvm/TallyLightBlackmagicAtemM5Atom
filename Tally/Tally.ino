#define DATA_PIN_LED 27

#include <ETH.h>
#include <WiFi.h>
#include <WiFiUdp.h>
#include <M5Atom.h>

uint8_t DisBuff[2 + 5 * 5 * 3];

// vars for changing number on clicking button
int ModuleNumber = 1;
const int PushButton = 39;
int ShowModuleNumber = 0;
bool ShowModuleNumberLoop = false;

// WiFi network name and password:
const char * networkName = "NETWORKNAME";
const char * networkPswd = "NETWORKPASSWORD";
byte number = 0x00;

//Are we currently connected?
boolean connected = false;
const unsigned int udpPort = 1052;
IPAddress multicastAddress(224, 100, 0, 1);
char packetBuffer[32];

//The udp library class
WiFiUDP udp;

void setup() {
  // Initilize hardware serial:
//  Serial.begin(115200);
  //Serial.println("Begin M5");
  M5.begin(true, false, true);

  //Serial.println("Delay 10");
  delay(10);

  setBuff(0xf0, 0xf0, 0xf0);
  M5.dis.displaybuff(DisBuff);

  Serial.begin(115200);

  delay(100);
  Serial.println("Connect to wifi");
  //Connect to the WiFi network
  connectToWiFi(networkName, networkPswd);
  while(!connected) {
    delay(200);
  }
  
  Serial.print("\nShow modulenumber");
  // show modulenumber
  ShowNumber(ModuleNumber);
}

void loop() {

  // if there's data available, read the packet
  int packetSize = udp.parsePacket();
  if (packetSize)
  {
    Serial.println("Package received");
    udp.read(packetBuffer, packetSize);

    // Read every availibe byte. Each byte contains a flag (pos 7), and a number (0-6).

    bool hasByte = false;
    for (int i = 0; i < packetSize; i++)
    {
      byte data = packetBuffer[i];

      // Camera in Preview
      if ((data & 0x7F) == number)
      {
        hasByte = true;
        if ((data & 0x80) == 0x80)
        {
          // Live
          setBuff(0xf0, 0x00, 0x00);
        }
        else
        {
          // Preview
          setBuff(0x00, 0x60, 0x00);
        }
      }
    }

    // If there is no matching byte with our number, then the leds will be turned of
    if (!hasByte && !ShowModuleNumberLoop)
    {
      Serial.println("No byte with our number, set display off");
      setBuff(0x00, 0x00, 0x00);
    }
    M5.dis.displaybuff(DisBuff);
  }

  if (ShowModuleNumberLoop) {
    if (ShowModuleNumber > 0) {
      // extract 200 from countdown and delay 200
      ShowModuleNumber = ShowModuleNumber - 200;
      delay(200);
    } else { // loop is finished, clear screen and set variables
      ShowModuleNumberLoop = false;
      setBuff(0x00, 0x00, 0x00);
      M5.dis.displaybuff(DisBuff);
    }
  }

  M5.Btn.read();
  if (M5.Btn.wasReleased()) {
    if (ShowModuleNumberLoop) { // push button when showing current number -> add 1 to number
      if (ModuleNumber == 9) { // can't go higher than 9
        ModuleNumber = 1;
      } else {
        ModuleNumber++;
      }
      // set number for use in handling UDP packages (starts at byte 0, so extract 1)
      number = (byte)(ModuleNumber - 1);
      ShowNumber(ModuleNumber);
      Serial.print("\nModulenumber changed to: ");
      Serial.print(ModuleNumber);
      // Reset loop counter
      ShowModuleNumber = 5000;
    } else {
      // Start loop
      ShowModuleNumber = 5000;
      ShowModuleNumberLoop = true;
      ShowNumber(ModuleNumber);
    }
  }

  //delay(50);
  M5.update();
}

// Connect to Wifi
void connectToWiFi(const char * ssid, const char * pwd) {
  Serial.println("Connecting to WiFi network: " + String(ssid));

  // delete old config
  WiFi.disconnect(true);
  //register event handler
  WiFi.onEvent(WiFiEvent);

  WiFi.mode(WIFI_STA); //station
  WiFi.setSleep(false);
  //wifi_set_sleep_type(NONE_SLEEP_T);

  //Initiate connection
  WiFi.begin(ssid, pwd);


  Serial.println("Waiting for WIFI connection...");
}

void setBuff(uint8_t Rdata, uint8_t Gdata, uint8_t Bdata)
{
  DisBuff[0] = 0x05;
  DisBuff[1] = 0x05;
  for (int i = 0; i < 25; i++)
  {
    DisBuff[2 + i * 3 + 0] = Rdata;
    DisBuff[2 + i * 3 + 1] = Gdata;
    DisBuff[2 + i * 3 + 2] = Bdata;
  }

  // Set connected dot
  byte value = 0x00;
  if (connected)
    value = 0x40;
  DisBuff[2] = value;
  DisBuff[3] = value;
  DisBuff[4] = value;
}

//wifi event handler
void WiFiEvent(WiFiEvent_t event) {
  switch (event) {
    case SYSTEM_EVENT_STA_GOT_IP:
      //When connected set
      Serial.println("WiFi connected! IP address: ");
      Serial.println(WiFi.localIP());
      //initializes the UDP state
      //This initializes the transfer buffer
      udp.beginMulticast(multicastAddress, udpPort);
      connected = true;
      setBuff(0x00, 0x00, 0xf0);
      M5.dis.displaybuff(DisBuff);
      break;
    case SYSTEM_EVENT_STA_DISCONNECTED:
      Serial.println("WiFi lost connection");
      setBuff(0xf0, 0x00, 0xf0);
      M5.dis.displaybuff(DisBuff);
      connected = false;
      break;
    default: break;
  }
}

void ShowNumber(int value)
{
  static int cubestartpoint[] = {0,0,0};
  static int cube[10][5][5]={{{0,1,1,1,0},
                              {1,1,0,1,1},
                              {1,0,0,0,1},
                              {1,1,0,1,1},
                              {0,1,1,1,0}},
                             {{0,0,1,1,0},
                              {0,1,0,1,0},
                              {1,0,0,1,0},
                              {0,0,0,1,0},
                              {1,1,1,1,1}},
                             {{1,1,1,1,0},
                              {0,0,1,1,1},
                              {0,1,1,0,0},
                              {1,1,0,0,0},
                              {1,1,1,1,1}},
                             {{1,1,1,1,1},
                              {0,0,0,0,1},
                              {1,1,1,1,1},
                              {0,0,0,0,1},
                              {1,1,1,1,1}},
                             {{0,0,0,0,1},
                              {0,0,1,1,1},
                              {0,1,1,0,1},
                              {1,1,1,1,1},
                              {0,0,0,1,0}},
                             {{1,1,1,1,1},
                              {1,0,0,0,0},
                              {1,1,1,1,1},
                              {0,0,0,0,1},
                              {1,1,1,1,1}},
                             {{1,1,1,1,1},
                              {1,0,0,0,0},
                              {1,1,1,1,1},
                              {1,0,0,0,1},
                              {1,1,1,1,1}},
                             {{1,1,1,1,1},
                              {0,0,0,1,1},
                              {0,0,1,1,0},
                              {0,1,1,0,0},
                              {1,1,0,0,0}},
                             {{1,1,1,1,1},
                              {1,0,0,0,1},
                              {1,1,1,1,1},
                              {1,0,0,0,1},
                              {1,1,1,1,1}},
                             {{1,1,1,1,1},
                              {1,0,0,0,1},
                              {1,1,1,1,1},
                              {0,0,0,0,1},
                              {1,1,1,1,1}},
                            };
  M5.dis.clear();
  for (int i = 0; i < 5; i++) {
    for (int j = 0; j < 5; j++) {
      int state = { cube[value][i][j] };
      if (state) {
        M5.dis.drawpix(j,i,CRGB::White);
      } else {
        M5.dis.drawpix(j,i,CRGB::Black);
      }
    } 
  }
}
