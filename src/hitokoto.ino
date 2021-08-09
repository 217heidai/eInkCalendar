//使用Json解析一言
static bool ParseHitokoto(String content, Hitokoto *pstHitokoto)
{
  DynamicJsonDocument json(1536); //分配内存,动态
  DeserializationError error = deserializeJson(json, content); //解析json

  if (error)
  {
    Serial.print("一言加载json配置失败:");
    Serial.println(error.c_str());
    return false;
  }

  if(!json["hitokoto"].isNull())
  {
    strcpy(pstHitokoto->hitokoto, json["hitokoto"]);
  }

  if(!json["from"].isNull())
  {
    strcpy(pstHitokoto->from, json["from"]);
  }
  else
  {
    strcpy(pstHitokoto->from, "佚名");
  }

  return true;
}

extern bool GetHitokoto(Hitokoto *pstHitokoto)
{
  String payload = callHttps(url_Hitokoto);
  return ParseHitokoto(payload, pstHitokoto);
}