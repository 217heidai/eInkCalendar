//使用Json解析加密货币
static bool ParseCoin(String content, CryptoCoin *pstCryptoCoin)
{
  DynamicJsonDocument json(1536); //分配内存,动态
  //content.replace("\\", "");
  //Serial.println(content);
  //content = content.substring(1, content.length()-1);
  DeserializationError error = deserializeJson(json, content); //解析json
  if (error)
  {
    Serial.print("加密货币加载json配置失败:");
    Serial.println(error.f_str());
    return false;
  }
  serializeJsonPretty(json, Serial);
  Serial.println("");

  if(!json["BTC"].isNull())
  {
    strcpy(pstCryptoCoin->price_btc, json["BTC"]);
  }
  if(!json["ETH"].isNull())
  {
    strcpy(pstCryptoCoin->price_eth, json["ETH"]);
  }
  if(!json["BNB"].isNull())
  {
    strcpy(pstCryptoCoin->price_bnb, json["BNB"]);
  }
  if(!json["SOL"].isNull())
  {
    strcpy(pstCryptoCoin->price_sol, json["SOL"]);
  }

  json.clear();
  return true;
}

extern bool GetCoin(CryptoCoin *pstCryptoCoin)
{
  String payload = callHttps(url_Coin);
  return ParseCoin(payload, pstCryptoCoin);
}
