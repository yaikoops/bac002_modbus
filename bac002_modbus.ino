/*
  0. 목표
  0.0 실외온도조절기(BAC-002), 실외 조도센서, 전력측정기(sdm120)
      에서 제공하는 정보를 mtqq에 publish한다. (bac002 ok, 조도센서 ok, sdm120 ok)
      mtqq에서 읽은 정보를 mysql에 저장한다. (bac002 ok, 조도센서 ok, sdm120 ok)
  0.1 실외온도기 기능 (BAC-002)
     - aduino) BAC-002에서 제공하는 파워설정/모드설정/온도설정/팬설정/LOCK설정/시간설정/분설정/요일설정/Room온도값/팬설정값/valve설정값
               을 ModbusRTU(485)통신을 활용해서 값을 읽는다. OK
     - aduino) BAC-002에서 읽은 값을 mtqq broker에 publish한다. OK
     - node-red) mqtt broker에서 publish된 값을 mysql에 저장한다. OK
     - node-red) BAC-002에 파워설정/모드설정/온도설정/팬설정/LOCK설정/시간설정/분설정/요일설정을 화면을 통해 설정한다. OK
  0.2 조도센서 기능
     - aduino) 조도센서의 값을 읽는다. OK
     - aduino) 조도센서에서 읽은 값을 mtqq broker에 publish한다. OK
       . NTP연동을 통해 현재일자와 시간을 읽는다. OK
       . 월별 일조시간/습도는 아래를 따른다. (일조량참고, https://m.blog.naver.com/hajunggu/220744317702)(진행중)
                                        (습도참고, http://www.climate.go.kr/home/09_monitoring/info/download_data?oneDepth=4&twoDepth=23&threeDepth=78&4d=Vapor:1:5)
       .. 1월 7시41분 ~ 17시39분 9:59   63%
       .. 2월 7시18분 ~ 18시11분 10:53  62%
       .. 3월 6시42분 ~ 18시38분 11:55  62%
       .. 4월 5시57분 ~ 19시04분 13:47  61%
       .. 5월 5시24분 ~ 19시30분 14:06  65%
       .. 6월 5시12분 ~ 19시49분 14:37  73%
       .. 7월 5시24분 ~ 19시48분 14:24  80%
       .. 8월 5시48분 ~ 19시21분 13:33  78%
       .. 9월 6시12분 ~ 18시38분 12:26  76%
       .. 10월 6시37분 ~ 17시55분 11:18  70%
       .. 11월 7시07분 ~ 17시22분 10:15  67%
       .. 12월 7시34분 ~ 17시16분 9:42   65%
       . 조도값이 1000 이하인 경우 led를 on하고 dimming을 20으로 설정한다. 진행중:dimming
       . 조도값이 500 이하인 경우 led를 on하고 dimming을 60으로 설정한다. 진행중:dimming
       . 조도값이 300 이하인 경우 led를 on하고 dimming을 100으로 설정한다. 진행중:dimming
       . 조도값이 1000 이상인 경우 led를 off한다. OK
  0.3 전력측정기 기능
     - aduino) sdm120에서 제공하는 Voltage(Volts)/Current(Amps)/Active Power(Watts)/Apparent Power(VoltAmps)/
               Reactive Power(VAr)/Power Factor()/Phase Angle(Degrees)/Frequency(Hz)/Import Active Energy(kWh)/
               Export Active Energy(kWh)/Import Reactive Energy(kVArh)/Export Reactive Energy(kVArh)/
               otal Active Energy (kWh)/Total Reactive Energy(kVArh) 설정값
               을 ModbusRTU(485)통신을 활용해서 값을 읽는다. OK
     - aduino) BAC-002에서 읽은 값을 mtqq broker에 publish한다. OK
     - node-red) mqtt broker에서 publish된 값을 mysql에 저장한다. 진행중
     - node-red) BAC-002에 파워설정/모드설정/온도설정/팬설정/LOCK설정/시간설정/분설정/요일설정을 화면을 통해 설정한다. 진행중
  
  1.1 BAC02온도센서 MODBUS RTU MAPPING
  1.1.1 funtion code : 03 
    - 40001/setting power : high=00, low=off(00)/on(01)
    - 40002/setting fan   : high=00, low=auto speed(00)/high speed(01)/mid speed(02)/low speed(03)
    - 40003/setting mode  : high=00, low=cooling(00)/heating(01)/ventilation(02)
    - 40004/setting temp  : setting temp * 10
    - 40005/setting lock  : high=00, low=unlock(00)/lock(01)
    - 40006/minute        : high=00, low=01 ~ 59
    - 40007/hour          : high=00, low=01 ~ 23
    - 40008/week          : high=00, low=monday(01),tuesday(02),wednesday(03),thursday(04),friday(05),saturday(06),sunday(07)
    - 40009/room temp     : high=00, low=data is hex code
    - 40010/heater status : high=00, low=off(00)/on(01)
    - 40011/fan status    : high=00, low=auto speed(00)/high speed(01)/mid speed(02)/low speed(03)
  1.1.2 funtion code : 06 
    - 40001/setting power : high=00, low=off(00)/on(01)
    - 40002/setting fan   : high=00, low=auto speed(00)/high speed(01)/mid speed(02)/low speed(03)
    - 40003/setting mode  : high=00, low=cooling(00)/heating(01)/ventilation(02)
    - 40004/setting temp  : setting temp * 10
    - 40005/setting lock  : high=00, low=unlock(00)/lock(01)
    - 40006/minute        : high=00, low=01 ~ 59
    - 40007/hour          : high=00, low=01 ~ 23
    - 40008/week          : high=00, low=monday(01),tuesday(02),wednesday(03),thursday(04),friday(05),saturday(06),sunday(07)
  1.2 SDM120 전력측정센서 MODBUS RTU MAPPING
  1.2.1 funtion code : 04 
    - 30001/Voltage(Volts) : high=00, low=00
    - 30007/Current(Amps)  : high=00, low=06
    - 30013/Active Power(Watts) : high=00, low=0C
    - 30019/Apparent Power(VoltAmps) : high=00, low=12
    - 30025/Reactive Power(VAr) : high=00, low=18
    - 30031/Power Factor() : high=00, low=1E
    - 30037/Phase Angle(Degrees) : high=00, low=24
    - 30071/Frequency(Hz) : high=00, low=46
    - 30073/Import Active Energy(kWh) : high=00, low=48
    - 30075/Export Active Energy(kWh) : high=00, low=4A
    - 30077/Import Reactive Energy(kVArh) : high=00, low=4C
    - 30079/Export Reactive Energy(kVArh) : high=00, low=4E
    - 30343/Total Active Energy(kWh) : high=01, low=56
    - 30345/Total Reactive Energy(kVArh) : high=01, low=58
  2, pin mapping
    - max485-WEMOS R1/D1 
      . BAC002 : RO-D6(RX), DI-D7(TX), DE/RE-D4/D5, VCC-3.3V, GND-GND
      . SDM120 : RO-D8(RX), DI-D9(TX), DE/RE-D2/D3, VCC-3.3V, GND-GND
      . MAX485의 VCC를 반드시 3.3V에 연결해야한다. 5V연결시 Respinse timeout이 발생함
    - 조도센서-WEMOS R1/D1
      . S-A0, + 5V, - GND 
    - relay-WEMOS R1 D1 : IN1-D11, IN2=D12, IN3=D13, IN4=D14
    - relay-기기
      . IN1-led(AC220v), IN2=n/a , IN3=PUMP(12v), IN4=ledFAN(12v)
    - dimmer-WEMOS R1/D1 : ?
*/

