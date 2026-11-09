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
    drawUTF8Centered((SCREEN_WIDTH - dataWidth) / 2, (SCREEN_HEIGTH - 200) / 2 - 2, disp);

    // 操作提示
    u8g2Fonts.setFont(u8g2_font_wqy14_t_gb2312);
    sprintf(disp, "确保手机连接到2.4GHz WiFi");
    Serial.println(disp);
    dataWidth = u8g2Fonts.getUTF8Width(disp);
    drawUTF8Centered((SCREEN_WIDTH - dataWidth) / 2, SCREEN_HEIGTH -(SCREEN_HEIGTH - 200) / 2 + FONT_SIZE_CHINESE_SPACING, disp);
    sprintf(disp, "请在3分钟内完成网络配置");
    Serial.println(disp);
    dataWidth = u8g2Fonts.getUTF8Width(disp); // 与第一行对齐
    drawUTF8Centered((SCREEN_WIDTH - dataWidth) / 2, SCREEN_HEIGTH -(SCREEN_HEIGTH - 200) / 2 + FONT_SIZE_CHINESE_SPACING*2, disp);
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
    drawUTF8Centered((SCREEN_WIDTH - dataWidth) / 2, SCREEN_HEIGTH / 2, disp);

    // 操作提示
    u8g2Fonts.setFont(u8g2_font_wqy14_t_gb2312);
    sprintf(disp, "确保手机连接到2.4GHz WiFi");
    dataWidth = u8g2Fonts.getUTF8Width(disp);
    drawUTF8Centered((SCREEN_WIDTH - dataWidth) / 2, SCREEN_HEIGTH / 2 + FONT_SIZE_CHINESE_SPACING, disp);
    sprintf(disp, "请在3分钟内完成网络配置");
    dataWidth = u8g2Fonts.getUTF8Width(disp); // 与第一行对齐
    drawUTF8Centered((SCREEN_WIDTH - dataWidth) / 2, SCREEN_HEIGTH / 2 + FONT_SIZE_CHINESE_SPACING*2, disp);
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
    display_smartConfig(); // 显示配网二维码
    Serial.println("SmartConfig已启动，等待配网数据...");
    Serial.println("配网超时时间: 3分钟");

    led_controller(true); // 打开LED
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
        eeprom_write(EEPROM_ADDR_SSID, (uint8_t *)pssid, len);
        len = strlen((const char*)ppasswd);
        eeprom_write(EEPROM_ADDR_PASSWD, (uint8_t *)ppasswd, len);
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

    led_controller(false); // 关闭LED
  }

  if (isSuccess)
  {
    Serial.println("\n配网成功！");
    Serial.printf("正在连接WiFi: %s\n", ssid);
    Serial.print("连接中");
  }
  else
  {
    display_smartConfig_fail(); // 显示配网失败
    Serial.println("SmartConfig配置失败");
  }

  return isSuccess;
}

extern bool connectToWifi(void)
{
  uint8_t len_ssid = 0;
  uint8_t len_passwd = 0;
  uint8_t ssid[255 + 1];
  uint8_t passwd[255 + 1];
  uint8_t i = 0;
  bool isSuccess = true; //可能不需要配网，所以默认成功

RETRY_CONNECT:
  // 读取EEPROM中的SSID和密码
  memset(ssid, 0, sizeof(ssid));
  memset(passwd, 0, sizeof(passwd));
  len_ssid = eeprom_read(EEPROM_ADDR_SSID, ssid);
  len_passwd = eeprom_read(EEPROM_ADDR_PASSWD, passwd);
  //len_ssid = 0; // for test
  if ((len_ssid < 1) || (len_ssid > WIFI_CONFIG_MAX_LEN) || (len_passwd < 1) || (len_passwd > WIFI_CONFIG_MAX_LEN)) //配网
  {
    // 如重新配网，则清空location，重新进行IP定位
    eeprom_clear(EEPROM_ADDR_LOCATION);

    if (!smartConfig(ssid, passwd))
    {
      return false;
    }
  }
  else
  {
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
      return true;
    }
  }

  ESP.wdtFeed();//喂狗
  goto RETRY_CONNECT;
}

// ===== 第二步 A：同一 host 复用一条 TLS 连接 =====
// 四个接口都在 api.heidai.space，用 keep-alive 把 4 次握手降为 1 次。
// 惰性创建：避免在 setup 之前就常驻占用约 6KB 堆；深睡重启后自动回收。
static BearSSL::WiFiClientSecure *g_tlsClient = nullptr;
static HTTPClient *g_https = nullptr;
static char g_sessionHost[64] = {0};
static bool g_sessionReady = false;            //上次请求成功且连接可继续使用

