//获取NTP时间
static bool GetNTPTime(unsigned long *pEpochTime, uint8_t *pweek)
{
  uint8_t update_count = 0;
  uint8_t hour;

  WiFiUDP ntpUDP;
  NTPClient timeClient(ntpUDP, "time.pool.aliyun.com", 8 * 3600, 10); //udp，服务器地址，时间偏移量，更新间隔

  Serial.print("Get ntp time: ");
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

  Serial.println(timeClient.getFormattedTime());
  *pEpochTime = timeClient.getEpochTime();
  *pweek = timeClient.getDay();
  hour = timeClient.getHours();
  if ((hour >= 8)&&(hour <= 20)) //天气预报“今天白天”是指上午8：00到晚上20：00这12个小时；“今天夜间”指20：00到次日早上8：00这12个小时。
  {
    isNight = false;
  }
  timeClient.end();
  return true;
}

//使用Json解析日期
static bool ParseDateTime(String workday, String holiday, String convert, DateTime *pstDateTime)
{
  DynamicJsonDocument json(1536); //分配内存,动态
  DeserializationError error;

  while(true)
  {
    error = deserializeJson(json, workday); //解析json
    if (error) break;
    pstDateTime->isWorkday = true;
    if(!json["isworkday"].isNull())
    {
      if (!json["isworkday"])
      {
        pstDateTime->isWorkday = false;
      }
    }

    error = deserializeJson(json, holiday); //解析json
    if (error) break;
    if(!json["holiday"].isNull())
    {
      strcpy(pstDateTime->holiday, json["holiday"]);
    }
    if(!json["remark"].isNull())
    {
      strcpy(pstDateTime->holidayRemark, json["remark"]);
    }
    
    error = deserializeJson(json, convert); //解析json
    if (error) break;
    if(!json["chinese"].isNull())
    {
      strcpy(pstDateTime->convert, json["chinese"]);
    }
    if(!json["shengxiao"].isNull())
    {
      strcpy(pstDateTime->shengxiao, json["shengxiao"]);
    }
    if(!json["ganzhi"].isNull())
    {
      strcpy(pstDateTime->ganzhi, json["ganzhi"]);
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

extern bool GetDatetime(DateTime *pstDateTime)
{
  unsigned long EpochTime;
  String workday, holiday, convert;

  //获取NTP时间
  if(GetNTPTime(&EpochTime, &(pstDateTime->week)))
  {
    getDate(EpochTime, &(pstDateTime->year), &(pstDateTime->month), &(pstDateTime->day));
  }
  Serial.printf("date: %d-%d-%d, week: %d\n", pstDateTime->year, pstDateTime->month, pstDateTime->day, pstDateTime->week);

  //获取日期信息
  workday = callHttps(url_Workday);
  holiday = callHttps(url_Holiday);
  convert = callHttps(url_Convert);
  return ParseDateTime(workday, holiday, convert, pstDateTime);
}

#define START_YEAR    (1970)
#define SECOND_DAY    (86400)   //60*60*24
#define SECOND_HOUR   (3600)    //60*60
#define SECOND_MIN    (60)      //60

const uint16_t mon_yday[][13] =
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
 
//根据秒数计算日期
static void getDate(unsigned long Second, uint16_t *pYear, uint8_t *pMonth, uint8_t *pDay)
{
  unsigned long days = Second / SECOND_DAY;
  uint16_t curYear = START_YEAR;
  unsigned long leftDays = days;

  //calc year
  uint16_t daysCurYear = getDaysForYear(curYear);
  while (leftDays >= daysCurYear)
  {
    leftDays -= daysCurYear;
    curYear++;
    daysCurYear = getDaysForYear(curYear);
  }
  *pYear = curYear;

  //calc month and day
  bool isLeepYear = isLeapYear(curYear);
  for (int i = 1; i < 13; i++)
  {
    if (leftDays < mon_yday[isLeepYear][i])
    {
      *pMonth = i;
      *pDay = leftDays - mon_yday[isLeepYear][i-1] + 1;
      break;
    }
  }
}