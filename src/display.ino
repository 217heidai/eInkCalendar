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
  //esp_sleep(SLEEP_TIME);
}

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
  Date stDate;
  uint16_t dataWidth;
  char disp[100];
  
  memset(&stDate, 0, sizeof(stDate));
  if (GetDate(&stDate))
  {
    //年月日
    u8g2Fonts.setFont(u8g2_mfyuanhei_16_gb2312);//设置为大字体
    sprintf(disp, "%d-%02d-%02d", stDate.time.year, stDate.time.month, stDate.time.day);
    u8g2Fonts.drawUTF8(0, FONT_SIZE_CHINESE_LARGE, disp);
    //节日
    sprintf(disp, "%s", stDate.holiday);
    if(strlen(disp))
    {
      SetForegroundColorRED(); //设置为红色

      u8g2Fonts.setFont(u8g2_mfyuanhei_16_gb2312);//设置为大字体
      dataWidth = u8g2Fonts.getUTF8Width(disp);
      u8g2Fonts.drawUTF8((SCREEN_WIDTH - dataWidth)/2, FONT_SIZE_CHINESE_LARGE * 2 + 4, disp);

      sprintf(disp, "(%s)", stDate.holidayRemark);
      if(strlen(disp))
      {
        u8g2Fonts.setFont(chinese_city_gb2312);//设置为正常字体
        dataWidth = u8g2Fonts.getUTF8Width(disp);
        u8g2Fonts.drawUTF8((SCREEN_WIDTH - dataWidth)/2, FONT_SIZE_CHINESE_LARGE * 2 + 4 + FONT_SIZE_CHINESE_SPACING, disp); 
      }

      SetForegroundColorBLACK(); //设置为黑色
    }
    else
    {
      u8g2Fonts.setFont(u8g2_mfyuanhei_16_gb2312);//设置为大字体
      sprintf(disp, "%s", stDate.holidayRemark);
      if(strlen(disp))
      {
        SetForegroundColorRED(); //设置为红色

        dataWidth = u8g2Fonts.getUTF8Width(disp);
        u8g2Fonts.drawUTF8((SCREEN_WIDTH - dataWidth)/2, FONT_SIZE_CHINESE_LARGE * 2 + 4, disp);

        SetForegroundColorBLACK(); //设置为黑色
      }
    }

    //设置为正常字体
    u8g2Fonts.setFont(chinese_city_gb2312);
    //星期
    GetWeekday(disp, stDate.time.week);
    u8g2Fonts.drawUTF8(0, FONT_SIZE_CHINESE_LARGE + 4 + FONT_SIZE_CHINESE_SPACING, disp);
    //干支 生肖
    sprintf(disp, "农历%s%s年", stDate.ganzhi, stDate.shengxiao);
    dataWidth = u8g2Fonts.getUTF8Width(disp);
    u8g2Fonts.drawUTF8(SCREEN_WIDTH - dataWidth, FONT_SIZE_CHINESE_LARGE + 4 + FONT_SIZE_CHINESE_SPACING, disp);
    //农历年月日
    sprintf(disp, "%s", stDate.convert);
    dataWidth = u8g2Fonts.getUTF8Width(disp);
    u8g2Fonts.drawUTF8(SCREEN_WIDTH - dataWidth, FONT_SIZE_CHINESE_LARGE + 4 + FONT_SIZE_CHINESE_SPACING * 2, disp);

    //大数字日期
    if (!stDate.isWorkday) //节假日修改为红色数字
    {
      SetForegroundColorRED(); //设置为红色
    }
    u8g2Fonts.setFont(u8g2_mfxinran_92_number);
    sprintf(disp, "%d", stDate.time.day);
    dataWidth = u8g2Fonts.getUTF8Width(disp);
    u8g2Fonts.drawUTF8((SCREEN_WIDTH - dataWidth)/2, (SCREEN_HEIGTH - FONT_SIZE_NUMBER)/2 + FONT_SIZE_NUMBER, disp);
    if (!stDate.isWorkday) //恢复为黑白色
    {
      SetForegroundColorBLACK(); //设置为黑色
    }

    return true;
  }

  return false;
}