// URL 常量在 PROGMEM(flash)，标准 strstr/strlen 会做非对齐 32 位读触发 Exception(3)。
// 逐字节复制到 RAM 后再处理；pgm_read_byte 对 RAM 与 flash 指针都安全。
static void https_copy_url(const char *url, char *buf, size_t cap)
{
  size_t i = 0;
  while (i + 1 < cap)
  {
    char c = (char)pgm_read_byte(url + i);
    if (!c) break;
    buf[i++] = c;
  }
  buf[i] = 0;
}

static void https_extract_host(const char *ramUrl, char *host, size_t cap)
{
  host[0] = 0;
  const char *p = strstr(ramUrl, "://");
  if (!p) return;
  p += 3;
  const char *q = strchr(p, '/');
  size_t n = q ? (size_t)(q - p) : strlen(p);
  if (n >= cap) n = cap - 1;
  memcpy(host, p, n);
  host[n] = 0;
}
// ===== 连接复用结束 =====

extern String callHttps(const char *url)
{
  String payload;
  payload.reserve(512); //预分配，避免多次小块 realloc（大响应会按需扩展一次）
  int httpsCode;
  char host[64] = {0};             //本次请求的 host
  char ramUrl[256] = {0};          //PROGMEM URL 的 RAM 副本
  bool needReset = true;           //默认关闭会话；仅成功时保持连接

  https_copy_url(url, ramUrl, sizeof(ramUrl));
  https_extract_host(ramUrl, host, sizeof(host));

  if (WiFi.status() != WL_CONNECTED)
  {
    payload = F("{\"status_code\":\"WIFI连接断开\"}");  //将错误值转换成json格式
    goto END;
  }

  //惰性创建会话对象：首次请求时分配，之后一直复用
  if (g_tlsClient == nullptr || g_https == nullptr)
  {
    g_tlsClient = new BearSSL::WiFiClientSecure();
    g_https = new HTTPClient();
    if (g_tlsClient == nullptr || g_https == nullptr)
    {
      payload = F("{\"status_code\":\"内存不足\"}");
      return payload;
    }
  }

  //首次、换 host、或上次失败：关闭旧会话并重新配置一次
  if (!g_sessionReady || strcmp(host, g_sessionHost) != 0)
  {
    g_https->end();
    g_tlsClient->stop();
    g_tlsClient->setInsecure(); //不检验证书
    g_tlsClient->setBufferSizes(1024, 512); //缓存大小
    g_tlsClient->setSSLVersion(BR_TLS12, BR_TLS12);
    g_https->setReuse(true);   //关键：keep-alive，同 host 复用一条 TLS 连接
    g_https->setTimeout(20000); //超时时间20s
    strcpy(g_sessionHost, host);
    g_sessionReady = false;
  }

  Serial.printf_P(PSTR("Before: free_heap %d, max_alloc_heap %d, heap_fragmentation  %d\n"), ESP.getFreeHeap(), ESP.getMaxFreeBlockSize(), ESP.getHeapFragmentation());
  Serial.printf("[HTTPS] begin... url: %s\n", ramUrl);
  if (g_https->begin(*g_tlsClient, String(ramUrl)))
  {
    httpsCode = g_https->GET();
    if (httpsCode > 0)  //判断有无返回值
    {
      Serial.printf("[HTTPS] GET... code: %d\n", httpsCode);
      if (httpsCode == HTTP_CODE_OK) //判断请求是正确
      {
        payload = g_https->getString();
        needReset = false;           //保持连接给下一个接口
        goto END;
      }
      else
      {
        char ebuf[48];
        snprintf(ebuf, sizeof(ebuf), "{\"status_code\":\"请求错误:%d\"}", httpsCode);
        payload = ebuf;
        needReset = false;           //仅是 HTTP 层错误，连接仍可复用
        goto END;
      }
    }
    else
    {
      char ebuf[64];
      snprintf(ebuf, sizeof(ebuf), "{\"status_code\":\"%s\"}", g_https->errorToString(httpsCode).c_str());
      payload = ebuf;
      goto END;
    }
  }
  else
  {
    payload = F("{\"status_code\":\"无法连接服务器\"}");  //将错误值转换成json格式
    goto END;
  }

END:
  if (g_https != nullptr) g_https->end();   //keep-alive 时不会关闭底层连接
  if (needReset)       //失败：彻底关闭，下次重试重新握手
  {
    if (g_tlsClient != nullptr) g_tlsClient->stop();
    g_sessionReady = false;
  }
  else
  {
    g_sessionReady = true;
  }
  Serial.println(payload);
  return payload;
}
