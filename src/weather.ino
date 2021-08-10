//使用Json解析天气数据，天气实况
static bool ParseActualWeather(String content, ActualWeather *pstActualWeather)
{
  DynamicJsonDocument json(1536); //分配内存,动态
  DeserializationError error = deserializeJson(json, content); //解析json

  if (error)
  {
    Serial.print("实况天气加载json配置失败:");
    Serial.println(error.c_str());
    return false;
  }

  //检查API是否有返回错误信息，有返回则进入休眠
  if (json["status_code"].isNull() == 0) //检查到不为空
  {
    strcpy(pstActualWeather->status_code, json["status_code"]);
    String errmsg = "实况天气获取异常: " + String(pstActualWeather->status_code);
    Serial.println(errmsg.c_str());
    return false;
  }

  // 复制我们感兴趣的字符串 ,先检查是否为空，空会导致系统无限重启
  // isNull()检查是否为空 空返回1 非空0
  if (json["results"][0]["location"]["name"].isNull() == 0)
    strcpy(pstActualWeather->city, json["results"][0]["location"]["name"]);
  if (json["results"][0]["now"]["text"].isNull() == 0)
    strcpy(pstActualWeather->weather_name, json["results"][0]["now"]["text"]);
  if (json["results"][0]["now"]["code"].isNull() == 0)
    strcpy(pstActualWeather->weather_code, json["results"][0]["now"]["code"]);
  if (json["results"][0]["now"]["temperature"].isNull() == 0)
    strcpy(pstActualWeather->temp, json["results"][0]["now"]["temperature"]);
  if (json["results"][0]["last_update"].isNull() == 0)
    strcpy(pstActualWeather->last_update, json["results"][0]["last_update"]);
  // 这不是强制复制，你可以使用指针，因为他们是指向"内容"缓冲区内，所以你需要确保
  // 当你读取字符串时它仍在内存中
  return true;
}

