extern bool connectToWifi(void)
{
  uint8_t i = 0;
  WiFi.mode(WIFI_STA);
  WiFiMulti.addAP(ssid, password);//要连接的WiFi名称和密码
  String disp;
  uint8_t line = SCREEN_HEIGTH/FONT_SIZE_CHINESE_SPACING;

  disp = "Connecting to WiFi[" + String(ssid) + "]...";
  Serial.println(disp.c_str());
  while (WiFiMulti.run() != WL_CONNECTED) {
    if(++i > MAX_TRY_COUNT) break;
    delay(1000);
  }
  if (WiFiMulti.run() != WL_CONNECTED)
  {
    disp = "Connecting to WiFi[" + String(ssid) + "] failed";
    Serial.println(disp.c_str());
    //esp_sleep(SLEEP_TIME);
    return false;
  }
  disp = "Connected to WiFi[" + String(ssid) + "], IP: " + WiFi.localIP().toString();
  Serial.println(disp.c_str());

  return true;
}

extern String callHttps(const char *url)
{
  String payload;
  WiFiClientSecure client;
  HTTPClient https;

  client.setInsecure(); //不检验证书
  client.setBufferSizes(512, 256); //缓存大小
  https.setReuse(false); //是否keep-alive

  delay(1);
  Serial.printf("[HTTPS] begin... url: %s\n", url);
  if (https.begin(client, String(url)))
  {
    delay(1);
    int httpsCode = https.GET();
    delay(1);
    if (httpsCode > 0)  //判断有无返回值
    {
      Serial.printf("[HTTPS] GET... code: %d\n", httpsCode);
      if (httpsCode == 200 || httpsCode == 304 || httpsCode == 403 || httpsCode == 404 || httpsCode == 500) //判断请求是正确
      {
        payload = https.getString();
        Serial.println(payload);
        return payload;
      }
      else
      {
        //Serial.print("请求错误："); Serial.println(httpsCode); Serial.println(" ");
        payload = "{\"status_code\":\"" + String("请求错误:") + String(httpsCode) + "\"}";  //将错误值转换成json格式
        Serial.println(payload);
        return payload;
      }
    }
    else
    {
      //Serial.println(" "); Serial.print("GET请求错误："); Serial.println(httpsCode);
      //Serial.printf("[HTTPS] GET... 失败, 错误: %s\n", https.errorToString(httpsCode).c_str());
      payload = "{\"status_code\":\"" + String(https.errorToString(httpsCode).c_str()) + "\"}";  //将错误值转换成json格式
      Serial.println(payload);
      return payload;
    }
    https.end();
  }
  else
  {
    //Serial.printf("[HTTPS] 无法连接服务器\n");
    payload = "{\"status_code\":\"" + String("无法连接服务器") + "\"}";  //将错误值转换成json格式
    Serial.println(payload);
    return payload;
  }
}
