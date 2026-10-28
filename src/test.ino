extern void display_test_str(void) //中文字体测试
{
  int16_t dataWidth;
  String str = "日值月破，大事不宜";
  const char* character = str.c_str(); 

  u8g2Fonts.setFont(u8g2_font_wqy14_t_gb2312a);
  dataWidth = u8g2Fonts.getUTF8Width(character);
  display.setPartialWindow(0, 0, SCREEN_WIDTH, SCREEN_HEIGTH/2); //设置刷新区域
  display.firstPage();
  do
  {
    u8g2Fonts.drawUTF8((SCREEN_WIDTH - dataWidth)/2, FONT_SIZE_CHINESE_SPACING, character);
    u8g2Fonts.drawUTF8((SCREEN_WIDTH - dataWidth)/2, FONT_SIZE_CHINESE_SPACING * 2, character);
    u8g2Fonts.drawUTF8((SCREEN_WIDTH - dataWidth)/2, FONT_SIZE_CHINESE_SPACING * 3, character);
  }
  while (display.nextPage());

  u8g2Fonts.setFont(u8g2_sarasa_16_gb2312);
  dataWidth = u8g2Fonts.getUTF8Width(character);
  display.setPartialWindow(0, SCREEN_HEIGTH/2, SCREEN_WIDTH, SCREEN_HEIGTH); //设置刷新区域
  display.firstPage();
  do
  {
    u8g2Fonts.drawUTF8((SCREEN_WIDTH - dataWidth)/2, SCREEN_HEIGTH/2 + FONT_SIZE_CHINESE_LARGE_SPACING, character);
    u8g2Fonts.drawUTF8((SCREEN_WIDTH - dataWidth)/2, SCREEN_HEIGTH/2 + FONT_SIZE_CHINESE_LARGE_SPACING * 2, character);
    u8g2Fonts.drawUTF8((SCREEN_WIDTH - dataWidth)/2, SCREEN_HEIGTH/2 + FONT_SIZE_CHINESE_LARGE_SPACING * 3, character);
  }
  while (display.nextPage());


  delay(5000);
}

extern void display_test_number(void) //大数字测试
{
  int16_t dataWidth;
  display.firstPage();
  do
  {
    uint8_t size = FONT_SIZE_NUMBER;
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
  delay(5000);
}

extern void display_test_weather() //图标测试
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
  delay(5000);
}

static void test_battery(uint8_t percentage, int16_t x_start)
{
  //电量百分比
  //uint8_t percentage = bat_get_percentage();

  //电量图标
  //int16_t x_start = 3;
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

extern void display_test_battery(void)
{
  display.fillScreen(COLOR_WHITE);  // 填充屏幕
  display.display(1);         // 显示缓冲内容到屏幕，用于全屏缓冲

  test_battery(100,  3);
  test_battery( 90, 33);
  test_battery( 70, 63);
  test_battery( 50, 93);
  test_battery( 30, 123);
  test_battery( 10, 153);
  test_battery(  5, 183);
  test_battery(  3, 213);
  test_battery(  0, 243);

  display.display(1);
}

extern void display_test_center(void) //sarasa 逐字垂直对齐观察页
{
  //区域1 [30,60]；区域2 [74,104]；基线=框顶+18（sarasa 字形高约19）
  const int16_t dy = 0;
  const int16_t box1_y0 = 30, box1_y1 = 60;
  const int16_t box2_y0 = 74, box2_y1 = 104;
  const int16_t baseline1 = box1_y0 + 18;
  const int16_t baseline2 = box2_y0 + 18;

  Serial.printf("逐字对齐验证: 框1[%d,%d]基线=%d 框2[%d,%d]基线=%d dy=%d\n",
                box1_y0, box1_y1, baseline1, box2_y0, box2_y1, baseline2, dy);

  display.firstPage();
  do
  {
    //目标区域框（加大行距，便于观察）
    display.drawRect(0, box1_y0, SCREEN_WIDTH, box1_y1 - box1_y0, COLOR_RED);
    display.drawRect(0, box2_y0, SCREEN_WIDTH, box2_y1 - box2_y0, COLOR_RED);

    //字格中心参考线（黑色两段：=基线-6，每个汉字字形视觉中心应对齐此线）
    display.drawFastHLine(20,  baseline1 - 6, 150, COLOR_BLACK);
    display.drawFastHLine(230, baseline1 - 6, 150, COLOR_BLACK);
    display.drawFastHLine(20,  baseline2 - 6, 150, COLOR_BLACK);
    display.drawFastHLine(230, baseline2 - 6, 150, COLOR_BLACK);

    //逐字上下、左右居中（红色）：与 drawUTF8 同入参，汉字字形在字格内居中
    u8g2Fonts.setFont(u8g2_sarasa_16_gb2312);
    SetForegroundColorRED();
    drawUTF8Centered(0, baseline1 + dy, "日值月破，大事不宜");
    drawUTF8Centered(0, baseline2 + dy, "寒露 国庆节");
    SetForegroundColorBLACK();

    //宜忌区：也按逐字对齐
    display.drawRect(0, SCREEN_HEIGTH - 84, SCREEN_WIDTH / 2, SCREEN_HEIGTH - 62, COLOR_RED);
    display.drawRect(SCREEN_WIDTH / 2, SCREEN_HEIGTH - 84, SCREEN_WIDTH, SCREEN_HEIGTH - 62, COLOR_RED);
    u8g2Fonts.setFont(u8g2_sarasa_16_gb2312);
    SetForegroundColorRED();
    drawUTF8Centered(0, SCREEN_HEIGTH - 84 + 11, "宜");
    drawUTF8Centered(SCREEN_WIDTH / 2, SCREEN_HEIGTH - 84 + 11, "忌");
    SetForegroundColorBLACK();
  }
  while (display.nextPage());

  Serial.println("逐字对齐观察页已显示，5秒后休眠");
  delay(5000);
}

extern void display_test_qrcode(void)
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
    u8g2Fonts.setFont(u8g2_font_wqy14_t_gb2312a);
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