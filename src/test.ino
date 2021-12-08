extern void display_test_str(void) //中文字体测试
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

extern void display_test_number(void) //大数字测试
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
}