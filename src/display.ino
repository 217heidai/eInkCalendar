extern void display_setup(void)
{
  u8g2Fonts.setFontMode(1);          // 使用u8g2透明模式（这是默认设置）
  u8g2Fonts.setFontDirection(0);     // 从左到右（这是默认设置）
  u8g2Fonts.setForegroundColor(COLOR_BLACK);  // 设置前景色
  u8g2Fonts.setBackgroundColor(COLOR_WHITE);  // 设置背景色
  ESP.wdtFeed();//喂狗

  updateMode();//获取屏幕刷新模式

  //测试
  //display_test_number();
  //display_test_weather();
  //display_test_str();
  //display_test_battery();
  //display_test_qrcode();
  //display_test_center();
  //esp_sleep(SLEEP_TIME);
}

// ===== 数据缓存：网络请求整轮只做一次，分页绘制只读缓存 =====
// MAX_DISPLAY_BUFFER_SIZE=15000 会把屏幕分成 2 页，若把网络请求留在
// firstPage()/nextPage() 循环体内，每页都会重新请求 4 个接口。
static Date       g_stDate;
static Weather    g_stWeather;
static Hitokoto   g_stHitokoto;
static CryptoCoin g_stCoin;
static bool       g_hasCoin = false;
static uint8_t    g_battery = 0;

static bool FetchAllData(void)
{
  int i;
  bool ok;

  //日期、天气为必需数据，取不到则整轮不刷新
  ok = false;
  for (i = 0; i < MAX_TRY_COUNT; i++)
  {
    memset(&g_stDate, 0, sizeof(g_stDate));
    if (GetDate(&g_stDate)) { ok = true; break; }
    delay(300); //失败退避：给服务器恢复/TCP清理留时间
  }
  if (!ok) return false;

  ok = false;
  for (i = 0; i < MAX_TRY_COUNT; i++)
  {
    memset(&g_stWeather, 0, sizeof(g_stWeather));
    if (GetWeather(&g_stWeather)) { ok = true; break; }
    delay(300);
  }
  if (!ok) return false;

  //一言取不到用默认句兜底
  ok = false;
  for (i = 0; i < MAX_TRY_COUNT; i++)
  {
    memset(&g_stHitokoto, 0, sizeof(g_stHitokoto));
    if (GetHitokoto(&g_stHitokoto)) { ok = true; break; }
    delay(300);
  }
  if (!ok)
  {
    strcpy(g_stHitokoto.hitokoto, DefaultHitokoto);
    strcpy(g_stHitokoto.from,     DefaultHitokotoFrom);
  }

  //币价取不到不影响其它内容
  g_hasCoin = false;
  for (i = 0; i < MAX_TRY_COUNT; i++)
  {
    memset(&g_stCoin, 0, sizeof(g_stCoin));
    if (GetCoin(&g_stCoin)) { g_hasCoin = true; break; }
    delay(300);
  }

  g_battery = bat_get_percentage();
  return true;
}
// ===== 数据缓存结束 =====

static void GetWeekday(char *pszWeekday, uint8_t nWeekday)
{
  if( nWeekday > 6) return;

  switch(nWeekday)
  {
    case 1:
      sprintf(pszWeekday, "星期一");
      break;
    case 2:
      sprintf(pszWeekday, "星期二");
      break;
    case 3:
      sprintf(pszWeekday, "星期三");
      break;
    case 4:
      sprintf(pszWeekday, "星期四");
      break;
    case 5:
      sprintf(pszWeekday, "星期五");
      break;
    case 6:
      sprintf(pszWeekday, "星期六");
      break;
    default:
      sprintf(pszWeekday, "星期日");
      break;
  }
}