#include <ModbusMaster.h>    // BAC-002 Modbus rtu
#include <SoftwareSerial.h>  // SHT20 Modbus rtu
#include <ESP8266WiFi.h>     // mtqq PUBSUB 
#include <PubSubClient.h>    // mtqq PUBSUB
#include <NTPClient.h>       // ntp client
#include <WiFiUdp.h>         // ntp client

#define MODBUS_RATE            9600      // Modbus speed of BAC-002, do not change

// bac002_MODBUS RTU 기본값 세팅
#define bac002_MAX485_DE       D4        // Driver /in/output Enable pin DE/RE Active HIGH
#define bac002_MAX485_RE       D5        // Driver /in/output Enable pin DE/RE Active HIGH
#define bac002_SLAVE_ID        2         // Default slave ID of BAC-002
#define bac002_SSERIAL_RX_PIN  D6        // SOFTWARE SERIAL RX
#define bac002_SSERIAL_TX_PIN  D7        // SOFTWARE SERIAL TX                                                                                                    

// smd120_MODBUS RTU 기본값 세팅
#define sdm120_MAX485_DE       D2                                                                  // Driver in/output Enable pin DE/RE Active HIGH
#define sdm120_MAX485_RE       D3        // Driver in/output Enable pin DE/RE Active HIGH
#define sdm120_SLAVE_ID        1         // Default slave ID of SDM120
#define sdm120_SSERIAL_RX_PIN  D8        // SOFTWARE SERIAL RX
#define sdm120_SSERIAL_TX_PIN  D9        // SOFTWARE SERIAL TX

