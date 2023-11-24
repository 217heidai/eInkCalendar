#define START_YEAR    (1970)
#define SECOND_DAY    (86400L)   //60*60*24
#define SECOND_HOUR   (3600)    //60*60
#define SECOND_MIN    (60)      //60

//获取NTP时间
extern bool GetNTPTime(unsigned long *pEpochTime)
{
  uint8_t update_count = 0;
  uint8_t hour;

  WiFiUDP ntpUDP;
  NTPClient timeClient(ntpUDP, "ntp.ntsc.ac.cn", 8 * 3600, 10); //udp，服务器地址，时间偏移量，更新间隔

  timeClient.begin();
  while (timeClient.forceUpdate() == 0 && update_count < 10)
  {
    update_count++;
    delay(100);
    if (update_count > 8) timeClient.setPoolServerName("s2k.time.edu.cn");
    else if (update_count > 7) timeClient.setPoolServerName("1d.time.edu.cn");
    else if (update_count > 6) timeClient.setPoolServerName("s1c.time.edu.cn");
    else if (update_count > 5) timeClient.setPoolServerName("ntp.sjtu.edu.cn");
    else if (update_count > 4) timeClient.setPoolServerName("s1a.time.edu.cn");
    else if (update_count > 3) timeClient.setPoolServerName("s1b.time.edu.cn");
    else if (update_count > 2) timeClient.setPoolServerName("ntp4.aliyun.com");
    else if (update_count > 1) timeClient.setPoolServerName("ntp3.aliyun.coms");
    else if (update_count > 0) timeClient.setPoolServerName("ntp2.aliyun.com");
  }

  *pEpochTime = timeClient.getEpochTime();
  hour = timeClient.getHours();
  if ((hour >= 8)&&(hour <= 20)) //天气预报“今天白天”是指上午8：00到晚上20：00这12个小时；“今天夜间”指20：00到次日早上8：00这12个小时。
  {
    gisNight = false;
  }
  else
  {
    gisNight = true;
  }
  timeClient.end();
  return true;
}

//使用Json解析日期
static bool ParseDate(String content, Date *pstDate)
{
  DynamicJsonDocument json(1536); //分配内存,动态
  DeserializationError error;

  pstDate->isWorkday = true; //默认为工作日
  //content.replace("\\", "");
  //content = content.substring(1, content.length()-1);
  //Serial.println(content);
  error = deserializeJson(json, content); //解析json
  if (error)
  {
    Serial.print("日期信息json配置失败:");
    Serial.println(error.f_str());
    return false;
  }
  serializeJsonPretty(json, Serial);
  Serial.println("");

  if(!json["workday"].isNull()) //是否工作日
  {
    if (json["workday"]==0)
    {
      pstDate->isWorkday = false;
    }
  }
  else
  {
    json.clear();
    Serial.println("日期信息json配置失败");
    return false;
  }
  if(!json["holiday"].isNull()) //节日信息
  {
    strcpy(pstDate->holiday, json["holiday"]);
  }
  if(!json["lunar_month"].isNull()) //农历月
  {
    strcpy(pstDate->convert, json["lunar_month"]);
  }
  if(!json["lunar_day"].isNull()) //农历日
  {
    strcpy(pstDate->convert+strlen(pstDate->convert), json["lunar_day"]);
  }
  if(!json["ganzhi_year"].isNull())//(干支)年柱
  {
    strcpy(pstDate->ganzhi, json["ganzhi_year"]);
  }
  if(!json["animal"].isNull())//生肖
  {
    strcpy(pstDate->shengxiao, json["animal"]);
  }
  if(!json["term"].isNull())//节气
  {
    strcpy(pstDate->term, json["term"]);
  }
  if(!json["yi"].isNull())//宜
  {
    strcpy(pstDate->yi, json["yi"]);
  }
  if(!json["ji"].isNull())//忌
  {
    strcpy(pstDate->ji, json["ji"]);
  }
  if(!json["lunar_festival"].isNull())//农历节日
  {
    strcpy(pstDate->lunar_festival, json["lunar_festival"]);
  }
  json.clear();
  return true;
}