static void display_icon_weather(uint16_t x, uint16_t y, char *weatherCode) //天气图标显示
{
  String code = weatherCode;

  if (code == "0")      display.drawInvertedBitmap(x, y + 5, Bitmap_qt, ICON_SIZE_WEATHER, ICON_SIZE_WEATHER, COLOR_RED);
  else if (code == "1") display.drawInvertedBitmap(x, y, Bitmap_qt_ws, ICON_SIZE_WEATHER, ICON_SIZE_WEATHER, COLOR_BLACK);
  else if (code == "2") display.drawInvertedBitmap(x, y + 5, Bitmap_qt, ICON_SIZE_WEATHER, ICON_SIZE_WEATHER, COLOR_RED);
  else if (code == "3") display.drawInvertedBitmap(x, y, Bitmap_qt_ws, ICON_SIZE_WEATHER, ICON_SIZE_WEATHER, COLOR_BLACK);
  else if (code == "4") display.drawInvertedBitmap(x, y, Bitmap_dy, ICON_SIZE_WEATHER, ICON_SIZE_WEATHER, COLOR_BLACK);
  else if (code == "5") display.drawInvertedBitmap(x, y, Bitmap_dy, ICON_SIZE_WEATHER, ICON_SIZE_WEATHER, COLOR_BLACK);
  else if (code == "6") display.drawInvertedBitmap(x, y, Bitmap_dy_ws, ICON_SIZE_WEATHER, ICON_SIZE_WEATHER, COLOR_BLACK);
  else if (code == "7") display.drawInvertedBitmap(x, y, Bitmap_dy, ICON_SIZE_WEATHER, ICON_SIZE_WEATHER, COLOR_BLACK);
  else if (code == "8") display.drawInvertedBitmap(x, y, Bitmap_dy_ws, ICON_SIZE_WEATHER, ICON_SIZE_WEATHER, COLOR_BLACK);
  else if (code == "9") display.drawInvertedBitmap(x, y + 3, Bitmap_yt, ICON_SIZE_WEATHER, ICON_SIZE_WEATHER, COLOR_BLACK);
  else if (code == "10") display.drawInvertedBitmap(x, y, Bitmap_zheny, ICON_SIZE_WEATHER, ICON_SIZE_WEATHER, COLOR_BLACK);
  else if (code == "11") display.drawInvertedBitmap(x, y, Bitmap_lzy, ICON_SIZE_WEATHER, ICON_SIZE_WEATHER, COLOR_BLACK);
  else if (code == "12") display.drawInvertedBitmap(x, y, Bitmap_lzybbb, ICON_SIZE_WEATHER, ICON_SIZE_WEATHER, COLOR_BLACK);
  else if (code == "13") display.drawInvertedBitmap(x, y, Bitmap_xy, ICON_SIZE_WEATHER, ICON_SIZE_WEATHER, COLOR_BLACK);
  else if (code == "14") display.drawInvertedBitmap(x, y, Bitmap_zhongy, ICON_SIZE_WEATHER, ICON_SIZE_WEATHER, COLOR_BLACK);
  else if (code == "15") display.drawInvertedBitmap(x, y, Bitmap_dayu, ICON_SIZE_WEATHER, ICON_SIZE_WEATHER, COLOR_BLACK);
  else if (code == "16") display.drawInvertedBitmap(x, y, Bitmap_by, ICON_SIZE_WEATHER, ICON_SIZE_WEATHER, COLOR_BLACK);
  else if (code == "17") display.drawInvertedBitmap(x, y, Bitmap_dby, ICON_SIZE_WEATHER, ICON_SIZE_WEATHER, COLOR_BLACK);
  else if (code == "18") display.drawInvertedBitmap(x, y, Bitmap_tdby, ICON_SIZE_WEATHER, ICON_SIZE_WEATHER, COLOR_BLACK);
  else if (code == "19") display.drawInvertedBitmap(x, y, Bitmap_dongy, ICON_SIZE_WEATHER, ICON_SIZE_WEATHER, COLOR_BLACK);
  else if (code == "20") display.drawInvertedBitmap(x, y, Bitmap_yjx, ICON_SIZE_WEATHER, ICON_SIZE_WEATHER, COLOR_BLACK);
  else if (code == "21") display.drawInvertedBitmap(x, y, Bitmap_zhenx, ICON_SIZE_WEATHER, ICON_SIZE_WEATHER, COLOR_BLACK);
  else if (code == "22") display.drawInvertedBitmap(x, y, Bitmap_xx, ICON_SIZE_WEATHER, ICON_SIZE_WEATHER, COLOR_BLACK);
  else if (code == "23") display.drawInvertedBitmap(x, y, Bitmap_zhongy, ICON_SIZE_WEATHER, ICON_SIZE_WEATHER, COLOR_BLACK);
  else if (code == "24") display.drawInvertedBitmap(x, y, Bitmap_dx, ICON_SIZE_WEATHER, ICON_SIZE_WEATHER, COLOR_BLACK);
  else if (code == "25") display.drawInvertedBitmap(x, y, Bitmap_bx, ICON_SIZE_WEATHER, ICON_SIZE_WEATHER, COLOR_BLACK);
  else if (code == "26") display.drawInvertedBitmap(x, y, Bitmap_fc, ICON_SIZE_WEATHER, ICON_SIZE_WEATHER, COLOR_BLACK);
  else if (code == "27") display.drawInvertedBitmap(x, y, Bitmap_ys, ICON_SIZE_WEATHER, ICON_SIZE_WEATHER, COLOR_BLACK);
  else if (code == "28") display.drawInvertedBitmap(x, y, Bitmap_scb, ICON_SIZE_WEATHER, ICON_SIZE_WEATHER, COLOR_BLACK);
  else if (code == "29") display.drawInvertedBitmap(x, y, Bitmap_scb, ICON_SIZE_WEATHER, ICON_SIZE_WEATHER, COLOR_BLACK);
  else if (code == "30") display.drawInvertedBitmap(x, y + 5, Bitmap_w, ICON_SIZE_WEATHER, ICON_SIZE_WEATHER, COLOR_BLACK);
  else if (code == "31") display.drawInvertedBitmap(x, y, Bitmap_m, ICON_SIZE_WEATHER, ICON_SIZE_WEATHER, COLOR_BLACK);
  else if (code == "32") display.drawInvertedBitmap(x, y, Bitmap_f, ICON_SIZE_WEATHER, ICON_SIZE_WEATHER, COLOR_BLACK);
  else if (code == "33") display.drawInvertedBitmap(x, y, Bitmap_f, ICON_SIZE_WEATHER, ICON_SIZE_WEATHER, COLOR_BLACK);
  else if (code == "34") display.drawInvertedBitmap(x, y, Bitmap_jf, ICON_SIZE_WEATHER, ICON_SIZE_WEATHER, COLOR_BLACK);
  else if (code == "35") display.drawInvertedBitmap(x, y, Bitmap_rdfb, ICON_SIZE_WEATHER, ICON_SIZE_WEATHER, COLOR_BLACK);
  else display.drawInvertedBitmap(x, y, Bitmap_wz, ICON_SIZE_WEATHER, ICON_SIZE_WEATHER, COLOR_BLACK);
}