static bool RefreshDate(void)
{
  Date &stDate = g_stDate;   //取数已在 FetchAllData() 完成
  uint16_t dataWidth;
  char disp[100];
  char disp1[100];

  //年月日
  u8g2Fonts.setFont(u8g2_mfxinran_16_number);//设置为大字体
  sprintf(disp, "%d-%02d-%02d", stDate.time.year, stDate.time.month, stDate.time.day);
  drawUTF8Centered(0, FONT_SIZE_CHINESE_LARGE, disp);

  //画线，年进度条
  display.fillRect(0, FONT_SIZE_CHINESE_LARGE + 4, SCREEN_WIDTH - stDate.time.percentage, 2, COLOR_RED);
  display.fillRect(SCREEN_WIDTH - stDate.time.percentage, FONT_SIZE_CHINESE_LARGE + 4, stDate.time.percentage, 2, COLOR_BLACK);

  //节日
  memset(disp, 0, sizeof(disp));
  if(strlen(disp)==0 && strlen(stDate.holiday))
  {
    sprintf(disp, "%s", stDate.holiday);
  }
  memset(disp1, 0, sizeof(disp1));
  if(strlen(disp1)==0 && strlen(stDate.lunar_festival))
  {
    sprintf(disp1, "%s", stDate.lunar_festival);
  }
  if(strlen(disp1)==0 && strlen(stDate.term))
  {
    sprintf(disp1, "%s", stDate.term);
  }
  if (!strlen(disp) && strlen(disp1))
  {
    strcpy(disp, disp1);
  }
  if (strlen(disp) && strcmp(disp, disp1)==0)
  {
    memset(disp1, 0, sizeof(disp1));
  }
  if(strlen(disp) || strlen(disp1))
  {
    SetForegroundColorRED(); //设置为红色
    u8g2Fonts.setFont(u8g2_sarasa_16_gb2312);//设置为大字体

    dataWidth = u8g2Fonts.getUTF8Width(disp);
    drawUTF8Centered((SCREEN_WIDTH - dataWidth)/2, FONT_SIZE_CHINESE_LARGE * 2 + 6, disp);
    if (strlen(disp1))
    {
      dataWidth = u8g2Fonts.getUTF8Width(disp1);
      drawUTF8Centered((SCREEN_WIDTH - dataWidth)/2, FONT_SIZE_CHINESE_LARGE * 3 + 6, disp1);
    }
    
    SetForegroundColorBLACK(); //设置为黑色
  }

  //设置为正常字体
  u8g2Fonts.setFont(u8g2_font_wqy14_t_gb2312);
  //星期
  GetWeekday(disp, stDate.time.week);
  drawUTF8Centered(0, FONT_SIZE_CHINESE_LARGE + 4 + FONT_SIZE_CHINESE_SPACING, disp);
  //干支 生肖
  if(strlen(stDate.ganzhi))
  {
    sprintf(disp, "农历%s%s年", stDate.ganzhi, stDate.shengxiao);
    dataWidth = u8g2Fonts.getUTF8Width(disp);
    drawUTF8Centered(SCREEN_WIDTH - dataWidth, FONT_SIZE_CHINESE_LARGE + 4 + FONT_SIZE_CHINESE_SPACING, disp);
    //农历年月日
    sprintf(disp, "%s", stDate.convert);
    dataWidth = u8g2Fonts.getUTF8Width(disp);
    drawUTF8Centered(SCREEN_WIDTH - dataWidth, FONT_SIZE_CHINESE_LARGE + 4 + FONT_SIZE_CHINESE_SPACING * 2, disp);
  }

  //大数字日期
  if (!stDate.isWorkday) //节假日修改为红色数字
  {
    SetForegroundColorRED(); //设置为红色
  }
  u8g2Fonts.setFont(u8g2_mfxinran_92_number);
  sprintf(disp, "%d", stDate.time.day);
  dataWidth = u8g2Fonts.getUTF8Width(disp);
  drawUTF8Centered((SCREEN_WIDTH - dataWidth)/2, (SCREEN_HEIGTH - FONT_SIZE_NUMBER)/2 + FONT_SIZE_NUMBER, disp);
  if (!stDate.isWorkday) //恢复为黑白色
  {
    SetForegroundColorBLACK(); //设置为黑色
  }

  //sprintf(stDate.yi, "%s", "日值月破 大事不宜");
  //sprintf(stDate.ji, "%s", "日值月破 大事不宜");
  //宜忌
  if (strlen(stDate.yi) && strcmp(stDate.yi, stDate.ji)==0)
  {
    u8g2Fonts.setFont(u8g2_sarasa_16_gb2312);
    SetForegroundColorRED(); //设置为红色
    sprintf(disp, "%s", stDate.yi);
    dataWidth = u8g2Fonts.getUTF8Width(disp);
    drawUTF8Centered(SCREEN_WIDTH / 2 - dataWidth / 2, SCREEN_HEIGTH - ((SCREEN_HEIGTH / 2 - FONT_SIZE_NUMBER / 2 - FONT_SIZE_CHINESE_SPACING * 2)/2 + FONT_SIZE_CHINESE_SPACING * 2) + FONT_SIZE_CHINESE_LARGE/2 - 4, disp);
    SetForegroundColorBLACK(); //设置为黑色
  }
  else
  {
    u8g2Fonts.setFont(u8g2_sarasa_16_gb2312);
    SetForegroundColorRED(); //设置为红色
    sprintf(disp, "%s", "宜");
    dataWidth = u8g2Fonts.getUTF8Width(disp);
    drawUTF8Centered(SCREEN_WIDTH / 4 - dataWidth / 2, SCREEN_HEIGTH - ((SCREEN_HEIGTH / 2 - FONT_SIZE_NUMBER / 2 - FONT_SIZE_CHINESE_SPACING * 2)/2 + FONT_SIZE_CHINESE_SPACING * 2) - 4, disp);
    sprintf(disp, "%s", "忌");
    dataWidth = u8g2Fonts.getUTF8Width(disp);
    drawUTF8Centered(SCREEN_WIDTH - (SCREEN_WIDTH / 4 + dataWidth / 2), SCREEN_HEIGTH - ((SCREEN_HEIGTH / 2 - FONT_SIZE_NUMBER / 2 - FONT_SIZE_CHINESE_SPACING * 2)/2 + FONT_SIZE_CHINESE_SPACING * 2) - 3, disp); //使用-3而不是-4,调整视觉差异
    
    u8g2Fonts.setFont(u8g2_font_wqy14_t_gb2312);
    if(strlen(stDate.yi))
    {
      sprintf(disp, "%s", stDate.yi);
      dataWidth = u8g2Fonts.getUTF8Width(disp);
      drawUTF8Centered(SCREEN_WIDTH / 4 - dataWidth / 2, SCREEN_HEIGTH - ((SCREEN_HEIGTH / 2 - FONT_SIZE_NUMBER / 2 - FONT_SIZE_CHINESE_SPACING * 2)/2 + FONT_SIZE_CHINESE_SPACING * 2) + FONT_SIZE_CHINESE_SPACING, disp);
    }
    if(strlen(stDate.ji))
    {
      sprintf(disp, "%s", stDate.ji);
      dataWidth = u8g2Fonts.getUTF8Width(disp);
      drawUTF8Centered(SCREEN_WIDTH - (SCREEN_WIDTH / 4 + dataWidth / 2), SCREEN_HEIGTH - ((SCREEN_HEIGTH / 2 - FONT_SIZE_NUMBER / 2 - FONT_SIZE_CHINESE_SPACING * 2)/2 + FONT_SIZE_CHINESE_SPACING * 2) + FONT_SIZE_CHINESE_SPACING, disp);
    }
    SetForegroundColorBLACK(); //设置为黑色
  }

  //画线，年进度条
  display.fillRect(0, SCREEN_HEIGTH - FONT_SIZE_CHINESE_SPACING * 2 - 3, stDate.time.percentage, 2, COLOR_BLACK);
  display.fillRect(stDate.time.percentage, SCREEN_HEIGTH - FONT_SIZE_CHINESE_SPACING * 2 - 3, SCREEN_WIDTH - stDate.time.percentage, 2, COLOR_RED);

  return true;
}