extern bool isNeedRefresh(void)
{
  unsigned long RTC_EpochTime_LastUpdate, RTC_EpochTime_Now;
  Time stTime_LastUpdate, stTime;
  bool isRefresh = false;

  //估算当前时间
  ESP.rtcUserMemoryRead(RTCdz_EpochTime_Now, (uint32_t *)&RTC_EpochTime_Now, sizeof(RTC_EpochTime_Now));
  RTC_EpochTime_Now += SLEEP_TIME * 60 + 10; //增加10s的误差时间
  GetTime(RTC_EpochTime_Now, &stTime);
  Serial.printf("RTC Time: %04d/%02d/%02d %02d:%02d:%02d\n", stTime.year, stTime.month, stTime.day, stTime.hour, stTime.minute, stTime.second);
  if((stTime.year<2023)) //时间异常则强制更新
  {
    isRefresh = true;
    goto END;
  }
  ESP.rtcUserMemoryWrite(RTCdz_EpochTime_Now, (uint32_t *)&RTC_EpochTime_Now, sizeof(RTC_EpochTime_Now));//更新RTC时间

  //获取上次更新时间
  ESP.rtcUserMemoryRead(RTCdz_EpochTime_LastUpdate, (uint32_t *)&RTC_EpochTime_LastUpdate, sizeof(RTC_EpochTime_LastUpdate));
  GetTime(RTC_EpochTime_LastUpdate, &stTime_LastUpdate);
  Serial.printf("last update Time: %04d/%02d/%02d %02d:%02d:%02d\n", stTime_LastUpdate.year, stTime_LastUpdate.month, stTime_LastUpdate.day, stTime_LastUpdate.hour, stTime_LastUpdate.minute, stTime_LastUpdate.second);
  if((stTime_LastUpdate.year<2023)) //时间异常则强制更新
  {
    isRefresh = true;
    goto END;
  }

  //异常时间对比
  if(RTC_EpochTime_Now < RTC_EpochTime_LastUpdate + SLEEP_TIME * 60) //RTC数据异常，需要刷新
  {
    isRefresh = true;
    goto END;
  }

  //正常时间下的刷新规则
  //if((((stTime.hour - stTime_LastUpdate.hour)>= REFRESH_FREQUENCY) || (stTime.day != stTime_LastUpdate.day)) && ((stTime.hour > REFRESH_TIME_START) && (stTime.hour < REFRESH_TIME_END))) //距离上次刷新超过REFRESH_FREQUENCY小时，或者日期发生变化
  if(((stTime.hour - stTime_LastUpdate.hour)>= REFRESH_FREQUENCY) || (stTime.day != stTime_LastUpdate.day)) //距离上次刷新超过REFRESH_FREQUENCY小时，或者日期发生变化
  {
    isRefresh = true;
    goto END;
  }

END:
  return isRefresh;
}

extern bool GetDate(Date *pstDate)
{
  unsigned long ulEpochTime;
  String dateInfo;
  bool Result;

  //获取NTP时间
  if(GetNTPTime(&ulEpochTime))
  {
    GetTime(ulEpochTime, &(pstDate->time));
  }
  Serial.printf("NTP Time: %04d/%02d/%02d %02d:%02d:%02d\n", pstDate->time.year, pstDate->time.month, pstDate->time.day, pstDate->time.hour, pstDate->time.minute, pstDate->time.second);

  //更新RTC时间
  ESP.rtcUserMemoryWrite(RTCdz_EpochTime_Now, (uint32_t *)&ulEpochTime, sizeof(ulEpochTime));

  //获取日期信息
  dateInfo = callHttps(url_Date);

  return ParseDate(dateInfo, pstDate);
}

static const uint16_t mon_yday[][13] =
{
  /* Normal years.  */
  { 0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365 },
  /* Leap years.  */
  { 0, 31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335, 366 }
};
 
//判断一个年份是否为闰年，是就返回1，不是就返回0
static bool isLeapYear(uint16_t year)
{
  return( (year%4 == 0 && year%100 != 0) || (year%400 == 0) );
}
 
//获取一年的天数
static uint16_t getDaysForYear(uint16_t year)
{
  return (isLeapYear(year)?366:365);
}
 
//根据秒数计算日期时间
extern void GetTime(unsigned long UlEpochTime, Time *pstTime)
{
  uint16_t curYear = START_YEAR;
  unsigned long leftDays = UlEpochTime / SECOND_DAY;

  //calc year
  uint16_t daysCurYear = getDaysForYear(curYear);
  while (leftDays >= daysCurYear)
  {
    leftDays -= daysCurYear;
    curYear++;
    daysCurYear = getDaysForYear(curYear);
  }
  pstTime->year = curYear;

  //calc month and day
  bool isLeepYear = isLeapYear(curYear);
  for (int i = 1; i < 13; i++)
  {
    if (leftDays < mon_yday[isLeepYear][i])
    {
      pstTime->month = i;
      pstTime->day = leftDays - mon_yday[isLeepYear][i-1] + 1;
      break;
    }
  }

  pstTime->week = ((UlEpochTime  / SECOND_DAY) + 4 ) % 7; //0 is Sunday
  pstTime->hour = (UlEpochTime % SECOND_DAY) / SECOND_HOUR;
  pstTime->minute = (UlEpochTime % SECOND_HOUR) / 60;
  pstTime->second = (UlEpochTime % 60);
}