static bool RefreshWeather(void)
{
  Weather stWeather;
  uint16_t dataWidth;
  char disp[120];
  uint16_t x = 300; //x起始位置
  uint16_t y = (SCREEN_HEIGTH - FONT_SIZE_NUMBER)/2; //y起始位置
  uint16_t temp_x, temp_y;
  uint16_t circle_x;

  memset(&stWeather, 0, sizeof(stWeather));
  if (GetWeather(&stWeather))
  {
    //设置为大字体
    u8g2Fonts.setFont(u8g2_mfyuanhei_16_gb2312);
    //位置
    sprintf(disp, "%s", stWeather.city);
    dataWidth = u8g2Fonts.getUTF8Width(disp);
    u8g2Fonts.drawUTF8(SCREEN_WIDTH - dataWidth, FONT_SIZE_CHINESE_LARGE, disp);

    //天气图标
    if(!gisNight)
    {
      display_icon_weather(x, y, stWeather.date_code_day);
    }
    else
    {
      display_icon_weather(x, y, stWeather.date_code_night);
    }

    //设置正常字体
    u8g2Fonts.setFont(chinese_city_gb2312);
    //天气
    if(String(stWeather.date_text_day) == String(stWeather.date_text_night))
    {
      sprintf(disp, "%s", stWeather.date_text_day);
    }
    else
    {
      sprintf(disp, "%s转%s", stWeather.date_text_day, stWeather.date_text_night);
    }
    dataWidth = u8g2Fonts.getUTF8Width(disp);
    temp_y = y + ICON_SIZE_WEATHER + FONT_SIZE_CHINESE_SPACING;
    u8g2Fonts.drawUTF8(x, temp_y, disp);

    //最低温度
    sprintf(disp, "%s", stWeather.date_low);
    dataWidth = u8g2Fonts.getUTF8Width(disp);
    temp_y = y + ICON_SIZE_WEATHER + FONT_SIZE_CHINESE_SPACING*2;
    u8g2Fonts.drawUTF8(x, temp_y, disp);
    temp_x = x + dataWidth;
    //画圆圈
    circle_x = temp_x + 3; //计算圈圈的位置
    display.drawCircle(circle_x, temp_y - FONT_SIZE_CHINESE_SPACING + 5, 2, 0);
    //最高温度
    sprintf(disp, " ~ %s", stWeather.date_high);
    dataWidth = u8g2Fonts.getUTF8Width(disp);
    u8g2Fonts.drawUTF8(temp_x, temp_y, disp);
    temp_x = temp_x + dataWidth;
    //画圆圈
    circle_x = temp_x + 3; //计算圈圈的位置
    display.drawCircle(circle_x, temp_y - FONT_SIZE_CHINESE_SPACING + 5, 2, 0);
  
    return true;
  }

  return false;
}