static void display_icon_weather(uint16_t x, uint16_t y, char *weatherCode) //天气图标显示
{
  uint16_t code = atoi(weatherCode);

  switch (code)
  {
    case  0: display.drawInvertedBitmap(x, y + 5, Bitmap_qt,     ICON_SIZE_WEATHER, ICON_SIZE_WEATHER, COLOR_RED  ); break;
    case  1: display.drawInvertedBitmap(x, y,     Bitmap_qt_ws,  ICON_SIZE_WEATHER, ICON_SIZE_WEATHER, COLOR_BLACK); break;
    case  2: display.drawInvertedBitmap(x, y + 5, Bitmap_qt,     ICON_SIZE_WEATHER, ICON_SIZE_WEATHER, COLOR_RED  ); break;
    case  3: display.drawInvertedBitmap(x, y,     Bitmap_qt_ws,  ICON_SIZE_WEATHER, ICON_SIZE_WEATHER, COLOR_BLACK); break;
    case  4: display.drawInvertedBitmap(x, y,     Bitmap_dy,     ICON_SIZE_WEATHER, ICON_SIZE_WEATHER, COLOR_BLACK); break;
    case  5: display.drawInvertedBitmap(x, y,     Bitmap_dy,     ICON_SIZE_WEATHER, ICON_SIZE_WEATHER, COLOR_BLACK); break;
    case  6: display.drawInvertedBitmap(x, y,     Bitmap_dy_ws,  ICON_SIZE_WEATHER, ICON_SIZE_WEATHER, COLOR_BLACK); break;
    case  7: display.drawInvertedBitmap(x, y,     Bitmap_dy,     ICON_SIZE_WEATHER, ICON_SIZE_WEATHER, COLOR_BLACK); break;
    case  8: display.drawInvertedBitmap(x, y,     Bitmap_dy_ws,  ICON_SIZE_WEATHER, ICON_SIZE_WEATHER, COLOR_BLACK); break;
    case  9: display.drawInvertedBitmap(x, y + 3, Bitmap_yt,     ICON_SIZE_WEATHER, ICON_SIZE_WEATHER, COLOR_BLACK); break;
    case 10: display.drawInvertedBitmap(x, y,     Bitmap_zheny,  ICON_SIZE_WEATHER, ICON_SIZE_WEATHER, COLOR_BLACK); break;
    case 11: display.drawInvertedBitmap(x, y,     Bitmap_lzy,    ICON_SIZE_WEATHER, ICON_SIZE_WEATHER, COLOR_BLACK); break;
    case 12: display.drawInvertedBitmap(x, y,     Bitmap_lzybbb, ICON_SIZE_WEATHER, ICON_SIZE_WEATHER, COLOR_BLACK); break;
    case 13: display.drawInvertedBitmap(x, y,     Bitmap_xy,     ICON_SIZE_WEATHER, ICON_SIZE_WEATHER, COLOR_BLACK); break;
    case 14: display.drawInvertedBitmap(x, y,     Bitmap_zhongy, ICON_SIZE_WEATHER, ICON_SIZE_WEATHER, COLOR_BLACK); break;
    case 15: display.drawInvertedBitmap(x, y,     Bitmap_dayu,   ICON_SIZE_WEATHER, ICON_SIZE_WEATHER, COLOR_BLACK); break;
    case 16: display.drawInvertedBitmap(x, y,     Bitmap_by,     ICON_SIZE_WEATHER, ICON_SIZE_WEATHER, COLOR_BLACK); break;
    case 17: display.drawInvertedBitmap(x, y,     Bitmap_dby,    ICON_SIZE_WEATHER, ICON_SIZE_WEATHER, COLOR_BLACK); break;
    case 18: display.drawInvertedBitmap(x, y,     Bitmap_tdby,   ICON_SIZE_WEATHER, ICON_SIZE_WEATHER, COLOR_BLACK); break;
    case 19: display.drawInvertedBitmap(x, y,     Bitmap_dongy,  ICON_SIZE_WEATHER, ICON_SIZE_WEATHER, COLOR_BLACK); break;
    case 20: display.drawInvertedBitmap(x, y,     Bitmap_yjx,    ICON_SIZE_WEATHER, ICON_SIZE_WEATHER, COLOR_BLACK); break;
    case 21: display.drawInvertedBitmap(x, y,     Bitmap_zhenx,  ICON_SIZE_WEATHER, ICON_SIZE_WEATHER, COLOR_BLACK); break;
    case 22: display.drawInvertedBitmap(x, y,     Bitmap_xx,     ICON_SIZE_WEATHER, ICON_SIZE_WEATHER, COLOR_BLACK); break;
    case 23: display.drawInvertedBitmap(x, y,     Bitmap_zhongy, ICON_SIZE_WEATHER, ICON_SIZE_WEATHER, COLOR_BLACK); break;
    case 24: display.drawInvertedBitmap(x, y,     Bitmap_dx,     ICON_SIZE_WEATHER, ICON_SIZE_WEATHER, COLOR_BLACK); break;
    case 25: display.drawInvertedBitmap(x, y,     Bitmap_bx,     ICON_SIZE_WEATHER, ICON_SIZE_WEATHER, COLOR_BLACK); break;
    case 26: display.drawInvertedBitmap(x, y,     Bitmap_fc,     ICON_SIZE_WEATHER, ICON_SIZE_WEATHER, COLOR_BLACK); break;
    case 27: display.drawInvertedBitmap(x, y,     Bitmap_ys,     ICON_SIZE_WEATHER, ICON_SIZE_WEATHER, COLOR_BLACK); break;
    case 28: display.drawInvertedBitmap(x, y,     Bitmap_scb,    ICON_SIZE_WEATHER, ICON_SIZE_WEATHER, COLOR_BLACK); break;
    case 29: display.drawInvertedBitmap(x, y,     Bitmap_scb,    ICON_SIZE_WEATHER, ICON_SIZE_WEATHER, COLOR_BLACK); break;
    case 30: display.drawInvertedBitmap(x, y + 5, Bitmap_w,      ICON_SIZE_WEATHER, ICON_SIZE_WEATHER, COLOR_BLACK); break;
    case 31: display.drawInvertedBitmap(x, y,     Bitmap_m,      ICON_SIZE_WEATHER, ICON_SIZE_WEATHER, COLOR_BLACK); break;
    case 32: display.drawInvertedBitmap(x, y,     Bitmap_f,      ICON_SIZE_WEATHER, ICON_SIZE_WEATHER, COLOR_BLACK); break;
    case 33: display.drawInvertedBitmap(x, y,     Bitmap_f,      ICON_SIZE_WEATHER, ICON_SIZE_WEATHER, COLOR_BLACK); break;
    case 34: display.drawInvertedBitmap(x, y,     Bitmap_jf,     ICON_SIZE_WEATHER, ICON_SIZE_WEATHER, COLOR_BLACK); break;
    case 35: display.drawInvertedBitmap(x, y,     Bitmap_rdfb,   ICON_SIZE_WEATHER, ICON_SIZE_WEATHER, COLOR_BLACK); break;
    default: display.drawInvertedBitmap(x, y,     Bitmap_wz,     ICON_SIZE_WEATHER, ICON_SIZE_WEATHER, COLOR_BLACK); break;
  }
}