// mtqq pubsub 기본값 세팅
const char* ssid        = "Xiaomi_B922";
const char* password    = "yks39969";
const char* mqtt_server = "192.168.31.118";
const char* topicRoot   = "myfarm/";             // MQTT root topic for the device, keep / at the end
unsigned long lastMsg   = 0;

// bac02_modbus rtu object init
ModbusMaster bac002_node;
SoftwareSerial bac002_RS485Serial(bac002_SSERIAL_RX_PIN, bac002_SSERIAL_TX_PIN);
// sdm120_modbus rtu object init
ModbusMaster sdm120_node;
SoftwareSerial sdm120_RS485Serial(sdm120_SSERIAL_RX_PIN, sdm120_SSERIAL_TX_PIN);

// mtqq object init
WiFiClient espClient;
PubSubClient client(espClient);

// ntpclient init 
// GMT+9, timeoffset=32400, update interval = 3600000(ms)
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "kr.pool.ntp.org", 32400, 3600000); 

// software reboot
void ( *resetFunc)(void) = 0;

// mtqq wifi connection setup 
void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  // ntpclient begin
  timeClient.begin();
}

// mtqq로 subscribe된 topic과 payload를 화면에 표기함
void callback(char* topic, byte* payload, unsigned int length) {
  // Convert the incoming byte array to a string
  String mytopic = (char*)topic;
  payload[length] = '\0'; // Null terminator used to terminate the char array
  String message = (char*)payload;
  
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  char checktopic[80];
  uint8_t result;

  ////////////////////////
  // BAC002 write register from node-red dashboard
  ////////////////////////
  // FUNC : 06
  // 0x000 파워설정
  if (mytopic==(String)topicRoot+"write/bac002/setpower") {
     ESP.wdtDisable();
     result = bac002_node.writeSingleRegister(0,atoi((char*)payload));
     ESP.wdtEnable(1);
     if (result == bac002_node.ku8MBSuccess)   {
        Serial.println("SetPower updated");
     } else {
        Serial.print("SetPower update failed: ");
        sendModbusError(bac002_node, result);
     }
  }
  // 0x001 팬설정
  if (mytopic==(String)topicRoot+"write/bac002/setfan") {
     ESP.wdtDisable();
     result = bac002_node.writeSingleRegister(1,atoi((char*)payload));
     ESP.wdtEnable(1);
     if (result == bac002_node.ku8MBSuccess)   {
        Serial.println("SetFan speed updated");
     } else {
        Serial.print("SetFan speed failed: ");
        sendModbusError(bac002_node, result);
     }
  }
  // 0x002 모드설정
  if (mytopic==(String)topicRoot+"write/bac002/setmode") {
     ESP.wdtDisable();
     result = bac002_node.writeSingleRegister(2,atoi((char*)payload));
     ESP.wdtEnable(1);
     if (result == bac002_node.ku8MBSuccess)   {
        Serial.println("SetMode updated");
     } else {
        Serial.print("SetMode update failed: ");
        sendModbusError(bac002_node, result);
     }
  }
  // 0x003 온도설정
  if (mytopic==(String)topicRoot+"write/bac002/settemp") {
     ESP.wdtDisable();
     result = bac002_node.writeSingleRegister(3,atoi((char*)payload));
     ESP.wdtEnable(1);
     if (result == bac002_node.ku8MBSuccess)   {
        Serial.println("SetTemp updated");
     } else {
        Serial.print("SetTemp update failed: ");
        sendModbusError(bac002_node, result);
     }
  }
  // 0x004 LOCK설정
  if (mytopic==(String)topicRoot+"write/bac002/setlock") {
     ESP.wdtDisable();
     result = bac002_node.writeSingleRegister(4,atoi((char*)payload));
     ESP.wdtEnable(1);
     if (result == bac002_node.ku8MBSuccess)   {
        Serial.println("SetLock updated");
     } else {
        Serial.print("SetLock update failed: ");
        sendModbusError(bac002_node, result);
     }
  }
  // 0x005 분설정
  if (mytopic==(String)topicRoot+"write/bac002/setminute") {
     ESP.wdtDisable();
     result = bac002_node.writeSingleRegister(5,atoi((char*)payload));
     ESP.wdtEnable(1);
     if (result == bac002_node.ku8MBSuccess)   {
        Serial.println("SetMinute updated");
     } else {
        Serial.print("SetMinute update failed: ");
        sendModbusError(bac002_node, result);
     }
  }
  // 0x006 시간설정
  if (mytopic==(String)topicRoot+"write/bac002/sethour") {
     ESP.wdtDisable();
     result = bac002_node.writeSingleRegister(6,atoi((char*)payload));
     ESP.wdtEnable(1);
     if (result == bac002_node.ku8MBSuccess)   {
        Serial.println("SetHour updated");
     } else {
        Serial.print("SetHour update failed: ");
        sendModbusError(bac002_node, result);
     }
  }
  // 0x007 요일설정
  if (mytopic==(String)topicRoot+"write/bac002/setweek") {
     ESP.wdtDisable();
     result = bac002_node.writeSingleRegister(7,atoi((char*)payload));
     ESP.wdtEnable(1);
     if (result == bac002_node.ku8MBSuccess)   {
        Serial.println("SetWeek updated");
     } else {
        Serial.print("SetWeek update failed: ");
        sendModbusError(bac002_node, result);
     }
  }
}

