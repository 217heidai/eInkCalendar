static void display_smartConfig(void)
{
  char disp[256];
  uint16 dataWidth = 0;

  display.firstPage();
  do
  {
    display.fillScreen(COLOR_WHITE);  // 填充屏幕

    // 配网二维码
    display.drawInvertedBitmap((SCREEN_WIDTH - 200) / 2, (SCREEN_HEIGTH - 200) / 2, Bitmap_QRcode, 200, 200, COLOR_BLACK);

    // 标题
    u8g2Fonts.setFont(u8g2_sarasa_16_gb2312);
    sprintf(disp, "微信扫码配置网络");
    Serial.println(disp);
    dataWidth = u8g2Fonts.getUTF8Width(disp);
    u8g2Fonts.drawUTF8((SCREEN_WIDTH - dataWidth) / 2, (SCREEN_HEIGTH - 200) / 2 - 2, disp);

    // 操作提示
    u8g2Fonts.setFont(chinese_city_gb2312);
    sprintf(disp, "确保手机连接到2.4GHz WiFi");
    Serial.println(disp);
    dataWidth = u8g2Fonts.getUTF8Width(disp);
    u8g2Fonts.drawUTF8((SCREEN_WIDTH - dataWidth) / 2, SCREEN_HEIGTH -(SCREEN_HEIGTH - 200) / 2 + FONT_SIZE_CHINESE_SPACING, disp);
    sprintf(disp, "请在3分钟内完成网络配置");
    Serial.println(disp);
    dataWidth = u8g2Fonts.getUTF8Width(disp); // 与第一行对齐
    u8g2Fonts.drawUTF8((SCREEN_WIDTH - dataWidth) / 2, SCREEN_HEIGTH -(SCREEN_HEIGTH - 200) / 2 + FONT_SIZE_CHINESE_SPACING*2, disp);
  }
  while (display.nextPage());
}

static void display_smartConfig_fail(void)
{
  char disp[256];
  uint16 dataWidth = 0;

  display.firstPage();
  do
  {
    display.fillScreen(COLOR_WHITE);  // 填充屏幕

    // 标题
    u8g2Fonts.setFont(u8g2_sarasa_16_gb2312);
    sprintf(disp, "配网失败，请重启再试");
    dataWidth = u8g2Fonts.getUTF8Width(disp);
    u8g2Fonts.drawUTF8((SCREEN_WIDTH - dataWidth) / 2, SCREEN_HEIGTH / 2, disp);

    // 操作提示
    u8g2Fonts.setFont(chinese_city_gb2312);
    sprintf(disp, "确保手机连接到2.4GHz WiFi");
    dataWidth = u8g2Fonts.getUTF8Width(disp);
    u8g2Fonts.drawUTF8((SCREEN_WIDTH - dataWidth) / 2, SCREEN_HEIGTH / 2 + FONT_SIZE_CHINESE_SPACING, disp);
    sprintf(disp, "请在3分钟内完成网络配置");
    dataWidth = u8g2Fonts.getUTF8Width(disp); // 与第一行对齐
    u8g2Fonts.drawUTF8((SCREEN_WIDTH - dataWidth) / 2, SCREEN_HEIGTH / 2 + FONT_SIZE_CHINESE_SPACING*2, disp);
  }
  while (display.nextPage());
}

