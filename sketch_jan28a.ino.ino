#include <U8g2lib.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>

#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif
#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
#endif


String confirm, confirm_add, suspect, suspect_add, dead, dead_add, heal, heal_add; //声明所需的变量

String ip = "192.168.31.177:2334"; //输入你的服务端所在电脑的IP + 端口号码   默认端口2334


//GPIO实际对应关系
static const uint8_t D1   = 5;
static const uint8_t D2   = 4;
static const uint8_t D4   = 2;
static const uint8_t D5   = 14;
static const uint8_t D6   = 12;

//定义4个屏幕的接口
U8G2_SSD1306_128X64_NONAME_F_SW_I2C u8g2(U8G2_R0, /* clock=*/ D1, /* data=*/ D2, /* reset=*/ U8X8_PIN_NONE);   //第一个屏幕
U8G2_SSD1306_128X64_NONAME_F_SW_I2C u8g2b(U8G2_R0, /* clock=*/ D1, /* data=*/ D4, /* reset=*/ U8X8_PIN_NONE);  //第二个屏幕
U8G2_SSD1306_128X64_NONAME_F_SW_I2C u8g2c(U8G2_R0, /* clock=*/ D1, /* data=*/ D5, /* reset=*/ U8X8_PIN_NONE);  //第三个屏幕
U8G2_SSD1306_128X64_NONAME_F_SW_I2C u8g2d(U8G2_R0, /* clock=*/ D1, /* data=*/ D6, /* reset=*/ U8X8_PIN_NONE);  //第四个屏幕

HTTPClient http;
WiFiClient client;
const unsigned long HTTP_TIMEOUT = 5000;

//自动连接保存的wifi
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
//智能配网
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
  u8g2b.begin();
  u8g2b.enableUTF8Print();
  u8g2c.begin();
  u8g2c.enableUTF8Print();
  u8g2d.begin();
  u8g2d.enableUTF8Print();
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
  u8g2.clearBuffer();
  http.setTimeout(HTTP_TIMEOUT);
}

void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    String GetUrl1 ="http://" + ip + "/json";
    http.begin(GetUrl1);
    int httpCode = http.GET();
    Serial.printf("code = %d\r\n", httpCode);
    if (httpCode > 0) {
      String resBuff = http.getString();
      //Serial.println(resBuff);
      DynamicJsonBuffer jsonBuffer;
      JsonObject& root = jsonBuffer.parseObject(resBuff);
      if (!root.success())
      {
        Serial.println("parseObject() failed");
        return;
      }
      String code = root["code"];
      Serial.println(code);
      if (code != "0")
      {
        return;
      }

      confirm = root["confirm"].asString();//取出现存确诊数
      confirm_add = root["confirm_add"].asString();//取出增加确诊数
      suspect = root["suspect"].asString();//取出现有疑似数
      suspect_add = root["suspect_add"].asString();//取出增加疑似数
      dead = root["dead"].asString();//取出现有死亡数
      dead_add = root["dead_add"].asString();//取出增加死亡数
      heal = root["heal"].asString();//取出现有痊愈数
      heal_add = root["heal_add"].asString();//取出增加痊愈数
      refreshMonitor(confirm, confirm_add, suspect, suspect_add, dead, dead_add, heal, heal_add); //刷新屏幕显示
    }
  }
  delay(60000);//每分钟更新一次,建议不要小于1分钟
}

void refreshMonitor(String confirm, String confirm_add, String suspect, String suspect_add, String dead, String dead_add, String heal, String heal_add)
{
  /*第一块屏幕*/
  u8g2.clearBuffer(); //清除缓存区
  u8g2.drawFrame(0, 0, 128, 63); //在x(0, 0) y(128, 63)绘制矩形
  u8g2.drawLine(0, 44, 128, 44); //在x(0, 40) y(128, 40)两点之间绘制线
  u8g2.setFont(u8g2_font_wqy12_t_gb2312a); // 设置字体
  u8g2.setCursor(2, 12); //光标位置
  u8g2.print("确诊数:"); //打印内容
  u8g2.setCursor(2, 58); //光标位置
  u8g2.print("较昨日:"); //打印内容
  u8g2.setCursor(42, 58); //光标位置
  u8g2.print(confirm_add); //打印内容
  u8g2.setFont(u8g2_font_unifont_t_Ikcfonts_24u); // 设置字体
  u8g2.setCursor(24, 38); //光标位置
  u8g2.print(confirm); //打印内容
  u8g2.sendBuffer(); //发送内容到缓存区

  /*第二块屏幕*/
  u8g2b.clearBuffer(); //清除缓存区
  u8g2b.drawFrame(0, 0, 128, 63); //在x(0, 0) y(128, 63)绘制矩形
  u8g2b.drawLine(0, 44, 128, 44); //在x(0, 40) y(128, 40)两点之间绘制线
  u8g2b.setFont(u8g2_font_wqy12_t_gb2312a); // 设置字体
  u8g2b.setCursor(2, 12); //光标位置
  u8g2b.print("疑似病例:"); //打印内容
  u8g2b.setCursor(2, 58); //光标位置
  u8g2b.print("较昨日:"); //打印内容
  u8g2b.setCursor(42, 58); //光标位置
  u8g2b.print(suspect_add); //打印内容
  u8g2b.setFont(u8g2_font_unifont_t_Ikcfonts_24u); // 设置字体
  u8g2b.setCursor(24, 38); //光标位置
  u8g2b.print(suspect); //打印内容
  u8g2b.sendBuffer(); //发送内容到缓存区

  /*第三块屏幕*/
  u8g2c.clearBuffer(); //清除缓存区
  u8g2c.drawFrame(0, 0, 128, 63); //在x(0, 0) y(128, 63)绘制矩形
  u8g2c.drawLine(0, 44, 128, 44); //在x(0, 40) y(128, 40)两点之间绘制线
  u8g2c.setFont(u8g2_font_wqy12_t_gb2312a); // 设置字体
  u8g2c.setCursor(2, 12); //光标位置
  u8g2c.print("死亡数:"); //打印内容
  u8g2c.setCursor(2, 58); //光标位置
  u8g2c.print("较昨日:"); //打印内容
  u8g2c.setCursor(42, 58); //光标位置
  u8g2c.print(dead_add); //打印内容
  u8g2c.setFont(u8g2_font_unifont_t_Ikcfonts_24u); // 设置字体
  u8g2c.setCursor(24, 38); //光标位置
  u8g2c.print(dead); //打印内容
  u8g2c.sendBuffer(); //发送内容到缓存区

  /*第四块屏幕*/
  u8g2d.clearBuffer(); //清除缓存区
  u8g2d.drawFrame(0, 0, 128, 63); //在x(0, 0) y(128, 63)绘制矩形
  u8g2d.drawLine(0, 44, 128, 44); //在x(0, 40) y(128, 40)两点之间绘制线
  u8g2d.setFont(u8g2_font_wqy12_t_gb2312a); // 设置字体
  u8g2d.setCursor(2, 12); //光标位置
  u8g2d.print("治愈数:"); //打印内容
  u8g2d.setCursor(2, 58); //光标位置
  u8g2d.print("较昨日:"); //打印内容
  u8g2d.setCursor(42, 58); //光标位置
  u8g2d.print(heal_add); //打印内容
  u8g2d.setFont(u8g2_font_unifont_t_Ikcfonts_24u); // 设置字体
  u8g2d.setCursor(24, 38); //光标位置
  u8g2d.print(heal); //打印内容
  u8g2d.sendBuffer(); //发送内容到缓存区



}