// mtqq reconnect
void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      // BAC002에 설정을 변경할 내용을 subscribe한다.
      client.subscribe("myfarm/write/bac002/#");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

// BAC002의 RS485모듈에 D2(DE)/D3(RE)를 송신모드로 설정
void bac002_preTransmission()  //set up call back function
{
  digitalWrite(bac002_MAX485_DE, 1);
  digitalWrite(bac002_MAX485_RE, 1);
}

// BAC02의 RS485모듈에 D2(DE)/D3(RE)를 수신모드로 설정
void bac002_postTransmission()   //set up call back function
{
  //delayMicroseconds(60);
  digitalWrite(bac002_MAX485_DE, 0);
  digitalWrite(bac002_MAX485_RE, 0);
}

// sdm120의 송신모드로 설정
void sdm120_preTransmission()  //set up call back function
{
  digitalWrite(sdm120_MAX485_DE, 1);
  digitalWrite(sdm120_MAX485_RE, 1);
}

// sdm120의 수신모드로 설정
void sdm120_postTransmission()   //set up call back function
{
  //delayMicroseconds(60);
  digitalWrite(sdm120_MAX485_DE, 0);
  digitalWrite(sdm120_MAX485_RE, 0);
}

// MODBUS 오류메시지 출력
void sendModbusError(ModbusMaster node, uint8_t result) {
  String message = "";
  if (result==node.ku8MBIllegalFunction) {
    message = "Illegal function";
  }
  if (result==node.ku8MBIllegalDataAddress) {
    message = "Illegal data address";
  }
  if (result==node.ku8MBIllegalDataValue) {
    message = "Illegal data value";
  }
  if (result==node.ku8MBSlaveDeviceFailure) {
    message = "Slave device failure";
  }
  if (result==node.ku8MBInvalidSlaveID) {
    message = "Invalid slave ID";
  }
  if (result==node.ku8MBInvalidFunction) {
    message = "Invalid function";
  }
  if (result==node.ku8MBResponseTimedOut) {
    message = "Response timed out";
  }
  if (result==node.ku8MBInvalidCRC) {
    message = "Invalid CRC";
  }
  if (message=="") {
    message = result;
  }
  Serial.println(message);    
  char topic[80];
  char value[30];
  sprintf(topic,"%serror",topicRoot);
  client.publish(topic, message.c_str());
}

