#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecureBearSSL.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <ArduinoJson.h>
#include <GxEPD2_3C.h>
#include "GxEPD2_display_selection_new_style.h"
#include <U8g2_for_Adafruit_GFX.h>

#include <avr/pgmspace.h>

#include "bitmap.h"
#include "gb2312.h"                    //13x13
#include "u8g2_sarasa_16_gb2312.h"     //21x21
#include "u8g2_mfxinran_16_number.h"   //21x21
#include "u8g2_mfxinran_92_number.h"   //80*100

//按键定义
#define KEY_UP            5  //上方按键
#define KEY_DOWN         EN  //下方按键
//电量读取PIN
#define BAT_PIN_SWITCH   12  //电池电压读取开关
#define BAT_PIN_VCC      A0  //读取电池电压引脚
#define BAT_FULL        4.2  //满电压
#define BAT_LOW         3.2  //低电压

//U8g2显示
U8G2_FOR_ADAFRUIT_GFX u8g2Fonts;
//wifi
ESP8266WiFiMulti WiFiMulti;

//屏幕大小 4.2寸 400x300
#define SCREEN_WIDTH  400 //display.width()
#define SCREEN_HEIGTH 300 //display.height()


//显示颜色
#define COLOR_WHITE  GxEPD_WHITE  //白色
#define COLOR_BLACK  GxEPD_BLACK  //黑色
#define COLOR_RED    GxEPD_RED    //红色

//字体大小
#define FONT_SIZE_NUMBER 100                                           //数字字体大小，根据选择的字体大小设置
#define FONT_SIZE_CHINESE 13                                           //小字体大小，根据选择的字体大小设置
#define FONT_SIZE_CHINESE_SPACING (FONT_SIZE_CHINESE + 3)              //留3个像素的行间距
#define FONT_SIZE_CHINESE_LARGE 21                                     //大字体大小，根据选择的字体大小设置
#define FONT_SIZE_CHINESE_LARGE_SPACING (FONT_SIZE_CHINESE_LARGE + 3)  //留3个像素的行间距

//重试次数
#define MAX_TRY_COUNT 3

//天气图表大小
#define ICON_SIZE_WEATHER 45  //45x45
#define ICON_SIZE_SMALL 13    //13x13

// wifi
const char *ssid     = "{ssid}";
const char *password = "{password}";

// 日期
const char url_Date[] PROGMEM = "https://api.heidai.space/date";
// 天气，修改location为所在城市拼音，首字母大写，可以用浏览器打开链接测试下
const char url_Weather[] PROGMEM = "https://api.heidai.space/weather/?location=Shanghai";
// 一言，max为返回句子的最大字数
const char url_Hitokoto[] PROGMEM = "https://api.heidai.space/hitokoto/?max=28";
// 加密货币
const char url_Coin[] PROGMEM = "https://api.heidai.space/coin";

// 刷新间隔
#define REFRESH_FREQUENCY  1   //每x小时刷新
#define REFRESH_TIME_START 7   //刷新屏幕的时间范围，8点以后刷新
#define REFRESH_TIME_END  20   //刷新屏幕的时间范围，20点前刷新
// 休眠时间
#define SLEEP_TIME (REFRESH_FREQUENCY * 60)

// 是否为夜间
bool gisNight = false;

//RTC变量地址
#define RTCdz_EpochTime_LastUpdate       0    //上次刷新时间RTC地址
#define RTCdz_EpochTime_Now              1    //当前系统时间RTC地址。非精准时间，通过推算获取

//时间信息结构
typedef struct _Time {
  uint16_t year;
  uint8_t month;
  uint8_t day;
  uint8_t week;  // 0 is Sunday
  uint8_t hour;
  uint8_t minute;
  uint8_t second;
  uint16_t percentage;      //当年进度 已过去天数/总天数*SCREEN_HEIGTH
} Time;

//日期信息结构
typedef struct _Date {
  Time time;                //时间
  char holiday[40];         //节日
  char convert[40];         //农历
  char lunar_festival[40];  //农历节日
  char shengxiao[8];        //生肖
  char ganzhi[16];          //干支
  char yi[100];             //宜忌
  char ji[100];             //宜忌
  char term[40];            //节气
  bool isWorkday;           //是否工作日
} Date;

//天气
typedef struct _Weather {
  char location[20];        //城市
  char text_day[20];        //白天天气现象名称
  char code_day[4];         //白天天气现象代码
  char text_night[20];      //晚上天气现象名称
  char code_night[4];       //晚上天气现象代码
  char high[5];             //最高温度
  char low[5];              //最低温度
  char humidity[5];         //相对湿度
  char wind_direction[20];  //风向
  char wind_speed[10];      //风速，单位km/h
  char dressing[20];        //穿衣
  char uv[10];              //紫外线指数
} Weather;

//一言API
typedef struct _Hitokoto {
  char hitokoto[100];  //一言
  char from[64];       //出自
} Hitokoto;

//加密货币
typedef struct _CryptoCoin {
  char price_btc[20];   //BTC 价格
  char price_eth[20];   //ETH 价格
  char price_bnb[20];   //BNB 价格
  char price_sol[20];   //SOL 价格
} CryptoCoin;

const char DefaultHitokoto[] PROGMEM = "Talk is cheap. Show me the code.";  //默认信息
const char DefaultHitokotoFrom[] PROGMEM = "Linus Torvalds";



void setup()
{
  Serial.begin(115200);

  //一些初始化
  display.init();
  ESP.wdtEnable(60000);  //使能软件看门狗的触发间隔

  display.setRotation(0);    //设置屏幕方向
  u8g2Fonts.begin(display);  //将u8g2过程连接到Adafruit GFX

  display_setup();  //开机信息显示

  Serial.printf("剩余堆空间：%d\n", ESP.getFreeHeap());  //查看堆空间

  if(!isNeedRefresh()) goto ESP_SLEEP;//检查是否需要刷新

  //led_controller(1); //点亮led

  if(!connectToWifi()) goto ESP_SLEEP;//连接wifi

  display_MainPage(); //显示主页

ESP_SLEEP:
  //led_controller(0); //熄灭led
  Serial.printf("剩余堆空间：%d\n", ESP.getFreeHeap());//查看堆空间
  esp_sleep(SLEEP_TIME); //休眠SLEEP_TIME后重启
}

void loop()
{

}
