//系统休眠
void esp_sleep(uint32_t minutes)
{
  Serial.printf("esp deep sleep: %d minutes\n", minutes);
  display.hibernate(); //屏幕进入深度睡眠
  if (minutes > 70) minutes = 70;
  //else if (get_bat_vcc_nwe() < BatLow) minutes = 0;
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