static bool smartConfig(uint8_t *pssid, uint8_t *ppasswd)
{
  bool isStart = false;
  uint8_t len = 0;
  String ssid;
  String passwd;
  uint8_t i = 0;
  bool isSuccess = false;
  
  //display_smartConfig();

  // 设置WiFi模式
  WiFi.mode(WIFI_STA);
  if(!WiFi.getAutoConnect()){
    WiFi.setAutoConnect(true);
  }
  
  // 开始SmartConfig
  isStart = WiFi.beginSmartConfig();
  if (isStart)
  {
    Serial.println("SmartConfig已启动，等待配网数据...");
    Serial.println("配网超时时间: 3分钟");
    for (i = 0; i < WIFI_CONFIG_MAX_COUNT; i++)
    {
      delay(1000);
      // 检查配网是否完成
      if (WiFi.smartConfigDone())
      {
        ssid = WiFi.SSID();
        strcpy((char*)pssid, ssid.c_str());
        passwd = WiFi.psk();
        strcpy((char*)ppasswd, passwd.c_str());
        
        // 保存配置
        len = strlen((const char*)pssid);
        eeprom_write(&len, EEPROM_ADDR_SSID_LEN, 1);
        eeprom_write(pssid, EEPROM_ADDR_SSID, len);
        len = strlen((const char*)ppasswd);
        eeprom_write(&len, EEPROM_ADDR_PASSWD_LEN, 1);
        eeprom_write(ppasswd, EEPROM_ADDR_PASSWD, len);
        EEPROM.commit();
        
        isSuccess = true;
        break;
      }
    }

    // 停止SmartConfig
    while (! WiFi.stopSmartConfig())
    {
      delay(500);
    }

    if (isSuccess)
    {
      Serial.println("\n配网成功！");
      Serial.printf("正在连接WiFi: %s\n", ssid);
      Serial.print("连接中");
    }
    else
    {
      Serial.println("\n配网超时!");
    }
  }
  else
  {
    Serial.println("启动SmartConfig失败!");
  }

  return isSuccess;
}

extern bool connectToWifi(void)
{
  uint8_t len_ssid = 0;
  uint8_t len_passwd = 0;
  uint8_t ssid[WIFI_CONFIG_MAX_LEN + 1] = {0};
  uint8_t passwd[WIFI_CONFIG_MAX_LEN + 1] = {0};
  uint8_t i = 0;
  bool isSuccess = true; //可能不需要配网，所以默认成功

  EEPROM.begin(EEPROM_SIZE);

RETRY_CONNECT:
  // 读取EEPROM中的SSID和密码
  eeprom_read(&len_ssid, EEPROM_ADDR_SSID_LEN, 1);
  eeprom_read(&len_passwd, EEPROM_ADDR_PASSWD_LEN, 1);
  //len_ssid = 0; // for test
  if ((len_ssid < 1) || (len_ssid > WIFI_CONFIG_MAX_LEN) || (len_passwd < 1) || (len_passwd > WIFI_CONFIG_MAX_LEN)) //配网
  {
    display_smartConfig();
    led_controller(1); // 放在 display  前会导致Busy Timeout不显示，原因不明
    isSuccess = smartConfig(ssid, passwd);
    led_controller(0);

    if (!isSuccess)
    {
      EEPROM.end();
      display_smartConfig_fail();
      return false;
    }
  }
  else
  {
    eeprom_read(ssid, EEPROM_ADDR_SSID, len_ssid);
    eeprom_read(passwd, EEPROM_ADDR_PASSWD, len_passwd);
    Serial.printf("\n正在连接WiFi: %s\n", ssid);
    Serial.print("连接中");
    WiFi.begin((const char *)ssid, (const char *)passwd);
  }
  
  for (i = 0; i < WIFI_CONFIG_MAX_COUNT; i++)
  {
    delay(500);
    if (WiFi.status() == WL_CONNECTED)
    {
      Serial.println("\nWiFi连接成功！");
      Serial.print("IP地址: ");
      Serial.println(WiFi.localIP());
      EEPROM.end();
      return true;
    }
  }

  ESP.wdtFeed();//喂狗
  goto RETRY_CONNECT;
}

extern String callHttps(const char *url)
{
  String payload;
  std::unique_ptr<BearSSL::WiFiClientSecure> client(new BearSSL::WiFiClientSecure);
  HTTPClient https;
  int httpsCode;

  if (WiFi.status() != WL_CONNECTED)
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
