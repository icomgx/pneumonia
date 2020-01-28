#include <U8g2lib.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif
#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
#endif

static const uint8_t D1   = 5;
static const uint8_t D2   = 4;
String qz;
String ys;



U8G2_SSD1306_128X64_NONAME_F_SW_I2C u8g2(U8G2_R0, /* clock=*/ D1, /* data=*/ D2, /* reset=*/ U8X8_PIN_NONE);
HTTPClient http;
WiFiClient client;
const unsigned long HTTP_TIMEOUT = 5000;
bool autoConfig()
{
  WiFi.mode(WIFI_STA);
  WiFi.begin();                    // 默认连接保存的WIFI
  for (int i = 0; i < 20; i++)
  {
    int wstatus = WiFi.status();
    if (wstatus == WL_CONNECTED)
    {
      Serial.println("AutoConfig Success");
      //Serial.printf("SSID:%s\r\n", WiFi.SSID().c_str());
      //Serial.printf("PSW:%s\r\n", WiFi.psk().c_str());
      WiFi.printDiag(Serial);
      return true;

    }
    else
    {
      Serial.print("AutoConfig Waiting...");
      Serial.println(wstatus);
      delay(1000);
    }
  }
  Serial.println("AutoConfig Faild!" );
  return false;

}
void smartConfig()
{
  WiFi.mode(WIFI_STA);
  Serial.println("\r\nWait for Smartconfig");
  WiFi.beginSmartConfig();
  while (1)
  {
    Serial.print(".");
    if (WiFi.smartConfigDone())
    {
      Serial.println("SmartConfig Success");
      //Serial.printf("SSID:%s\r\n", WiFi.SSID().c_str());
      //Serial.printf("PSW:%s\r\n", WiFi.psk().c_str());
      WiFi.setAutoConnect(true);  // 设置自动连接
      break;
    }
    delay(1000); // 这个地方一定要加延时，否则极易崩溃重启
  }
}


void setup() {

  Serial.begin(9600);
  u8g2.begin();
  u8g2.enableUTF8Print();
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_wqy12_t_gb2312a); // choose a suitable font  (设置字体)
  u8g2.setCursor(0, 28);
  u8g2.print("Wifi Connecting...");
  delay(2000);
  u8g2.sendBuffer();
  if (!autoConfig())
  {
    //u8g2.clearDisplay();
    u8g2.clearBuffer();
    Serial.println("Start module");
    u8g2.setCursor(0, 28);
    u8g2.print("Connecting Faild...");
    u8g2.setCursor(0, 42);
    u8g2.print("Start SmartConfig");
    u8g2.sendBuffer();
    smartConfig();
  }

  delay(1000);
  //u8g2.clearDisplay();
  u8g2.clearBuffer();
  u8g2.setCursor(0, 14);
  u8g2.print("Wifi Connected");
  u8g2.setCursor(0, 28);
  u8g2.print("Connected SSID:");
  u8g2.setCursor(90, 28);
  u8g2.print(WiFi.SSID().c_str());
  u8g2.setCursor(0, 40);
  u8g2.print("Local IP:");
  u8g2.setCursor(52, 40);
  u8g2.print(WiFi.localIP());
  u8g2.setCursor(0, 52);
  u8g2.print("System Begin...");
  u8g2.sendBuffer();
  delay(2000);
  //u8g2.clearDisplay();
  u8g2.clearBuffer();
  http.setTimeout(HTTP_TIMEOUT);
}

void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    String GetUrlqz = "http://192.168.31.177:2334/?Mode=qz";
    String GetUrlys = "http://192.168.31.177:2334/?Mode=ys";
    http.begin(GetUrlqz);
    int httpCode = http.GET();
    Serial.printf("code = %d\r\n", httpCode);
    if (httpCode > 0) {
      String resBuffqz = http.getString();
      qz = resBuffqz;

      http.begin(GetUrlys);
      int httpCode = http.GET();
      Serial.printf("code = %d\r\n", httpCode);
      if (httpCode > 0) {
        String resBuffys = http.getString();
        ys = resBuffys;
        //u8g2.clearDisplay();
        u8g2.clearBuffer();
        u8g2.drawFrame(0, 53, 128, 10);
        u8g2.setFont(u8g2_font_wqy12_t_gb2312a); // choose a suitable font  (设置字体)
        u8g2.setCursor(12, 14);
        u8g2.print("新型冠状病毒统计");
        u8g2.setCursor(0, 28);
        u8g2.print("确诊数量:");
        u8g2.setCursor(0, 42);
        u8g2.print("疑似数量:");
        u8g2.setCursor(64, 28);
        u8g2.print(qz);
        u8g2.setCursor(64, 42);
        u8g2.print(ys);
        u8g2.sendBuffer();

      }
    }
  }

 
  for (int i = 0; i < 128; i = i + 5)
  {
    u8g2.clearBuffer();
    u8g2.drawFrame(0, 53, 128, 10);
    u8g2.drawLine(0, 54, i, 54);
    u8g2.drawLine(0, 55, i, 55);
    u8g2.drawLine(0, 56, i, 56);
    u8g2.drawLine(0, 57, i, 57);
    u8g2.drawLine(0, 58, i, 58);
    u8g2.drawLine(0, 59, i, 59);
    u8g2.drawLine(0, 60, i, 60);
    u8g2.drawLine(0, 61, i, 61);
    u8g2.setFont(u8g2_font_wqy12_t_gb2312a); // choose a suitable font  (设置字体)
    u8g2.setCursor(12, 14);
    u8g2.print("新型冠状病毒统计");
    u8g2.setCursor(0, 28);
    u8g2.print("确诊数量:");
    u8g2.setCursor(0, 42);
    u8g2.print("疑似数量:");
    u8g2.setCursor(64, 28);
    u8g2.print(qz);
    u8g2.setCursor(64, 42);
    u8g2.print(ys);
    u8g2.sendBuffer();
    delay(10);
  }





}
