//使用Json解析一言
static bool ParseHitokoto(String content, Hitokoto *pstHitokoto)
{
  DynamicJsonDocument json(1536); //分配内存,动态
  //content.replace("\\", "");
  //Serial.println(content);
  //content = content.substring(1, content.length()-1);
  DeserializationError error = deserializeJson(json, content); //解析json
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
