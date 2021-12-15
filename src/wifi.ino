ESP8266WiFiMulti WiFiMulti;

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

//https请求
extern String callHttps(const char *url)
{
  String payload = "";
  std::unique_ptr<BearSSL::WiFiClientSecure>client(new BearSSL::WiFiClientSecure);

  //client->setFingerprint(fingerprint);
  // Or, if you happy to ignore the SSL certificate, then use the following line instead:
  client->setInsecure();
  client->setBufferSizes(512, 512);

  HTTPClient https;
  https.setReuse(false); //是否keep-alive

  Serial.printf("[HTTPS] begin... url: %s\n", url);
  if (https.begin(*client, String(url))) {  // HTTPS
    // start connection and send HTTP header
    int httpCode = https.GET();
    Serial.printf("[HTTPS] GET... code: %d\n", httpCode);

    // httpCode will be negative on error
    if (httpCode > 0) {
      // file found at server
      if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
        payload = https.getString();
        Serial.println(payload);
      }
    } else {
      Serial.printf("[HTTPS] GET... failed, error: %s\n", https.errorToString(httpCode).c_str());
    }

    https.end();
  } else {
    Serial.printf("[HTTPS] Unable to connect\n");
  }

  return payload;
}
