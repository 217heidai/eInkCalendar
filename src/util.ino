//获取屏幕刷新模式
extern void updateMode(void)
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

//显示为红色字体
extern void SetForegroundColorRED(void)
{
  u8g2Fonts.setForegroundColor(COLOR_RED);    // 设置前景色
  u8g2Fonts.setBackgroundColor(COLOR_WHITE);  // 设置背景色
}

//显示为黑色字体
extern void SetForegroundColorBLACK(void)
{
  u8g2Fonts.setForegroundColor(COLOR_BLACK);    // 设置前景色
  u8g2Fonts.setBackgroundColor(COLOR_WHITE);  // 设置背景色
}

//系统休眠
extern void esp_sleep(uint32_t minutes)
{
  Serial.printf("esp deep sleep: %d minutes\n", minutes);
  display.hibernate(); //屏幕进入深度睡眠
  if (minutes > 70) minutes = 70;
  ESP.deepSleep(minutes * 60 * 1000000, WAKE_NO_RFCAL);  //WAKE_RF_DEFAULT  WAKE_RFCAL  WAKE_NO_RFCAL  WAKE_RF_DISABLED
}

//led控制
extern void led_controller(bool controller)
{ 
  if (controller)
  {
    pinMode(LED_BUILTIN, OUTPUT); // Initialize the LED_BUILTIN pin as an output

    digitalWrite(LED_BUILTIN, LOW);   // Turn the LED on (Note that LOW is the voltage level but actually the LED is on; this is because it is active low on the ESP-01)
  }
  else
  {
    digitalWrite(LED_BUILTIN, HIGH);  // Turn the LED off by making the voltage HIGH

    pinMode(LED_BUILTIN, INPUT);
  }
}

//获取电压
static float bat_get_vcc(void) //即时的电压
{
  pinMode(BAT_PIN_SWITCH, OUTPUT);
  digitalWrite(BAT_PIN_SWITCH, 1);
  delay(1);
  float vcc_cache = 0.0;
  for (uint8_t i = 0; i < 20; i++)
  {
    //delay(1);
    vcc_cache += analogRead(BAT_PIN_VCC) * 0.0009765625 * 5.537;
  }
  digitalWrite(BAT_PIN_SWITCH, 0); //关闭电池测量
  pinMode(BAT_PIN_SWITCH, INPUT);  //改为输入状态避免漏电
  return (vcc_cache / 20);
}

//获取电量百分比
extern float bat_get_percentage(void)
{
  float vcc = 0.0;
  float percentage = 0.0;
  
  vcc = bat_get_vcc();
  
  //低电量
  if(vcc > BAT_LOW)
  {
    percentage = ((vcc - BAT_LOW) / (BAT_FULL - BAT_LOW)) * 100 + 0.5; //计算百分比
    if (percentage > 100) percentage = 100.0;
  }

  return percentage;
}