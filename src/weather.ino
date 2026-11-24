//使用Json解析天气数据，今天和未来2天
static bool ParseWeather(String content, Weather *pstWeather)
{
  StaticJsonDocument<1280> json; //静态分配，不占堆；12个单层键值字段，1280留余量
  //content.replace("\\", "");
  //Serial.println(content);
  //content = content.substring(1, content.length()-1);
  DeserializationError error = deserializeJson(json, content); //解析json
  if (error)
  {
    Serial.print("天气json配置失败:");
    Serial.println(error.f_str());
    return false;
  }
  serializeJsonPretty(json, Serial);
  Serial.println("");

  if (!json["location"].isNull())    //位置
  {
    strcpy_safe(pstWeather->location, sizeof(pstWeather->location), json["location"]);
  }
  else
  {
    json.clear();
    Serial.println("天气json配置失败");
    return false;
  }
  if (!json["text_day"].isNull())    //白天天气现象
  {
    strcpy_safe(pstWeather->text_day, sizeof(pstWeather->text_day), json["text_day"]);
  }
  if (!json["code_day"].isNull())    //白天天气代码
  {
    strcpy_safe(pstWeather->code_day, sizeof(pstWeather->code_day), json["code_day"]);
  }
  if (!json["text_night"].isNull())    //晚间天气现象
  {
    strcpy_safe(pstWeather->text_night, sizeof(pstWeather->text_night), json["text_night"]);
  }
  if (!json["code_night"].isNull())    //晚间天气代码
  {
    strcpy_safe(pstWeather->code_night, sizeof(pstWeather->code_night), json["code_night"]);
  }
  if (!json["high"].isNull())
  {
    strcpy_safe(pstWeather->high, sizeof(pstWeather->high), json["high"]);  //最高温度
  }
  if (!json["low"].isNull())             //最低温度
  {
    strcpy_safe(pstWeather->low, sizeof(pstWeather->low), json["low"]);
  }
  if (!json["humidity"].isNull())                //湿度
  {
    strcpy_safe(pstWeather->humidity, sizeof(pstWeather->humidity), json["humidity"]);
  }
  if (!json["wind_direction"].isNull())                //风向
  {
    strcpy_safe(pstWeather->wind_direction, sizeof(pstWeather->wind_direction), json["wind_direction"]);
  }
  if (!json["wind_speed"].isNull())                //风速，单位km/h
  {
    strcpy_safe(pstWeather->wind_speed, sizeof(pstWeather->wind_speed), json["wind_speed"]);
  }
  if (!json["dressing"].isNull())                //穿衣
  {
    strcpy_safe(pstWeather->dressing, sizeof(pstWeather->dressing), json["dressing"]);
  }
  if (!json["uv"].isNull())                //紫外线指数
  {
    strcpy_safe(pstWeather->uv, sizeof(pstWeather->uv), json["uv"]);
  }
  json.clear();
  return true;
}

extern bool GetWeather(Weather *pstWeather)
{
  char url[256+1]={0};
  char location[256+1]={0};

  if(!GetLocation(location))
  {
    return false;
  }

  sprintf(url, "%s/?ip=%s", url_Weather, location);
  String payload = callHttps(url);
  return ParseWeather(payload, pstWeather);
}
