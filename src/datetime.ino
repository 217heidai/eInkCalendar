//获取NTP时间
static bool GetNTPTime(unsigned long *pEpochTime)
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
  timeClient.end();
  return true;
}

//使用Json解析日期
static bool ParseDate(String workday, String holiday, String convert, Date *pstDate)
{
  DynamicJsonDocument json(1536); //分配内存,动态
  DeserializationError error;

  while(true)
  {
    error = deserializeJson(json, workday); //解析json
    if (error) break;
    pstDate->isWorkday = true;
    if(!json["isworkday"].isNull())
    {
      if (!json["isworkday"])
      {
        pstDate->isWorkday = false;
      }
    }

    error = deserializeJson(json, holiday); //解析json
    if (error) break;
    if(!json["holiday"].isNull())
    {
      strcpy(pstDate->holiday, json["holiday"]);
    }
    if(!json["remark"].isNull())
    {
      strcpy(pstDate->holidayRemark, json["remark"]);
    }
    
    error = deserializeJson(json, convert); //解析json
    if (error) break;
    if(!json["chinese"].isNull())
    {
      strcpy(pstDate->convert, json["chinese"]);
    }
    if(!json["shengxiao"].isNull())
    {
      strcpy(pstDate->shengxiao, json["shengxiao"]);
    }
    if(!json["ganzhi"].isNull())
    {
      strcpy(pstDate->ganzhi, json["ganzhi"]);
    }

    break;
  }

  if (error)
  {
    Serial.print("日期信息json配置失败:");
    Serial.println(error.c_str());
    return false;
  }
  
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
  ESP.rtcUserMemoryWrite(RTCdz_EpochTime_Now, (uint32_t *)&RTC_EpochTime_Now, sizeof(RTC_EpochTime_Now));//更新RTC时间

  //获取上次更新时间
  ESP.rtcUserMemoryRead(RTCdz_EpochTime_LastUpdate, (uint32_t *)&RTC_EpochTime_LastUpdate, sizeof(RTC_EpochTime_LastUpdate));
  //Serial.printf("RTC_EpochTime_LastUpdate: %lu\n", RTC_EpochTime_LastUpdate);
  GetTime(RTC_EpochTime_LastUpdate, &stTime_LastUpdate);
  Serial.printf("last update Time: %04d/%02d/%02d %02d:%02d:%02d\n", stTime_LastUpdate.year, stTime_LastUpdate.month, stTime_LastUpdate.day, stTime_LastUpdate.hour, stTime_LastUpdate.minute, stTime_LastUpdate.second);

  //RTC数据异常，需要刷新
  if(RTC_EpochTime_Now <= RTC_EpochTime_LastUpdate)
  {
    isRefresh = true;
  }
  //日期变化，需要刷新
  if((stTime.year != stTime_LastUpdate.year) || (stTime.month != stTime_LastUpdate.month) || (stTime.day != stTime_LastUpdate.day))
  {
    isRefresh = true;
  }
  //距离上次刷新超过8小时，需要刷新
  if((stTime.hour - stTime_LastUpdate.hour)>= REFRESH_FREQUENCY) //距离上次刷新超过REFRESH_FREQUENCY小时
  {
    isRefresh = true;
  }
  
  return isRefresh;
}

extern bool GetDate(Date *pstDate)
{
  unsigned long ulEpochTime;
  String workday, holiday, convert;

  //获取NTP时间
  if(GetNTPTime(&ulEpochTime))
  {
    GetTime(ulEpochTime, &(pstDate->time));
  }
  Serial.printf("NTP Time: %04d/%02d/%02d %02d:%02d:%02d\n", pstDate->time.year, pstDate->time.month, pstDate->time.day, pstDate->time.hour, pstDate->time.minute, pstDate->time.second);

  //更新RTC时间
  ESP.rtcUserMemoryWrite(RTCdz_EpochTime_Now, (uint32_t *)&ulEpochTime, sizeof(ulEpochTime));
  //更新刷新时间
  ESP.rtcUserMemoryWrite(RTCdz_EpochTime_LastUpdate, (uint32_t *)&ulEpochTime, sizeof(ulEpochTime));

  //获取日期信息
  workday = callHttps(url_Workday);
  holiday = callHttps(url_Holiday);
  convert = callHttps(url_Convert);
  return ParseDate(workday, holiday, convert, pstDate);
}

#define START_YEAR    (1970)
#define SECOND_DAY    (86400L)   //60*60*24
#define SECOND_HOUR   (3600)    //60*60
#define SECOND_MIN    (60)      //60

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
static void GetTime(unsigned long UlEpochTime, Time *pstTime)
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