static void updateMode(void)
{
  const char* updatemode;
  if (display.epd2.hasFastPartialUpdate)
  {
    updatemode = "快速局部刷新";
  }
  else if (display.epd2.hasPartialUpdate)
  {
    updatemode = "缓慢局部刷新";
  }
  else
  {
    updatemode = "不支持局部刷新";
  }
  Serial.printf("\n屏幕刷新模式: %s\n", updatemode);
}

extern void display_setup(void)
{
  u8g2Fonts.setFontMode(1);          // 使用u8g2透明模式（这是默认设置）
  u8g2Fonts.setFontDirection(0);     // 从左到右（这是默认设置）
  u8g2Fonts.setForegroundColor(COLOR_BLACK);  // 设置前景色
  u8g2Fonts.setBackgroundColor(COLOR_WHITE);  // 设置背景色
  ESP.wdtFeed();//喂狗

  updateMode();//获取屏幕刷新模式

  //display_Bitmap_Setup(); //开机壁纸

  //测试
  //display_test_number();
  //display_test_weather();
  //display_test_str();
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

static void SetForegroundColorRED(void)
{
  u8g2Fonts.setForegroundColor(COLOR_RED);    // 设置前景色
  u8g2Fonts.setBackgroundColor(COLOR_WHITE);  // 设置背景色
}

static void SetForegroundColorBLACK(void)
{
  u8g2Fonts.setForegroundColor(COLOR_BLACK);    // 设置前景色
  u8g2Fonts.setBackgroundColor(COLOR_WHITE);  // 设置背景色
}

static void RefreshDateTime(void)
{
  DateTime stDateTime;
  uint16_t dataWidth;
  char disp[100];
  
  memset(&stDateTime, 0, sizeof(stDateTime));
  if (GetDatetime(&stDateTime))
  {
    //年月日
    u8g2Fonts.setFont(u8g2_mfyuanhei_16_gb2312);//设置为大字体
    sprintf(disp, "%d-%02d-%02d", stDateTime.year, stDateTime.month, stDateTime.day);
    u8g2Fonts.drawUTF8(0, FONT_SIZE_CHINESE_LARGE, disp);
    //节日
    sprintf(disp, "%s", stDateTime.holiday);
    if(strlen(disp))
    {
      SetForegroundColorRED(); //设置为红色

      u8g2Fonts.setFont(u8g2_mfyuanhei_16_gb2312);//设置为大字体
      dataWidth = u8g2Fonts.getUTF8Width(disp);
      u8g2Fonts.drawUTF8((SCREEN_WIDTH - dataWidth)/2, FONT_SIZE_CHINESE_LARGE * 2 + 4, disp);

      sprintf(disp, "(%s)", stDateTime.holidayRemark);
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
      sprintf(disp, "%s", stDateTime.holidayRemark);
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
    GetWeekday(disp, stDateTime.week);
    u8g2Fonts.drawUTF8(0, FONT_SIZE_CHINESE_LARGE + 4 + FONT_SIZE_CHINESE_SPACING, disp);
    //干支 生肖
    sprintf(disp, "农历%s%s年", stDateTime.ganzhi, stDateTime.shengxiao);
    dataWidth = u8g2Fonts.getUTF8Width(disp);
    u8g2Fonts.drawUTF8(SCREEN_WIDTH - dataWidth, FONT_SIZE_CHINESE_LARGE + 4 + FONT_SIZE_CHINESE_SPACING, disp);
    //农历年月日
    sprintf(disp, "%s", stDateTime.convert);
    dataWidth = u8g2Fonts.getUTF8Width(disp);
    u8g2Fonts.drawUTF8(SCREEN_WIDTH - dataWidth, FONT_SIZE_CHINESE_LARGE + 4 + FONT_SIZE_CHINESE_SPACING * 2, disp);

    //大数字日期
    if (!stDateTime.isWorkday) //节假日修改为红色数字
    {
      SetForegroundColorRED(); //设置为红色
    }
    u8g2Fonts.setFont(u8g2_mfxinran_92_number);
    sprintf(disp, "%d", stDateTime.day);
    dataWidth = u8g2Fonts.getUTF8Width(disp);
    u8g2Fonts.drawUTF8((SCREEN_WIDTH - dataWidth)/2, (SCREEN_HEIGTH - FONT_SIZE_NUMBER)/2 + FONT_SIZE_NUMBER, disp);
    if (!stDateTime.isWorkday) //恢复为黑白色
    {
      SetForegroundColorBLACK(); //设置为黑色
    }
  }
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

static void RefreshWeather(void)
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
    if(!isNight)
    {
      display_icon_weather(x, y, stWeather.date0_code_day);
    }
    else
    {
      display_icon_weather(x, y, stWeather.date0_code_night);
    }

    //设置正常字体
    u8g2Fonts.setFont(chinese_city_gb2312);
    //天气
    if(String(stWeather.date0_text_day) == String(stWeather.date0_text_night))
    {
      sprintf(disp, "%s", stWeather.date0_text_day);
    }
    else
    {
      sprintf(disp, "%s转%s", stWeather.date0_text_day, stWeather.date0_text_night);
    }
    dataWidth = u8g2Fonts.getUTF8Width(disp);
    temp_y = y + ICON_SIZE_WEATHER + FONT_SIZE_CHINESE_SPACING;
    u8g2Fonts.drawUTF8(x, temp_y, disp);

    //最低温度
    sprintf(disp, "%s", stWeather.date0_low);
    dataWidth = u8g2Fonts.getUTF8Width(disp);
    temp_y = y + ICON_SIZE_WEATHER + FONT_SIZE_CHINESE_SPACING*2;
    u8g2Fonts.drawUTF8(x, temp_y, disp);
    temp_x = x + dataWidth;
    //画圆圈
    circle_x = temp_x + 3; //计算圈圈的位置
    display.drawCircle(circle_x, temp_y - FONT_SIZE_CHINESE_SPACING + 5, 2, 0);
    //最高温度
    sprintf(disp, " ~ %s", stWeather.date0_high);
    dataWidth = u8g2Fonts.getUTF8Width(disp);
    u8g2Fonts.drawUTF8(temp_x, temp_y, disp);
    temp_x = temp_x + dataWidth;
    //画圆圈
    circle_x = temp_x + 3; //计算圈圈的位置
    display.drawCircle(circle_x, temp_y - FONT_SIZE_CHINESE_SPACING + 5, 2, 0);
  }
}

static void RefreshLifeIndex(void)
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
  }
}

