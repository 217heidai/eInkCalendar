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
#define KEY_UP    5

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
//#include "u8g2_deng_56_temperature.c"    //温度显示字体
#include "u8g2_mfxinran_92_number.c"    //80*100
//#include "u8g2_mfyuehei_12_gb2312.c"    //16x16
//#include "u8g2_mfyuehei_14_gb2312.c"    //19x19
#include "u8g2_mfyuanhei_16_gb2312.c"   //21x21
//#include "u8g2_mfyuehei_18_gb2312.c"    //24x24
//声明外部变量
extern const uint8_t chinese_city_gb2312[239032] PROGMEM U8G2_FONT_SECTION("chinese_city_gb2312");
//extern const uint8_t u8g2_deng_56_temperature[1423] PROGMEM U8G2_FONT_SECTION("u8g2_deng_56_temperature");
extern const uint8_t u8g2_mfxinran_92_number[1240] PROGMEM U8G2_FONT_SECTION("u8g2_mfxinran_92_number");
//extern const uint8_t u8g2_mfyuehei_12_gb2312[310408] PROGMEM U8G2_FONT_SECTION("u8g2_mfyuehei_12_gb2312");
// const uint8_t u8g2_mfyuehei_14_gb2312[367573] PROGMEM U8G2_FONT_SECTION("u8g2_mfyuehei_14_gb2312");
extern const uint8_t u8g2_mfyuanhei_16_gb2312[334933] PROGMEM U8G2_FONT_SECTION("u8g2_mfyuanhei_16_gb2312");
//extern const uint8_t u8g2_mfyuehei_18_gb2312[527573] PROGMEM U8G2_FONT_SECTION("u8g2_mfyuehei_18_gb2312");
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
const String url_Workday = "https://api.xlongwei.com/service/datetime/isworkday.json"; //是否工作日
const String url_Holiday = "https://api.xlongwei.com/service/datetime/holiday.json"; //节假日信息
const String url_Convert = "https://api.xlongwei.com/service/datetime/convert.json"; //农历信息

// 心知天气
const String url_ActualWeather = "https://api.seniverse.com/v3/weather/now.json?key={个人KEY}&location=上海&language=zh-Hans&unit=c";
const String url_FutureWeather = "https://api.seniverse.com/v3/weather/daily.json?key={个人KEY}&location=上海&language=zh-Hans&unit=c&start=0&days=3";
const String url_LifeIndex = "https://api.seniverse.com/v3/life/suggestion.json?key={个人KEY}&location=上海&language=zh-Hans";

// 一言
const String url_Hitokoto = "https://v1.hitokoto.cn/?encode=json&charset=utf-8&max_length=25";

// 天气刷新间隔
#define REFRESH_FREQUENCY 60  //每60分钟刷新一次

// 是否为夜间
bool isNight = false;


//日期信息结构
typedef struct _DateTime
{
  uint16_t year;
  uint8_t month;
  uint8_t day;
  uint8_t week; // 0 is Sunday
  char holiday[20]; //节日
  char holidayRemark[40]; //节日备注
  char convert[40]; //农历
  char shengxiao[8]; //生肖
  char ganzhi[16]; //干支
  bool isWorkday; //是否工作日
}DateTime;

//今日天气
typedef struct _ActualWeather
{
  char status_code[64];  //错误代码
  char city[16];         //城市名称
  char weather_name[16]; //天气现象名称
  char weather_code[4];  //天气现象代码
  char temp[5];          //温度
  char last_update[25];  //最后更新时间
}ActualWeather;

//未来天气
typedef struct _FutureWeather
{
  char status_code[64];       //错误代码
  char city[16];              //城市名称
  char last_update[25];       //最后更新时间

  char date0[14];             //今天日期
  char date0_text_day[20];    //白天天气现象名称
  char date0_code_day[4];     //白天天气现象代码
  char date0_text_night[16];  //晚上天气现象名称
  char date0_code_night[4];   //晚上天气现象代码
  char date0_high[5];         //最高温度
  char date0_low[5];          //最低温度
  char date0_humidity[5];     //相对湿度
  char date0_wind_scale[5];   //风力等级

  char date1[14];             //明天日期
  char date1_text_day[20];    //白天天气现象名称
  char date1_code_day[4];     //白天天气现象代码
  char date1_text_night[16];  //晚上天气现象名称
  char date1_code_night[4];   //晚上天气现象代码
  char date1_high[5];         //最高温度
  char date1_low[5];          //最低温度

  char date2[14];             //后天日期
  char date2_text_day[20];    //白天天气现象名称
  char date2_code_day[4];     //白天天气现象代码
  char date2_text_night[16];  //晚上天气现象名称
  char date2_code_night[4];   //晚上天气现象代码
  char date2_high[5];         //最高温度
  char date2_low[5];          //最低温度
}FutureWeather;

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
const String DefaultHitokoto = "当你怀疑人生的时候，其实这就是你的人生。"; //默认信息

//RTC临时数据
#define RTCdz_hour 0           //小时地址
#define RTCdz_night_count 1    //夜间计数地址
#define RTCdz_peiwang_state 2  //配网状态地址
uint32_t RTC_hour = 100;        //小时
uint32_t RTC_night_count = 0;   //24-6点，夜间不更新计数
int32_t night_count_max = 0;    //需要跳过几次

void setup()
{
  Serial.begin(115200);

  //一些初始化
  display.init();
  ESP.wdtEnable(60000); //使能软件看门狗的触发间隔

  display.setRotation(0); //设置屏幕方向
  u8g2Fonts.begin(display); //将u8g2过程连接到Adafruit GFX  

  display_setup(); //开机信息显示

  //led_controller(1); //点亮led

  if(!connectToWifi()) goto ESP_SLEEP;//连接wifi

  display_MainPage(); //显示主页

ESP_SLEEP:
  //led_controller(0); //熄灭led
  esp_sleep(REFRESH_FREQUENCY); //休眠REFRESH_FREQUENCY后重启
}

void loop()
{

}