void setup()
{
  ////////////////////
  // bac002_modbusrtu
  ////////////////////
  pinMode(bac002_MAX485_DE, OUTPUT);
  pinMode(bac002_MAX485_RE, OUTPUT);
  // Init in receive mode
  digitalWrite(bac002_MAX485_DE, 0);
  digitalWrite(bac002_MAX485_RE, 0);

  ////////////////////
  // sdm120_modbusrtu
  ////////////////////
  pinMode(sdm120_MAX485_DE, OUTPUT);
  pinMode(sdm120_MAX485_RE, OUTPUT);
  // Init in receive mode
  digitalWrite(sdm120_MAX485_DE, 0);
  digitalWrite(sdm120_MAX485_RE, 0);

  // Modbus communication runs at 9600 baud
  Serial.begin(MODBUS_RATE);
  bac002_RS485Serial.begin(MODBUS_RATE);
  sdm120_RS485Serial.begin(MODBUS_RATE);
  
  // Modbus slave ID 1, numbers are in decimal format
  bac002_node.begin(bac002_SLAVE_ID, bac002_RS485Serial);  //data from max 485 are communicating with serial
  sdm120_node.begin(sdm120_SLAVE_ID, sdm120_RS485Serial);  //data from max 485 are communicating with serial
  // Callbacks allow us to configure the RS485 transceiver correctly
  bac002_node.preTransmission(bac002_preTransmission);
  bac002_node.postTransmission(bac002_postTransmission);
  sdm120_node.preTransmission(sdm120_preTransmission);
  sdm120_node.postTransmission(sdm120_postTransmission);

  // mtqq wifi 접속 설정
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}


