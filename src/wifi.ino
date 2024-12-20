extern bool connectToWifi(void)
{
  uint8_t i = 0;
  WiFi.mode(WIFI_STA);
  WiFiMulti.addAP(ssid, password);//要连接的WiFi名称和密码
  String disp;
  uint8_t line = SCREEN_HEIGTH/FONT_SIZE_CHINESE_SPACING;

  Serial.printf("Connecting to WiFi[\"%s\"]...", ssid);
  while (WiFiMulti.run() != WL_CONNECTED) {
    Serial.printf(".");
    if(++i > MAX_TRY_COUNT) break;
    delay(1000);
  }
  Serial.printf("\n");
  if (WiFiMulti.run() != WL_CONNECTED)
  {
    Serial.printf("Connecting to WiFi[\"%s\"] failed\n", ssid);
    return false;
  }
  Serial.printf("Connecting to WiFi[\"%s\"], IP: %s\n", ssid, WiFi.localIP().toString().c_str());

  return true;
}

extern String callHttps(const char *url)
{
  String payload;
  std::unique_ptr<BearSSL::WiFiClientSecure> client(new BearSSL::WiFiClientSecure);
  HTTPClient https;
  int httpsCode;

  if ((WiFiMulti.run() != WL_CONNECTED))
  {
    payload = "{\"status_code\":\"" + String("WIFI连接断开") + "\"}";  //将错误值转换成json格式
    goto END;
  }

  client->setInsecure(); //不检验证书
  client->setBufferSizes(1024, 512); //缓存大小
  client->setSSLVersion(BR_TLS12, BR_TLS12);
  //client->setCiphersLessSecure();
  https.setReuse(false); //是否keep-alive
  https.setTimeout(20000); //超时时间20s

  Serial.printf_P(PSTR("Before: free_heap %d, max_alloc_heap %d, heap_fragmentation  %d\n"), ESP.getFreeHeap(), ESP.getMaxFreeBlockSize(), ESP.getHeapFragmentation());
  Serial.printf("[HTTPS] begin... url: %s\n", url);
  if (https.begin(*client, String(url)))
  {
    httpsCode = https.GET();
    if (httpsCode > 0)  //判断有无返回值
    {
      Serial.printf("[HTTPS] GET... code: %d\n", httpsCode);
      if (httpsCode == HTTP_CODE_OK) //判断请求是正确
      {
        payload = https.getString();
        goto END;
      }
      else
      {
        payload = "{\"status_code\":\"" + String("请求错误:") + String(httpsCode) + "\"}";  //将错误值转换成json格式
        goto END;
      }
    }
    else
    {
      payload = "{\"status_code\":\"" + String(https.errorToString(httpsCode).c_str()) + "\"}";  //将错误值转换成json格式
      goto END;
    }
  }
  else
  {
    payload = "{\"status_code\":\"" + String("无法连接服务器") + "\"}";  //将错误值转换成json格式
    goto END;
  }

END:
  https.end();
  Serial.println(payload);
  return payload;
}
