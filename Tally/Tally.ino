#include <ETH.h>
#include <WiFi.h>
#include <WiFiUdp.h>
#include <M5Atom.h>
#include <FastLED.h>

uint8_t DisBuff[2 + 5 * 5 * 3];

// WiFi network name and password:
const char * networkName = "NetworkName";
const char * networkPswd = "NetworkPassword";
byte number = 0x00;
int counter = 0;
const bool _ = false;
const bool X = true;

//Are we currently connected?
boolean connected = false;
const unsigned int udpPort = 1052;
IPAddress multicastAddress(224, 100, 0, 1);
char packetBuffer[32];

//The udp library class
WiFiUDP udp;

// Variables for showing numbers
const int ledpin = 27;
const int numleds = 25;
int hue = 0;
CRGB leds[numleds];
const bool digits[] = {
  X,X,X,  _,X,_,  X,X,X,  X,X,X,  X,_,X,  X,X,X,  _,X,X,  X,X,X,  X,X,X,  X,X,X,
  X,_,X,  _,X,_,  _,_,X,  _,_,X,  X,_,X,  X,_,_,  X,_,_,  _,_,X,  X,_,X,  X,_,X,
  X,_,X,  _,X,_,  X,X,X,  X,X,X,  X,X,X,  X,X,X,  X,X,X,  _,X,_,  X,X,X,  X,X,X,
  X,_,X,  _,X,_,  X,_,_,  _,_,X,  _,_,X,  _,_,X,  X,_,X,  _,X,_,  X,_,X,  _,_,X,
  X,X,X,  _,X,_,  X,X,X,  X,X,X,  _,_,X,  X,X,X,  X,X,X,  _,X,_,  X,X,X,  X,X,_
};

void setup() {
  // Initilize hardware serial:
  Serial.begin(115200);

  M5.begin(true, false, true);

  delay(10);

  setBuff(0xf0, 0xf0, 0xf0);
  M5.dis.displaybuff(DisBuff);

  // Prepare leds
  FastLED.addLeds < WS2812B, ledpin, GRB > (leds, numleds);
  FastLED.setBrightness(20);
  FastLED.showColor(CRGB::Blue);
  
  //Connect to the WiFi network
  connectToWiFi(networkName, networkPswd);

  FastLED.showColor(CRGB::Black);
}

void loop() {

  // if there's data available, read the packet
  int packetSize = udp.parsePacket();
  if (packetSize)
  {
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
    if (!hasByte)
    {
      // Check for button presses to change number of Telly
      if (M5.Btn.wasPressed())
      {
        if (counter > 0) // button was pressed during counter, change number of this telly
        {
          if (number < 0x06)  {
            number++;
          }
          else                {
            number = 0x00;
          }
        }
        else // start counter and show current number
        {
          counter = 10;
          showNumber(number);
        }
      }
      else if(counter > 0)
      {
        counter--;
      }
      else
      {
        setBuff(0x00, 0x00, 0x00);
      }
    }

    M5.dis.displaybuff(DisBuff);
  }

  delay(50);
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

void showNumber(uint8_t number)
{
  int first = number / 10;
  int second = number % 10;
  for (int i = 0; i < numleds; i++) { leds[i] = CRGB::Black; }
  for (int y = 0; y < 5; y++)
  {
    if (first) { leds[y*5] = CHSV(0, 255, 255); }
    for (int x = 0; x < 3; x++) {
      if (digits[y*30 + second*3 + x]) { leds[y*5 + first + 1 + x] = CHSV(0,255,255); }
    }
  }
  FastLED.show();
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
      Serial.print("WiFi connected! IP address: ");
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