void loop()
{
  // 멀티쓰레딩 2초
  unsigned long now = millis();
  
  // millis()는 시스템 부팅 후 현재까지 clock이므로
  // 0xFFFFFFF(50일)이상이 되면 다시 0x0000000이 되므로 쓰레드가 멈추게 된다.
  // 리부팅을 통해 다시 이전시간과 현재를 비교해야한다.
  // 참고 : https://cafe.naver.com/cplc/272101?art=ZXh0ZXJuYWwtc2VydmljZS1uYXZlci1zZWFyY2gtY2FmZS1wcg.eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJjYWZlVHlwZSI6IkNBRkVfVVJMIiwiY2FmZVVybCI6ImNwbGMiLCJhcnRpY2xlSWQiOjI3MjEwMSwiaXNzdWVkQXQiOjE2NzE2ODUyNTU2MjJ9.1DbKlqWxS4wK-Vh-h74A91ilX9Kg4kzwC4XZPdehzlY 
  if (now - lastMsg < 0 ) resetFunc();
  
  if (now - lastMsg > 2000) {
    lastMsg = now;

    ///////////////
    // mtqq connect
    ///////////////
    if (!client.connected()) {
      reconnect();
    }
    client.loop();

    //////////////
    // 조도센서측정
    //////////////
    // ntpclient update
    /////////////
    timeClient.update();
    float Illumina = analogRead(A0);
    Serial.println("------------");
    Serial.print("Illumina : ");
    Serial.println(Illumina);

    int current_hour;
    // 현재시간 찾기
    current_hour = timeClient.getHours();  
    Serial.print("hour :");
    Serial.println(current_hour);

    /////////////////////////
    // mtqq publish 변수
    /////////////////////////
    char topic[80];
    char value[10];

    /////////////////////////
    // bac002 modbusrtu read
    /////////////////////////
    uint8_t bac002_result;  
    // Read 16 registers starting at 0x00, read 11 register. Meaning that read 0x00, then read 0x01, so on and so forth. Until the eleventh resister 0x0A
    Serial.println("bac002 request");
    
    bac002_result = bac002_node.readHoldingRegisters(0x0000, 11);
    if (bac002_result == bac002_node.ku8MBSuccess)
    {
      Serial.println("------------");
      Serial.print("POWER ON: ");
      Serial.println(bac002_node.getResponseBuffer(0x00));
      Serial.print("FAN SPEED: ");
      Serial.println(bac002_node.getResponseBuffer(0x01));
      Serial.print("SETTING MODE: ");
      Serial.println(bac002_node.getResponseBuffer(0x02));
      Serial.print("SETTING TEMP: ");
      Serial.println(bac002_node.getResponseBuffer(0x03)/10.0f);
      Serial.print("SETTING LOCK: ");
      Serial.println(bac002_node.getResponseBuffer(0x04));
      Serial.print("MINUTE: ");
      Serial.println(bac002_node.getResponseBuffer(0x05));
      Serial.print("HOUR: ");
      Serial.println(bac002_node.getResponseBuffer(0x06));
      Serial.print("WEEK: ");
      Serial.println(bac002_node.getResponseBuffer(0x07));
      Serial.print("TEMP: ");
      Serial.println(bac002_node.getResponseBuffer(0x08)/10.0f);
      Serial.print("VALVE STATUS: ");
      Serial.println(bac002_node.getResponseBuffer(0x09));
      Serial.print("FAN STATUS: ");
      Serial.println(bac002_node.getResponseBuffer(0x0a));

      /////////////////////////
      // BAC002 mtqq publishing
      /////////////////////////
      // Power
      sprintf(topic,"%sread/power",topicRoot);
      sprintf(value,"%d",bac002_node.getResponseBuffer(0x00));
      client.publish(topic, value);
      // fan speed
      sprintf(topic,"%sread/fanspeed",topicRoot);
      sprintf(value,"%d",bac002_node.getResponseBuffer(0x01));
      client.publish(topic, value);
      // setting mode
      sprintf(topic,"%sread/settingmode",topicRoot);
      sprintf(value,"%d",bac002_node.getResponseBuffer(0x02));
      client.publish(topic, value);
      // setting temp
      sprintf(topic,"%sread/settingtemp",topicRoot);
      sprintf(value,"%d",bac002_node.getResponseBuffer(0x03)/10.0f);
      client.publish(topic, value);
      // setting lock
      sprintf(topic,"%sread/settinglock",topicRoot);
      sprintf(value,"%d",bac002_node.getResponseBuffer(0x04));
      client.publish(topic, value);
      // minute
      sprintf(topic,"%sread/minute",topicRoot);
      sprintf(value,"%d",bac002_node.getResponseBuffer(0x05));
      client.publish(topic, value);
      // hour
      sprintf(topic,"%sread/hour",topicRoot);
      sprintf(value,"%d",bac002_node.getResponseBuffer(0x06));
      client.publish(topic, value);
      // week
      sprintf(topic,"%sread/week",topicRoot);
      sprintf(value,"%d",bac002_node.getResponseBuffer(0x07));
      client.publish(topic, value);
      // roomtemp
      sprintf(topic,"%sread/roomtemp",topicRoot);
      sprintf(value,"%f",bac002_node.getResponseBuffer(0x08)/10.0f);
      client.publish(topic, value);
      // realy_in3 HEATER 제어를 위한 publish
      sprintf(topic,"%swrite/relay03",topicRoot);
      client.publish(topic, value);
      // heater status
      sprintf(topic,"%sread/heaterstatus",topicRoot);
      sprintf(value,"%d",bac002_node.getResponseBuffer(0x09));
      client.publish(topic, value);
      // fan status
      sprintf(topic,"%sread/fanstatus",topicRoot);
      sprintf(value,"%d",bac002_node.getResponseBuffer(0x0A));
      client.publish(topic, value);
      // 조도센서 Illumina value
      sprintf(topic,"%sread/illumina",topicRoot);
      sprintf(value,"%f",Illumina);
      client.publish(topic, value);
      // realy_in1/in4 LED/LEDFAN 제어를 위한 publish
      sprintf(topic,"%swrite/relay01",topicRoot);
      client.publish(topic, value);

    } else {
      sendModbusError(bac002_node, bac002_result);
    }

    /////////////////////////
    // sdm120 modbusrtu read
    /////////////////////////
    uint8_t sdm120_result;  
    uint16_t data[10];
    union 
    {      uint16_t b[4] ;
           float f ;
    } u ;

    // Voltage
    sdm120_result = sdm120_node.readInputRegisters(0x0000,2);//readHoldingRegisters(0x0000, 45);
    if (sdm120_result == sdm120_node.ku8MBSuccess)
    {
      Serial.println("------------");
      Serial.print("Voltage: ");
      for (int j = 0; j < 2; j++) {
        data[j] = sdm120_node.getResponseBuffer(j);
        u.b[1-j] = data[j];
      }
      Serial.print(u.f);
      Serial.println("");
      sprintf(topic,"%sread/voltage",topicRoot);
      sprintf(value,"%f",u.f);
      client.publish(topic, value);

    }
    // Current
    sdm120_result = sdm120_node.readInputRegisters(0x0006,2);//readHoldingRegisters(0x0000, 45);
    if (sdm120_result == sdm120_node.ku8MBSuccess)
    {
      Serial.print("Current: ");
      for (int j = 0; j < 2; j++) {
        data[j] = sdm120_node.getResponseBuffer(j);
        u.b[1-j] = data[j];
      }
      Serial.print(u.f);
      Serial.println("");
      sprintf(topic,"%sread/current",topicRoot);
      sprintf(value,"%f",u.f);
      client.publish(topic, value);
    }
    // Active Power:
    sdm120_result = sdm120_node.readInputRegisters(0x000C,2);//readHoldingRegisters(0x0000, 45);
    if (sdm120_result == sdm120_node.ku8MBSuccess)
    {
      Serial.print("Active Power: ");
      for (int j = 0; j < 2; j++) {
        data[j] = sdm120_node.getResponseBuffer(j);
        u.b[1-j] = data[j];
      }
      Serial.print(u.f);
      Serial.println("");
      sprintf(topic,"%sread/active_power",topicRoot);
      sprintf(value,"%f",u.f);
      client.publish(topic, value);
    }
    // Apparent Power
    sdm120_result = sdm120_node.readInputRegisters(0x0012,2);//readHoldingRegisters(0x0000, 45);
    if (sdm120_result == sdm120_node.ku8MBSuccess)
    {
      Serial.print("Apparent Power: ");
      for (int j = 0; j < 2; j++) {
        data[j] = sdm120_node.getResponseBuffer(j);
        u.b[1-j] = data[j];
      }
      Serial.print(u.f);
      Serial.println("");
      sprintf(topic,"%sread/apparent_power",topicRoot);
      sprintf(value,"%f",u.f);
      client.publish(topic, value);
    }
    // Reactive Power
    sdm120_result = sdm120_node.readInputRegisters(0x0018,2);//readHoldingRegisters(0x0000, 45);
    if (sdm120_result == sdm120_node.ku8MBSuccess)
    {
      Serial.print("Reactive Power: ");
      for (int j = 0; j < 2; j++) {
        data[j] = sdm120_node.getResponseBuffer(j);
        u.b[1-j] = data[j];
      }
      Serial.print(u.f);
      Serial.println("");
      sprintf(topic,"%sread/reactive_power",topicRoot);
      sprintf(value,"%f",u.f);
      client.publish(topic, value);
    }
    // Power Factor
    sdm120_result = sdm120_node.readInputRegisters(0x001E,2);//readHoldingRegisters(0x0000, 45);
    if (sdm120_result == sdm120_node.ku8MBSuccess)
    {
      Serial.print("Power Factor: ");
      for (int j = 0; j < 2; j++) {
        data[j] = sdm120_node.getResponseBuffer(j);
        u.b[1-j] = data[j];
      }
      Serial.print(u.f);
      Serial.println("");
      sprintf(topic,"%sread/power_factor",topicRoot);
      sprintf(value,"%f",u.f);
      client.publish(topic, value);
    }
    // Phase Angle
    sdm120_result = sdm120_node.readInputRegisters(0x0024,2);//readHoldingRegisters(0x0000, 45);
    if (sdm120_result == sdm120_node.ku8MBSuccess)
    {
      Serial.print("Phase Angle: ");
      for (int j = 0; j < 2; j++) {
        data[j] = sdm120_node.getResponseBuffer(j);
        u.b[1-j] = data[j];
      }
      Serial.print(u.f);
      Serial.println("");
      sprintf(topic,"%sread/phase_angle",topicRoot);
      sprintf(value,"%f",u.f);
      client.publish(topic, value);
    }
    // Frequency
    sdm120_result = sdm120_node.readInputRegisters(0x0046,2);//readHoldingRegisters(0x0000, 45);
    if (sdm120_result == sdm120_node.ku8MBSuccess)
    {
      Serial.print("Frequency: ");
      for (int j = 0; j < 2; j++) {
        data[j] = sdm120_node.getResponseBuffer(j);
        u.b[1-j] = data[j];
      }
      Serial.print(u.f);
      Serial.println("");
      sprintf(topic,"%sread/frequency",topicRoot);
      sprintf(value,"%f",u.f);
      client.publish(topic, value);
    }
    // Import Active Energy
    sdm120_result = sdm120_node.readInputRegisters(0x0048,2);//readHoldingRegisters(0x0000, 45);
    if (sdm120_result == sdm120_node.ku8MBSuccess)
    {
      Serial.print("Import Active Energy: ");
      for (int j = 0; j < 2; j++) {
        data[j] = sdm120_node.getResponseBuffer(j);
        u.b[1-j] = data[j];
      }
      Serial.print(u.f);
      Serial.println("");
      sprintf(topic,"%sread/import_active_energy",topicRoot);
      sprintf(value,"%f",u.f);
      client.publish(topic, value);
    }
    // Export Active Energy
    sdm120_result = sdm120_node.readInputRegisters(0x004A,2);//readHoldingRegisters(0x0000, 45);
    if (sdm120_result == sdm120_node.ku8MBSuccess)
    {
      Serial.print("Export Active Energy: ");
      for (int j = 0; j < 2; j++) {
        data[j] = sdm120_node.getResponseBuffer(j);
        u.b[1-j] = data[j];
      }
      Serial.print(u.f);
      Serial.println("");
      sprintf(topic,"%sread/export_active_energy",topicRoot);
      sprintf(value,"%f",u.f);
      client.publish(topic, value);
    }
    // Import Reactive Energy
    sdm120_result = sdm120_node.readInputRegisters(0x004C,2);//readHoldingRegisters(0x0000, 45);
    if (sdm120_result == sdm120_node.ku8MBSuccess)
    {
      Serial.print("Import Reactive Energy: ");
      for (int j = 0; j < 2; j++) {
        data[j] = sdm120_node.getResponseBuffer(j);
        u.b[1-j] = data[j];
      }
      Serial.print(u.f);
      Serial.println("");
      sprintf(topic,"%sread/import_reactive_energy",topicRoot);
      sprintf(value,"%f",u.f);
      client.publish(topic, value);
    }
    // Export Reactive Energy
    sdm120_result = sdm120_node.readInputRegisters(0x004E,2);//readHoldingRegisters(0x0000, 45);
    if (sdm120_result == sdm120_node.ku8MBSuccess)
    {
      Serial.print("Export Reactive Energy: ");
      for (int j = 0; j < 2; j++) {
        data[j] = sdm120_node.getResponseBuffer(j);
        u.b[1-j] = data[j];
      }
      Serial.print(u.f);
      Serial.println("");
      sprintf(topic,"%sread/export_reactive_energy",topicRoot);
      sprintf(value,"%f",u.f);
      client.publish(topic, value);
    }
    // Total Active Energy 
    sdm120_result = sdm120_node.readInputRegisters(0x0156,2);//readHoldingRegisters(0x0000, 45);
    if (sdm120_result == sdm120_node.ku8MBSuccess)
    {
      Serial.print("Total Active Energy : ");
      for (int j = 0; j < 2; j++) {
        data[j] = sdm120_node.getResponseBuffer(j);
        u.b[1-j] = data[j];
      }
      Serial.print(u.f);
      Serial.println("");
      sprintf(topic,"%sread/total_active_energy",topicRoot);
      sprintf(value,"%f",u.f);
      client.publish(topic, value);
    }
    // Total Reactive Energy
    sdm120_result = sdm120_node.readInputRegisters(0x0158,2);//readHoldingRegisters(0x0000, 45);
    if (sdm120_result == sdm120_node.ku8MBSuccess)
    {
      Serial.print("Total Reactive Energy: ");
      for (int j = 0; j < 2; j++) {
        data[j] = sdm120_node.getResponseBuffer(j);
        u.b[1-j] = data[j];
      }
      Serial.print(u.f);
      Serial.println("");
      sprintf(topic,"%sread/total_reactive_energy",topicRoot);
      sprintf(value,"%f",u.f);
      client.publish(topic, value);
    }
    if (sdm120_result != sdm120_node.ku8MBSuccess) {
      sendModbusError(sdm120_node, sdm120_result);
    }  
  }
  delay(1000);
}