static void RefreshHitokoto(void)
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
    dataWidth = u8g2Fonts.getUTF8Width(DefaultHitokoto.c_str());
    u8g2Fonts.drawUTF8((SCREEN_WIDTH - dataWidth)/2, SCREEN_HEIGTH - FONT_SIZE_CHINESE_SPACING - (FONT_SIZE_CHINESE_SPACING - FONT_SIZE_CHINESE), DefaultHitokoto.c_str());
  }
}

extern void display_MainPage(void)
{
  display.setPartialWindow(0, 0, SCREEN_WIDTH, SCREEN_HEIGTH); //设置全局刷新
  display.firstPage();
  do
  {
    RefreshDateTime();//刷新日期时间
    RefreshWeather();//刷新天气
    RefreshLifeIndex();//刷新天气指数
    RefreshHitokoto();//刷新一言
  }
  while (display.nextPage());
}

static void display_test_str(void)
{
  int16_t dataWidth;
  String str = "2021年08月06日 星期五";
  const char* character = str.c_str(); 
  uint8_t size = 24;

  display.firstPage();
  do
  {
    u8g2Fonts.setFont(u8g2_mfyuanhei_16_gb2312);

    dataWidth = u8g2Fonts.getUTF8Width(character);
    u8g2Fonts.drawUTF8((SCREEN_WIDTH - dataWidth)/2, size, character);
    Serial.printf("\ndataWidth: %d\n", dataWidth/8);

    dataWidth = u8g2Fonts.getUTF8Width(character);
    u8g2Fonts.drawUTF8((SCREEN_WIDTH - dataWidth)/2, size * 2, character);
    Serial.printf("\ndataWidth: %d\n", dataWidth/8);

    dataWidth = u8g2Fonts.getUTF8Width(character);
    u8g2Fonts.drawUTF8((SCREEN_WIDTH - dataWidth)/2, size * 3, character);
    Serial.printf("\ndataWidth: %d\n", dataWidth/8);
  }
  while (display.nextPage());
}

static void display_test_number(void)
{
  int16_t dataWidth;
  display.firstPage();
  do
  {
    uint8_t size = 101;
    u8g2Fonts.setFont(u8g2_mfxinran_92_number);
    dataWidth = u8g2Fonts.getUTF8Width("123");
    u8g2Fonts.drawUTF8((SCREEN_WIDTH - dataWidth)/2, size, "123");
    Serial.printf("\ndataWidth: %d\n", dataWidth/3);

    dataWidth = u8g2Fonts.getUTF8Width("456");
    u8g2Fonts.drawUTF8((SCREEN_WIDTH - dataWidth)/2, size*2, "456");
    Serial.printf("\ndataWidth: %d\n", dataWidth/3);

    dataWidth = u8g2Fonts.getUTF8Width("789");
    u8g2Fonts.drawUTF8((SCREEN_WIDTH - dataWidth)/2, size*3, "789");
    Serial.printf("\ndataWidth: %d\n", dataWidth/3);
  }
  while (display.nextPage());
}

