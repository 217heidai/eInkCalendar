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

#include "bitmap.h"

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

//屏幕大小 4.2寸 400x300
#define SCREEN_WIDTH  400 //display.width()
#define SCREEN_HEIGTH 300 //display.height()


//显示颜色
#define COLOR_WHITE  GxEPD_WHITE  //白色
#define COLOR_BLACK  GxEPD_BLACK  //黑色
#define COLOR_RED    GxEPD_RED    //红色

//GB2312字库
#include "gb2312.c" //13x13
#include "u8g2_mfxinran_92_number.c"    //80*100
#include "u8g2_mfyuanhei_16_gb2312.c"   //21x21
//声明外部变量
//extern const uint8_t chinese_city_gb2312[239032] PROGMEM U8G2_FONT_SECTION("chinese_city_gb2312");
//extern const uint8_t u8g2_mfxinran_92_number[1240] PROGMEM U8G2_FONT_SECTION("u8g2_mfxinran_92_number");
//extern const uint8_t u8g2_mfyuanhei_16_gb2312[334933] PROGMEM U8G2_FONT_SECTION("u8g2_mfyuanhei_16_gb2312");
//字体大小
#define FONT_SIZE_NUMBER  100 //80*100
#define FONT_SIZE_CHINESE 13  //根据选择的字体大小设置
#define FONT_SIZE_CHINESE_SPACING (FONT_SIZE_CHINESE + 3)  //留3个像素的行间距
#define FONT_SIZE_CHINESE_LARGE  21
#define FONT_SIZE_CHINESE_LARGE_SPACING (FONT_SIZE_CHINESE_LARGE + 3)  //留3个像素的行间距

#define MAX_TRY_COUNT 3

//天气图表大小
#define ICON_SIZE_WEATHER 45 //45x45
#define ICON_SIZE_SMALL 13   //13x13

// wifi
const char *ssid     = "{ssid}";
const char *password = "{password}";

//日期
static const char *url_Workday = "https://api.xlongwei.com/service/datetime/isworkday.json"; //是否工作日
static const char *url_Holiday = "https://api.xlongwei.com/service/datetime/holiday.json"; //节假日信息
static const char *url_Convert = "https://api.xlongwei.com/service/datetime/convert.json"; //农历信息

// 心知天气
static const char *url_Weather = "https://api.seniverse.com/v3/weather/daily.json?key={个人KEY}&location=上海&language=zh-Hans&unit=c&start=0&days=1";
static const char *url_LifeIndex = "https://api.seniverse.com/v3/life/suggestion.json?key={个人KEY}&location=上海&language=zh-Hans";

// 一言
static const char *url_Hitokoto = "https://v1.hitokoto.cn/?encode=json&charset=utf-8&max_length=25";

// 天气刷新间隔
#define REFRESH_FREQUENCY  8 //8小时
// 休眠时间
#define SLEEP_TIME 60  //60分钟

// 是否为夜间
bool gisNight = false;

//RTC变量地址
#define RTCdz_EpochTime_LastUpdate       0    //上次刷新时间
#define RTCdz_EpochTime_Now              1    //当前系统时间，非精准时间，通过推算获取

//日期信息结构
typedef struct _Time
{
  uint16_t year;
  uint8_t month;
  uint8_t day;
  uint8_t week; // 0 is Sunday
  uint8_t hour;
  uint8_t minute;
  uint8_t second;
}Time;

//日期信息结构
typedef struct _Date
{
  Time time; //时间
  char holiday[20]; //节日
  char holidayRemark[40]; //节日备注
  char convert[40]; //农历
  char shengxiao[8]; //生肖
  char ganzhi[16]; //干支
  bool isWorkday; //是否工作日
}Date;

//天气
typedef struct _Weather
{
  char status_code[64];       //错误代码
  char city[16];              //城市名称
  char last_update[25];       //最后更新时间

  char date[14];             //今天日期
  char date_text_day[20];    //白天天气现象名称
  char date_code_day[4];     //白天天气现象代码
  char date_text_night[16];  //晚上天气现象名称
  char date_code_night[4];   //晚上天气现象代码
  char date_high[5];         //最高温度
  char date_low[5];          //最低温度
  char date_humidity[5];     //相对湿度
  char date_wind_scale[5];   //风力等级
}Weather;

//生活指数
typedef struct _LifeIndex
{
  char status_code[64];  //错误代码
  char uvi[10];          //紫外线指数
}LifeIndex;

//一言API
typedef struct _Hitokoto
{
  char hitokoto[100]; //一言
  char from[64]; //出自
}Hitokoto;

static const char DefaultHitokoto[] = "Talk is cheap. Show me the code."; //默认信息
static const char DefaultHitokotoFrom[] = "Linus Torvalds";



void setup()
{
  Serial.begin(115200);

  //一些初始化
  display.init();
  ESP.wdtEnable(60000); //使能软件看门狗的触发间隔

  display.setRotation(0); //设置屏幕方向
  u8g2Fonts.begin(display); //将u8g2过程连接到Adafruit GFX  

  display_setup(); //开机信息显示

  Serial.printf("剩余堆空间：%d\n", ESP.getFreeHeap());//查看堆空间
  Serial.printf("sizeof(int)：%d, sizeof(long): %d\n", sizeof(int), sizeof(long));//

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