static bool RefreshWeather(void)
{
  Weather &stWeather = g_stWeather;   //取数已在 FetchAllData() 完成
  uint16_t dataWidth;
  char disp[120];
  uint16_t x = 300; //x起始位置
  uint16_t y = (SCREEN_HEIGTH - FONT_SIZE_NUMBER)/2; //y起始位置
  uint16_t temp_x, temp_y;
  uint16_t circle_x;

  //天气图标
  if(!gisNight)
  {
    display_icon_weather(x, y, stWeather.code_day);
  }
  else
  {
    display_icon_weather(x, y, stWeather.code_night);
  }

  //天气
  u8g2Fonts.setFont(u8g2_font_wqy14_t_gb2312);
  if (strcmp(stWeather.text_day, stWeather.text_night) != 0)
  {
    sprintf(disp, "%s转%s", stWeather.text_day, stWeather.text_night);
  }
  else
  {
    sprintf(disp, "%s", stWeather.text_day);
  }
  dataWidth = u8g2Fonts.getUTF8Width(disp);
  temp_y = y + ICON_SIZE_WEATHER + FONT_SIZE_CHINESE_SPACING;
  drawUTF8Centered(x, temp_y, disp);

  //最低温度
  sprintf(disp, "%s", stWeather.low);
  dataWidth = u8g2Fonts.getUTF8Width(disp);
  temp_y = y + ICON_SIZE_WEATHER + FONT_SIZE_CHINESE_SPACING*2;
  drawUTF8Centered(x, temp_y, disp);
  temp_x = x + dataWidth;
  //画圆圈
  circle_x = temp_x + 3; //计算圈圈的位置
  display.drawCircle(circle_x, temp_y - FONT_SIZE_CHINESE_SPACING + 5, 2, 0);
  //最高温度
  temp_x = temp_x + 1;
  sprintf(disp, " ~ %s", stWeather.high);
  dataWidth = u8g2Fonts.getUTF8Width(disp);
  drawUTF8Centered(temp_x, temp_y, disp);
  temp_x = temp_x + dataWidth;
  //画圆圈
  circle_x = temp_x + 3; //计算圈圈的位置
  display.drawCircle(circle_x, temp_y - FONT_SIZE_CHINESE_SPACING + 5, 2, 0);

  //定位
  temp_y = (SCREEN_HEIGTH - FONT_SIZE_NUMBER)/2 + ICON_SIZE_WEATHER + FONT_SIZE_CHINESE_SPACING * 2; //y起始位置
  display.fillCircle(x + 3, temp_y + 7, 3, 0);
  display.drawFastVLine(x + 3, temp_y + 7 + 3, FONT_SIZE_CHINESE - 6, 0);
  display.drawFastHLine(x + 1, temp_y + FONT_SIZE_CHINESE + 3, 5, 0);
  sprintf(disp, "  %s", stWeather.location);
  drawUTF8Centered(x, temp_y + FONT_SIZE_CHINESE_SPACING + 2, disp);

  return true;
}