static void display_test_weather() //图标测试
{
  display.fillScreen(COLOR_WHITE);  // 填充屏幕
  display.display(1);         // 显示缓冲内容到屏幕，用于全屏缓冲

  //网格
  display.drawLine(0, 1, 400, 1, COLOR_RED);
  display.drawLine(0, 45, 400, 45, COLOR_RED);
  display.drawLine(1, 0, 1, 300, COLOR_RED);
  display.drawLine(45, 0, 45, 300, COLOR_RED);

  display.drawInvertedBitmap(0, 0, Bitmap_qt, 45, 45, COLOR_BLACK);
  display.drawInvertedBitmap(50, 0, Bitmap_dy, 45, 45, COLOR_BLACK);
  display.drawInvertedBitmap(100, 0, Bitmap_yt, 45, 45, COLOR_BLACK);
  display.drawInvertedBitmap(150, 0, Bitmap_zheny, 45, 45, COLOR_BLACK);
  display.drawInvertedBitmap(200, 0, Bitmap_lzybbb, 45, 45, COLOR_BLACK);
  display.drawInvertedBitmap(250, 0, Bitmap_xy, 45, 45, COLOR_BLACK);
  display.drawInvertedBitmap(300, 0, Bitmap_zhongy, 45, 45, COLOR_BLACK);
  display.drawInvertedBitmap(350, 0, Bitmap_dayu, 45, 45, COLOR_BLACK);

  display.drawInvertedBitmap(0, 50, Bitmap_by, 45, 45, COLOR_BLACK);
  display.drawInvertedBitmap(50, 50, Bitmap_dby, 45, 45, COLOR_BLACK);
  display.drawInvertedBitmap(100, 50, Bitmap_tdby, 45, 45, COLOR_BLACK);
  display.drawInvertedBitmap(150, 50, Bitmap_dongy, 45, 45, COLOR_BLACK);
  display.drawInvertedBitmap(200, 50, Bitmap_yjx, 45, 45, COLOR_BLACK);
  display.drawInvertedBitmap(250, 50, Bitmap_zhenx, 45, 45, COLOR_BLACK);
  display.drawInvertedBitmap(300, 50, Bitmap_xx, 45, 45, COLOR_BLACK);
  display.drawInvertedBitmap(350, 50, Bitmap_zhongx, 45, 45, COLOR_BLACK);

  display.drawInvertedBitmap(0, 100, Bitmap_dx, 45, 45, COLOR_BLACK);
  display.drawInvertedBitmap(50, 100, Bitmap_bx, 45, 45, COLOR_BLACK);
  display.drawInvertedBitmap(100, 100, Bitmap_fc, 45, 45, COLOR_BLACK);
  display.drawInvertedBitmap(150, 100, Bitmap_ys, 45, 45, COLOR_BLACK);
  display.drawInvertedBitmap(200, 100, Bitmap_scb, 45, 45, COLOR_BLACK);
  display.drawInvertedBitmap(250, 100, Bitmap_w, 45, 45, COLOR_BLACK);
  display.drawInvertedBitmap(300, 100, Bitmap_m, 45, 45, COLOR_BLACK);
  display.drawInvertedBitmap(350, 100, Bitmap_f, 45, 45, COLOR_BLACK);

  display.drawInvertedBitmap(0, 150, Bitmap_jf, 45, 45, COLOR_BLACK);
  display.drawInvertedBitmap(50, 150, Bitmap_rdfb, 45, 45, COLOR_BLACK);
  display.drawInvertedBitmap(100, 150, Bitmap_ljf, 45, 45, COLOR_BLACK);
  display.drawInvertedBitmap(150, 150, Bitmap_wz, 45, 45, COLOR_BLACK);
  display.drawInvertedBitmap(200, 150, Bitmap_qt_ws, 45, 45, COLOR_BLACK);
  display.drawInvertedBitmap(250, 150, Bitmap_yt_ws, 45, 45, COLOR_BLACK);
  display.drawInvertedBitmap(300, 150, Bitmap_dy_ws, 45, 45, COLOR_BLACK);
  display.drawInvertedBitmap(350, 150, Bitmap_zy_ws, 45, 45, COLOR_BLACK);

  display.drawInvertedBitmap(0, 200, Bitmap_zx_ws, 45, 45, COLOR_BLACK);
  display.drawInvertedBitmap(50, 200, Bitmap_weizhi, 13, 13, COLOR_BLACK);

  display.display(1);
}