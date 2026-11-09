//使用Json解析一言
static bool ParseHitokoto(const String &content, Hitokoto *pstHitokoto)
{
  StaticJsonDocument<512> json; //栈上静态分配，留足中文 UTF-8 余量，不占堆
  //content.replace("\\", "");
  //Serial.println(content);
  //content = content.substring(1, content.length()-1);
  //低堆/高碎片下传 const char* 走只读解析，避免 ArduinoJson 原地解析时分配失败
  DeserializationError error = deserializeJson(json, content.c_str()); //解析json
  if (error)
  {
    Serial.print("一言加载json配置失败:");
    Serial.println(error.f_str());
    return false;
  }
  serializeJsonPretty(json, Serial);
  Serial.println("");

  if(!json["hitokoto"].isNull())
  {
    strcpy(pstHitokoto->hitokoto, json["hitokoto"]);
  }
  else if(!json["poetry"].isNull())
  {
    strcpy(pstHitokoto->hitokoto, json["poetry"]);
  }
  else
  {
    json.clear();
    Serial.println("一言加载json配置失败");
    return false;
  }

  if(!json["from"].isNull())
  {
    strcpy(pstHitokoto->from, json["from"]);
  }
  else
  {
    strcpy(pstHitokoto->from, "佚名");
  }
  json.clear();
  return true;
}

extern bool GetHitokoto(Hitokoto *pstHitokoto)
{
  String payload = callHttps(url_Hitokoto);
  return ParseHitokoto(payload, pstHitokoto);
}