static bool RefreshHitokoto(void)
{
  Hitokoto &stHitokoto = g_stHitokoto;   //取数已在 FetchAllData() 完成
  uint16_t dataWidth;
  char disp[120];
  uint16_t dataWidth1;
  char disp1[20] = "一言";
  uint16_t x = 0;
  uint16_t y = 0;

  u8g2Fonts.setFont(u8g2_font_wqy14_t_gb2312);
  //一言
  sprintf(disp, "%s", stHitokoto.hitokoto);
  dataWidth = u8g2Fonts.getUTF8Width(disp);
  drawUTF8Centered((SCREEN_WIDTH - dataWidth)/2, SCREEN_HEIGTH - FONT_SIZE_CHINESE_SPACING - (FONT_SIZE_CHINESE_SPACING - FONT_SIZE_CHINESE), disp);
  //一言出处
  sprintf(disp, "%s", stHitokoto.from);
  dataWidth = u8g2Fonts.getUTF8Width(disp);
  if (dataWidth <= SCREEN_WIDTH/2)
  {
    x = SCREEN_WIDTH - (SCREEN_WIDTH / 4 + dataWidth / 2 );
  }
  else
  {
    x = SCREEN_WIDTH - dataWidth;
  }
  y = SCREEN_HEIGTH - (FONT_SIZE_CHINESE_SPACING - FONT_SIZE_CHINESE);
  dataWidth1 = u8g2Fonts.getUTF8Width(disp1);
  display.drawFastHLine(x - dataWidth1 - 2, y - FONT_SIZE_CHINESE / 2, dataWidth1, 0);
  drawUTF8Centered(x, y, disp);

  return true;
}

