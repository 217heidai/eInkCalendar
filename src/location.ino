static bool ParseLocation(String content, char *pszLocation)
{
  DynamicJsonDocument json(1536); //分配内存,动态
  //content.replace("\\", "");
  //Serial.println(content);
  //content = content.substring(1, content.length()-1);
  DeserializationError error = deserializeJson(json, content); //解析json
  if (error)
  {
    Serial.print("ip定位加载json配置失败:");
    Serial.println(error.f_str());
    return false;
  }
  serializeJsonPretty(json, Serial);
  Serial.println("");

  if(!json["City"].isNull())
  {
    strcpy(pszLocation, json["City"]);
  }
  else
  {
    json.clear();
    Serial.println("ip定位加载json配置失败");
    return false;
  }

  json.clear();
  return true;
}

static bool _GetLocation(char *pszLocation)
{
  String payload = callHttps(url_location);
  return ParseLocation(payload, pszLocation);
}

extern bool GetLocation(char *pszLocation)
{
  uint8_t len_location=0;

  len_location = eeprom_read(EEPROM_ADDR_LOCATION, (uint8_t *)pszLocation);
  //len_location = 0; // for test
  if ((len_location < 1) || (len_location > LOCATION_CONFIG_MAX_LEN))
  {
    if(_GetLocation(pszLocation))
    {
      len_location = strlen((const char*)pszLocation);
      eeprom_write(EEPROM_ADDR_LOCATION, (uint8_t *)pszLocation, len_location);
      EEPROM.commit();
    }
    else
    {
      return false;
    }
  }
  
  return true;
}