//使用Json解析天气数据，今天和未来2天
static bool ParseFutureWeather(String content, FutureWeather *pstFutureWeather)
{
  DynamicJsonDocument json(2560); //分配内存,动态
  DeserializationError error = deserializeJson(json, content); //解析json

  if (error)
  {
    Serial.print("未来天气json配置失败:");
    Serial.println(error.c_str());
    return false;
  }

  //检查API是否有返回错误信息，有返回则进入休眠
  if (json["status_code"].isNull() == 0) //检查到不为空
  {
    strcpy(pstFutureWeather->status_code, json["status_code"]);
    String errmsg = "未来天气获取异常: " + String(pstFutureWeather->status_code);
    Serial.println(errmsg.c_str());
    return false;
  }

  // 复制我们感兴趣的字符串，先检查是否为空，空会复制失败导致系统无限重启
  if (json["results"][0]["location"]["name"].isNull() == 0)  //城市名
    strcpy(pstFutureWeather->city, json["results"][0]["location"]["name"]);
  if (json["results"][0]["last_update"].isNull() == 0)       //更新时间
    strcpy(pstFutureWeather->last_update, json["results"][0]["last_update"]);

  if (json["results"][0]["daily"][0]["date"].isNull() == 0)        //日期
    strcpy(pstFutureWeather->date0, json["results"][0]["daily"][0]["date"]);
  if (json["results"][0]["daily"][1]["date"].isNull() == 0)
    strcpy(pstFutureWeather->date1, json["results"][0]["daily"][1]["date"]);
  if (json["results"][0]["daily"][2]["date"].isNull() == 0)
    strcpy(pstFutureWeather->date2, json["results"][0]["daily"][2]["date"]);

  if (json["results"][0]["daily"][0]["text_day"].isNull() == 0)    //白天天气现象
    strcpy(pstFutureWeather->date0_text_day, json["results"][0]["daily"][0]["text_day"]);
  if (json["results"][0]["daily"][1]["text_day"].isNull() == 0)
    strcpy(pstFutureWeather->date1_text_day, json["results"][0]["daily"][1]["text_day"]);
  if (json["results"][0]["daily"][2]["text_day"].isNull() == 0)
    strcpy(pstFutureWeather->date2_text_day, json["results"][0]["daily"][2]["text_day"]);

  if (json["results"][0]["daily"][0]["code_day"].isNull() == 0)    //白天天气代码
    strcpy(pstFutureWeather->date0_code_day, json["results"][0]["daily"][0]["code_day"]);
  if (json["results"][0]["daily"][1]["code_day"].isNull() == 0)
    strcpy(pstFutureWeather->date1_code_day, json["results"][0]["daily"][1]["code_day"]);
  if (json["results"][0]["daily"][2]["code_day"].isNull() == 0)
    strcpy(pstFutureWeather->date2_code_day, json["results"][0]["daily"][2]["code_day"]);   

  if (json["results"][0]["daily"][0]["text_night"].isNull() == 0)    //晚间天气现象
    strcpy(pstFutureWeather->date0_text_night, json["results"][0]["daily"][0]["text_night"]);
  if (json["results"][0]["daily"][1]["text_night"].isNull() == 0)
    strcpy(pstFutureWeather->date1_text_night, json["results"][0]["daily"][1]["text_night"]);
  if (json["results"][0]["daily"][2]["text_night"].isNull() == 0)
    strcpy(pstFutureWeather->date2_text_night, json["results"][0]["daily"][2]["text_night"]);

  if (json["results"][0]["daily"][0]["code_night"].isNull() == 0)    //晚间天气代码
    strcpy(pstFutureWeather->date0_code_night, json["results"][0]["daily"][0]["code_night"]);
  if (json["results"][0]["daily"][1]["code_night"].isNull() == 0)
    strcpy(pstFutureWeather->date1_code_night, json["results"][0]["daily"][1]["code_night"]);
  if (json["results"][0]["daily"][2]["code_night"].isNull() == 0)
    strcpy(pstFutureWeather->date2_code_night, json["results"][0]["daily"][2]["code_night"]);

  if (json["results"][0]["daily"][0]["high"].isNull() == 0)
    strcpy(pstFutureWeather->date0_high, json["results"][0]["daily"][0]["high"]);  //最高温度
  if (json["results"][0]["daily"][1]["high"].isNull() == 0)
    strcpy(pstFutureWeather->date1_high, json["results"][0]["daily"][1]["high"]);
  if (json["results"][0]["daily"][2]["high"].isNull() == 0)
    strcpy(pstFutureWeather->date2_high, json["results"][0]["daily"][2]["high"]);

  if (json["results"][0]["daily"][0]["low"].isNull() == 0)             //最低温度
    strcpy(pstFutureWeather->date0_low, json["results"][0]["daily"][0]["low"]);
  if (json["results"][0]["daily"][1]["low"].isNull() == 0)
    strcpy(pstFutureWeather->date1_low, json["results"][0]["daily"][1]["low"]);
  if (json["results"][0]["daily"][2]["low"].isNull() == 0)
    strcpy(pstFutureWeather->date2_low, json["results"][0]["daily"][2]["low"]);

  if (json["results"][0]["daily"][0]["humidity"].isNull() == 0)                //湿度
    strcpy(pstFutureWeather->date0_humidity, json["results"][0]["daily"][0]["humidity"]);

  if (json["results"][0]["daily"][0]["wind_scale"].isNull() == 0)        //风力等级
    strcpy(pstFutureWeather->date0_wind_scale, json["results"][0]["daily"][0]["wind_scale"]);
  return true;
}


//使用Json解析天气数据，天气实况
static bool ParseLifeIndex(String content, LifeIndex *pstLifeIndex)
{
  DynamicJsonDocument json(1536); //分配内存,动态
  DeserializationError error = deserializeJson(json, content); //解析json

  if (error)
  {
    Serial.print("天气指数加载json配置失败:");
    Serial.println(error.c_str());
    return false;
  }

  //检查API是否有返回错误信息，有返回则进入休眠
  if (json["status_code"].isNull() == 0) //检查到不为空
  {
    strcpy(pstLifeIndex->status_code, json["status_code"]);
    String errmsg = "天气指数获取异常: " + String(pstLifeIndex->status_code);
    Serial.println(errmsg.c_str());
    return false;
  }

  // 复制我们感兴趣的字符串 ,先检查是否为空，空会导致系统无限重启
  // isNull()检查是否为空 空返回1 非空0
  if (json["results"][0]["suggestion"]["uv"]["brief"].isNull() == 0)          //紫外线指数
    strcpy(pstLifeIndex->uvi, json["results"][0]["suggestion"]["uv"]["brief"]);
  return true;
}

extern bool GetActualWeather(ActualWeather *pstActualWeather)
{
  String payload = callHttps(url_ActualWeather);
  return ParseActualWeather(payload, pstActualWeather);
}

extern bool GetFutureWeather(FutureWeather *pstFutureWeather)
{
  String payload = callHttps(url_FutureWeather);
  return ParseFutureWeather(payload, pstFutureWeather);
}

extern bool GetLifeIndex(LifeIndex *pstLifeIndex)
{
  String payload = callHttps(url_LifeIndex);
  return ParseLifeIndex(payload, pstLifeIndex);
}