static bool RefreshCoin(void)
{
  CryptoCoin &stCryptoCoin = g_stCoin;   //取数已在 FetchAllData() 完成
  uint16_t dataWidth;
  uint16_t start_x = 1;
  uint16_t start_y = (SCREEN_HEIGTH - FONT_SIZE_CHINESE_SPACING * 8)/2; //y起始位置
  char disp[120];

  if (!g_hasCoin) return true; // 加密货币价格获取失败不影响

  u8g2Fonts.setFont(u8g2_font_wqy14_t_gb2312);
  if (strlen(stCryptoCoin.price_btc))
  {
    //btc
    sprintf(disp, "BTC");
    drawUTF8Centered(start_x, start_y + FONT_SIZE_CHINESE_SPACING*1, disp);
    sprintf(disp, "%s", stCryptoCoin.price_btc);
    dataWidth = u8g2Fonts.getUTF8Width(disp);
    drawUTF8Centered(start_x, start_y + FONT_SIZE_CHINESE_SPACING*2, disp);

    //eth
    sprintf(disp, "ETH");
    drawUTF8Centered(start_x, start_y + FONT_SIZE_CHINESE_SPACING*3, disp);
    sprintf(disp, "%s", stCryptoCoin.price_eth);
    drawUTF8Centered(start_x + dataWidth - u8g2Fonts.getUTF8Width(disp), start_y + FONT_SIZE_CHINESE_SPACING*4, disp);

    //BNB
    sprintf(disp, "BNB");
    drawUTF8Centered(start_x, start_y + FONT_SIZE_CHINESE_SPACING*5, disp);
    sprintf(disp, "%s", stCryptoCoin.price_bnb);
    drawUTF8Centered(start_x + dataWidth - u8g2Fonts.getUTF8Width(disp), start_y + FONT_SIZE_CHINESE_SPACING*6, disp);

    //SOL
    sprintf(disp, "SOL");
    drawUTF8Centered(start_x, start_y + FONT_SIZE_CHINESE_SPACING*7, disp);
    sprintf(disp, "%s", stCryptoCoin.price_sol);
    drawUTF8Centered(start_x + dataWidth - u8g2Fonts.getUTF8Width(disp), start_y + FONT_SIZE_CHINESE_SPACING*8, disp);
  }

  return true;
}