static bool RefreshLifeIndex(void)
{
  LifeIndex stLifeIndex;
  char disp[120];
  uint16_t x = 300; //x起始位置
  uint16_t y = (SCREEN_HEIGTH - FONT_SIZE_NUMBER)/2 + ICON_SIZE_WEATHER + FONT_SIZE_CHINESE_SPACING * 3; //y起始位置

  memset(&stLifeIndex, 0, sizeof(stLifeIndex));
  if (GetLifeIndex(&stLifeIndex))
  {
    //设置正常字体
    u8g2Fonts.setFont(chinese_city_gb2312);

    //刷紫外线指数
    sprintf(disp, "紫外线%s", stLifeIndex.uvi);
    u8g2Fonts.drawUTF8(x, y, disp);

    return true;
  }

  return false;
}

static bool RefreshHitokoto(void)
{
  Hitokoto stHitokoto;
  uint16_t dataWidth;
  char disp[120];

  memset(&stHitokoto, 0, sizeof(stHitokoto));
  if (GetHitokoto(&stHitokoto))
  {
    u8g2Fonts.setFont(chinese_city_gb2312);
    //一言
    sprintf(disp, "%s", stHitokoto.hitokoto);
    dataWidth = u8g2Fonts.getUTF8Width(disp);
    u8g2Fonts.drawUTF8((SCREEN_WIDTH - dataWidth)/2, SCREEN_HEIGTH - FONT_SIZE_CHINESE_SPACING - (FONT_SIZE_CHINESE_SPACING - FONT_SIZE_CHINESE), disp);
    //一言出处
    sprintf(disp, "%s", stHitokoto.from);
    if(strlen(disp))
    {
      dataWidth = u8g2Fonts.getUTF8Width(disp);
      u8g2Fonts.drawUTF8(SCREEN_WIDTH - dataWidth - FONT_SIZE_CHINESE * 2, SCREEN_HEIGTH - (FONT_SIZE_CHINESE_SPACING - FONT_SIZE_CHINESE), disp);
    }
  }
  else
  {
    u8g2Fonts.setFont(chinese_city_gb2312);
    dataWidth = u8g2Fonts.getUTF8Width(DefaultHitokoto);
    u8g2Fonts.drawUTF8((SCREEN_WIDTH - dataWidth)/2, SCREEN_HEIGTH - FONT_SIZE_CHINESE_SPACING - (FONT_SIZE_CHINESE_SPACING - FONT_SIZE_CHINESE), DefaultHitokoto);

    dataWidth = u8g2Fonts.getUTF8Width(DefaultHitokotoFrom);
    u8g2Fonts.drawUTF8(SCREEN_WIDTH - dataWidth - FONT_SIZE_CHINESE * 2, SCREEN_HEIGTH - (FONT_SIZE_CHINESE_SPACING - FONT_SIZE_CHINESE), DefaultHitokotoFrom);
  }

  return true;
}

static void RefreshBattery(void)
{
  //电量百分比
  uint8_t percentage = bat_get_percentage();
  
  //电量图标
  int16_t x_start = 3;
  int16_t y_start = SCREEN_HEIGTH - FONT_SIZE_CHINESE_SPACING;
  int16_t head_width  = 3;
  int16_t head_height = 6;
  int16_t body_width  = 24;
  int16_t body_height = 14;
  int16_t full_width  = (body_width - 2 - 2) * percentage / 100; //填充宽度
  int16_t full_height = body_height - 2 - 2; //填充高度，留1像素间隙
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
}

extern void display_MainPage(void)
{
  display.setPartialWindow(0, 0, SCREEN_WIDTH, SCREEN_HEIGTH); //设置全局刷新
  display.firstPage();
  do
  {
    RefreshDate();//刷新日期
    RefreshWeather();//刷新天气
    RefreshLifeIndex();//刷新天气指数
    RefreshHitokoto();//刷新一言
    RefreshBattery();//刷新电量
  }
  while (display.nextPage());
}
