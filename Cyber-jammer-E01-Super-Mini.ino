#include "RF24.h"
#include <SPI.h>
#include "esp_bt.h"
#include "esp_wifi.h"
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <GyverButton.h>
#include "GyverTimer.h"

#define BUTTON_LEFT 0
#define BUTTON_SELECT 1
#define BUTTON_RIGHT 2

#define BLUETOOTH_START 2
#define BLUETOOTH_END 80
#define BLUETOOTH_STEP 1

#define BLE_START 2
#define BLE_END 80
#define BLE_STEP 1

#define WIFI_START 12
#define WIFI_END 84
#define WIFI_STEP 20

#define ADVERTISING_START 37
#define ADVERTISING_END 39
#define ADVERTISING_STEP 1

#define MODE_COUNT 11


SPIClass *hp = nullptr;

// radio(CE, CS)
RF24 radio1(10, 7, 10000000);

const byte bluetooth_channels[] = {32, 34, 46, 48, 50, 52, 0, 1, 2, 4, 6, 8, 22, 24, 26, 28, 30, 74, 76, 78, 80};
const byte ble_channels[] = {2, 26, 80};
const byte wifi_channels[] = {10, 35, 60};

byte channel1 = 45;

GButton buttonLeft(BUTTON_LEFT);
GButton buttonSelect(BUTTON_SELECT);
GButton buttonRight(BUTTON_RIGHT);

GTimer myTimer(MS); 
Adafruit_SSD1306 display(128, 64, &Wire, -1);

byte mode = 1;
bool isClicked = false;

void initHP() {
  hp = new SPIClass(SPI);
  hp->begin();
  if (radio1.begin(hp)) {
    //Serial.println("HP Started !!!------------------------------------------------------------------------");
    radio1.setAutoAck(false);//Инфа о получении сигнала приёмником отключена
    radio1.stopListening();//Прекращение прослушивания вхордящих сигналов
    radio1.setRetries(0, 0);//Отключение задержки при не получении сигналов приёмником
    radio1.setPALevel(RF24_PA_MAX, true);//Усилитель PA на максимум
    radio1.setDataRate(RF24_2MBPS);//Скорость передачи данных
    radio1.setCRCLength(RF24_CRC_DISABLED);
    //radio1.printPrettyDetails();
    radio1.startConstCarrier(RF24_PA_MAX, channel1);
  } else {
    //Serial.println("HP couldn't start !!!");
  }
}

void jammingWifiExperiment2(){
  int randomIndex1 = random(0, sizeof(wifi_channels) / sizeof(wifi_channels[0]));
  channel1 = wifi_channels[randomIndex1];
  radio1.setChannel(channel1);
}

void jammingBluetoothExperiment2(){
  byte length = sizeof(bluetooth_channels) / sizeof(bluetooth_channels[0]);
  int randomIndex1 = random(0, length);
  channel1 = bluetooth_channels[randomIndex1];
  radio1.setChannel(channel1);
}

void jammingBLEExperiment2(){
  byte length = sizeof(ble_channels) / sizeof(ble_channels[0]);
  int randomIndex1 = random(0, length);
  channel1 = ble_channels[randomIndex1]; 
  radio1.setChannel(channel1);
}

void jammingWifiExperiment1(){
  int randomIndex = random(0, sizeof(wifi_channels) / sizeof(wifi_channels[0]));
  byte channel = wifi_channels[randomIndex]; 
  radio1.setChannel(channel);
}

void jammingBluetoothExperiment1(){
  int randomIndex = random(0, sizeof(bluetooth_channels) / sizeof(bluetooth_channels[0]));
  byte channel = bluetooth_channels[randomIndex]; 
  radio1.setChannel(channel);
}

void jammingBLEExperiment1(){
  int randomIndex = random(0, sizeof(ble_channels) / sizeof(ble_channels[0]));
  byte channel = ble_channels[randomIndex]; 
  radio1.setChannel(channel);
}

void jammingBluetooth(){
  for(byte i = BLUETOOTH_START; i < BLUETOOTH_END; i += BLUETOOTH_STEP){
    radio1.setChannel(i);
  }
}

void jammingBLE(){
  for(byte i = BLE_START; i < BLE_END; i += BLE_STEP){
    radio1.setChannel(i);
  }
}

void jammingWifi(){
  for(byte i = WIFI_START; i < WIFI_END; i += WIFI_STEP){
    radio1.setChannel(i);
  }
}

void jammingAdvertisong(){
  for(byte i = ADVERTISING_START; i < ADVERTISING_END; i += ADVERTISING_STEP){
    radio1.setChannel(i);
  }
}

void setup() {
  //Serial.begin(115200);

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.println("Nothing");
  display.display();

  myTimer.setTimeout(180);
  setCpuFrequencyMhz(240);
  esp_bt_controller_deinit();
  esp_wifi_stop();
  esp_wifi_deinit();
  esp_wifi_disconnect();

  initHP();

}

void loop() {
  isClicked = false;
  
  buttonLeft.tick();
  //buttonSelect.tick();
  buttonRight.tick();

  if(buttonRight.isClick()){
    isClicked = true;
    if(mode < MODE_COUNT){
      mode++;
    }
    else{
      mode = mode % MODE_COUNT + 1;
    }
  }

  if(buttonLeft.isClick()){
    isClicked = true;
    if(mode <= MODE_COUNT && mode > 1){
      mode--;
    }
    else{
      mode = MODE_COUNT;
    }
  }

  switch(mode){
  case 1:
    // display.println("Nothing");
    break;
  case 2:
    jammingBluetooth();
    break;
  case 3:
    jammingBLE();
    break;
  case 4:
    jammingWifi();
    break;
  case 5:
    jammingAdvertisong();
    break;
  case 6:
    jammingBluetoothExperiment1();
    break;
  case 7:
    jammingBLEExperiment1();
    break;
  case 8:
    jammingWifiExperiment1();
    break;
  case 9:
    jammingBluetoothExperiment2();
    break;
  case 10:
    jammingBLEExperiment2();
    break;
  case 11:
    jammingWifiExperiment2();
    break;
  }

  if(isClicked){
    //Serial.println(mode);
    display.setCursor(0, 0);
    display.clearDisplay();
    switch(mode){
    case 1:
      display.println("Nothing");
      break;
    case 2:
      display.println("Bluetooth");
      //Serial.println("jammingBluetooth");
      break;
    case 3:
      display.println("BLE");
      break;
    case 4:
      jammingWifi();
      display.println("WiFi");
      break;
    case 5:
      display.println("Advertisong");
      break;
    case 6:
      display.println("Bluetooth - 2");
      break;
    case 7:
      display.println("BLE - 2");
      break;
    case 8:
      display.println("WiFi - 2");
      break;
    case 9:
      display.println("Bluetooth - 3");
      break;
    case 10:
      display.println("BLE - 3");
      break;
    case 11:
      display.println("WiFI - 3");
      break;
    }
    display.display();
  }
}