static bool RefreshBattery(void)
{
  //电量百分比
  uint8_t percentage = g_battery;   //取数已在 FetchAllData() 完成
  
  //电量图标
  int16_t head_width  = 3;
  int16_t head_height = 6;
  int16_t body_width  = 24;
  int16_t body_height = 14;
  int16_t full_width  = (body_width - 2 - 2) * percentage / 100; //填充宽度
  int16_t full_height = body_height - 2 - 2; //填充高度，留1像素间隙
  int16_t x_start = SCREEN_WIDTH - head_width - body_width;
  int16_t y_start = (FONT_SIZE_CHINESE_LARGE - body_height) / 2 + 2;
  uint16_t color = COLOR_BLACK; //颜色
  if(percentage <= 10) color = COLOR_RED; //低于10%使用红色
  //head
  int16_t x_head = x_start;
  int16_t y_head = y_start + (FONT_SIZE_CHINESE_SPACING - body_height)/2 + (body_height - head_height)/2;
  display.fillRect(x_head, y_head, head_width, head_height, color);
  //body
  int16_t x_body = x_head + head_width;
  int16_t y_body = y_start + (FONT_SIZE_CHINESE_SPACING - body_height)/2;
  display.drawRect(x_body, y_body, body_width, body_height, color);
  //full
  if(percentage >= 5)
  {
    int16_t x_full = x_body + 2 + (body_width - 2 - 2) - full_width;
    int16_t y_full = y_start + (FONT_SIZE_CHINESE_SPACING - full_height)/2;
    display.fillRect(x_full, y_full, full_width, full_height, color);
  }

  return true;
}

extern void display_MainPage(void)
{
  unsigned long ulEpochTime;
  Time stTime;

  if (!FetchAllData()) return;  //网络阶段：整轮只取一次数据，分页循环内不再请求

  display.setPartialWindow(0, 0, SCREEN_WIDTH, SCREEN_HEIGTH); //设置全局刷新
  display.firstPage();
  do
  {
    if(!RefreshDate()) return;//刷新日期
    if(!RefreshWeather()) return;//刷新天气
    if(!RefreshHitokoto()) return;//刷新一言
    if(!RefreshCoin()) return;//刷新加密货币
    if(!RefreshBattery()) return;//刷新电量
  }
  while (display.nextPage());

  //更新刷新时间
  if(GetNTPTime(&ulEpochTime))
  {
    GetTime(ulEpochTime, &stTime);
    Serial.printf("NTP Time: %04d/%02d/%02d %02d:%02d:%02d\n", stTime.year, stTime.month, stTime.day, stTime.hour, stTime.minute, stTime.second);
    ESP.rtcUserMemoryWrite(RTCdz_EpochTime_Now, (uint32_t *)&ulEpochTime, sizeof(ulEpochTime));
    ESP.rtcUserMemoryWrite(RTCdz_EpochTime_LastUpdate, (uint32_t *)&ulEpochTime, sizeof(ulEpochTime));
  }
}
