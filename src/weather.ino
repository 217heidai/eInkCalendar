//使用Json解析天气数据，今天和未来2天
static bool ParseWeather(String content, Weather *pstWeather)
{
  DynamicJsonDocument json(2560); //分配内存,动态
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
    strcpy(pstWeather->location, json["location"]);
  }
  else
  {
    json.clear();
    Serial.println("天气json配置失败");
    return false;
  }
  if (!json["text_day"].isNull())    //白天天气现象
  {
    strcpy(pstWeather->text_day, json["text_day"]);
  }
  if (!json["code_day"].isNull())    //白天天气代码
  {
    strcpy(pstWeather->code_day, json["code_day"]);
  }
  if (!json["text_night"].isNull())    //晚间天气现象
  {
    strcpy(pstWeather->text_night, json["text_night"]);
  }
  if (!json["code_night"].isNull())    //晚间天气代码
  {
    strcpy(pstWeather->code_night, json["code_night"]);
  }
  if (!json["high"].isNull())
  {
    strcpy(pstWeather->high, json["high"]);  //最高温度
  }
  if (!json["low"].isNull())             //最低温度
  {
    strcpy(pstWeather->low, json["low"]);
  }
  if (!json["humidity"].isNull())                //湿度
  {
    strcpy(pstWeather->humidity, json["humidity"]);
  }
  if (!json["wind_direction"].isNull())                //风向
  {
    strcpy(pstWeather->wind_direction, json["wind_direction"]);
  }
  if (!json["wind_speed"].isNull())                //风速，单位km/h
  {
    strcpy(pstWeather->wind_speed, json["wind_speed"]);
  }
  if (!json["dressing"].isNull())                //穿衣
  {
    strcpy(pstWeather->dressing, json["dressing"]);
  }
  if (!json["uv"].isNull())                //紫外线指数
  {
    strcpy(pstWeather->uv, json["uv"]);
  }
  json.clear();
  return true;
}

extern bool GetWeather(Weather *pstWeather)
{
  String payload = callHttps(url_Weather);
  return ParseWeather(payload, pstWeather);
}
