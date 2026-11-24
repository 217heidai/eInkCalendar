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

//在矩形区域内让字符串上下、左右居中（纯中文行；使用前需已 setFont）
//汉字视觉几何为实测值：视觉顶到基线距离 topToBaseline、视觉高度 charH
//sarasa_16：18/20；wqy14：12/13；dy 为垂直微调：偏上填正数，偏下填负数
extern void drawUTF8CenteredRect(int16_t x0, int16_t y0, int16_t x1, int16_t y1,
                                 const char *str, int16_t dy, int16_t topToBaseline, int16_t charH)
{
  //左右居中
  int16_t strW = u8g2Fonts.getUTF8Width(str);
  int16_t x = x0 + (x1 - x0 - strW) / 2;
  if (x < x0) x = x0;

  //上下居中：drawUTF8 的 y 是基线，汉字视觉顶在基线上方 topToBaseline 处
  //基线 = 区域顶 + (区域高 - 字高)/2 + 顶距基线 + 微调
  int16_t y = y0 + (y1 - y0 - charH) / 2 + topToBaseline + dy;
  if (y < y0 + topToBaseline) y = y0 + topToBaseline;

  u8g2Fonts.drawUTF8(x, y, str);
}

//===== 逐字垂直居中：每个汉字按自身视觉盒中心对齐到区域中线 =====
//sarasa 是比例字体，各汉字 bbx 高度/偏移不同（实测：顶距基线 16~20px），
//整行统一偏移无法对齐，需运行时解码每个字形。参考线应画"中心线"而非基线。
extern const uint8_t *u8g2_font_get_glyph_data(u8g2_font_t *u8g2, uint16_t encoding);

static uint8_t utf8_dec_unsigned(u8g2_font_decode_t *f, uint8_t cnt)
{
  uint8_t val, bit_pos = f->decode_bit_pos;
  uint8_t bit_pos_plus_cnt = bit_pos + cnt;
  val = u8x8_pgm_read(f->decode_ptr);
  val >>= bit_pos;
  if (bit_pos_plus_cnt >= 8)
  {
    uint8_t s = 8 - bit_pos;
    f->decode_ptr++;
    val |= u8x8_pgm_read(f->decode_ptr) << s;
    bit_pos_plus_cnt -= 8;
  }
  val &= (1U << cnt) - 1;
  f->decode_bit_pos = bit_pos_plus_cnt;
  return val;
}

static int8_t utf8_dec_signed(u8g2_font_decode_t *f, uint8_t cnt)
{
  int8_t v = (int8_t)utf8_dec_unsigned(f, cnt);
  v -= (1 << (cnt - 1));
  return v;
}

//与 drawUTF8 完全同入参同语义：x=字符串起点（左边缘）、y=基线
//区别仅在汉字(U+4E00~U+9FFF)：字形在自身字格内上下、左右居中
//  上下：字形视觉中心对齐字格中心（sarasa 字格高=ascent+descent=20，中心=基线上方6px）
//  左右：字形水平中心对齐字格(advance)中心
//非汉字/缺字形：与 drawUTF8 完全一致，无任何偏移
//与 drawUTF8 完全同入参同语义：x=字符串起点、y=基线；仅汉字(U+4E00~U+9FFF)逐字居中，非汉字零偏移
//步进口径：汉字=u8g2_DrawGlyph 返回值(advance)；非汉字=u8g2_GetGlyphWidth(advance)
//两者都与库 drawUTF8 内部 drawGlyph 的步进完全一致（不能用 getUTF8Width：它有 issue#16 尾字修正，
//会把单字符宽度换成"真实像素宽+x_offset"，导致字母/数字步进偏小、字符贴在一起）
//4 字节字符（如 emoji）：字库无此字形，只消费不绘制、不步进
extern void drawUTF8Centered(int16_t x, int16_t y, const char *str)
{
  const char *p = str;
  while (*p)
  {
    uint32_t enc;
    int len;
    //多字节分支都校验后续字节是 0x80~0xBF：截断的 UTF-8 不会读出字符串末尾
    if ((*p & 0x80) == 0)          { enc = *p; len = 1; }
    else if (((*p & 0xE0) == 0xC0) && ((p[1] & 0xC0) == 0x80))  { enc = ((*p & 0x1F) << 6) | (p[1] & 0x3F); len = 2; }
    else if (((*p & 0xF0) == 0xE0) && ((p[1] & 0xC0) == 0x80) && ((p[2] & 0xC0) == 0x80))  { enc = ((*p & 0x0F) << 12) | ((p[1] & 0x3F) << 6) | (p[2] & 0x3F); len = 3; }
    else if (((*p & 0xF8) == 0xF0) && ((p[1] & 0xC0) == 0x80) && ((p[2] & 0xC0) == 0x80) && ((p[3] & 0xC0) == 0x80))  { enc = ((*p & 0x07) << 18) | ((p[1] & 0x3F) << 12) | ((p[2] & 0x3F) << 6) | (p[3] & 0x3F); len = 4; }
    else { p++; continue; } //无效字节/截断的 UTF-8：跳过，保证不越界、不死循环

    if (enc >= 0x4E00 && enc <= 0x9FFF)  //仅汉字：字形在字格内上下、左右居中
    {
      const uint8_t *gd = u8g2_font_get_glyph_data(&u8g2Fonts.u8g2, (uint16_t)enc);
      if (gd)
      {
        u8g2_font_decode_t dec;
        dec.decode_ptr = gd;
        dec.decode_bit_pos = 0;
        uint8_t w = utf8_dec_unsigned(&dec, u8g2Fonts.u8g2.font_info.bits_per_char_width);
        uint8_t h = utf8_dec_unsigned(&dec, u8g2Fonts.u8g2.font_info.bits_per_char_height);
        int8_t xo = utf8_dec_signed(&dec, u8g2Fonts.u8g2.font_info.bits_per_char_x);
        int8_t yo = utf8_dec_signed(&dec, u8g2Fonts.u8g2.font_info.bits_per_char_y);
        int8_t delta = utf8_dec_signed(&dec, u8g2Fonts.u8g2.font_info.bits_per_delta_x);
        //字形中心=基线-(h+2*yo)/2；字格中心=基线-6 → 基线修正 dy
        int16_t dy = ((int16_t)h + 2 * (int16_t)yo) / 2 - 6;
        //字形水平中心=槽位+xo+w/2；字格中心=槽位+delta/2 → 起点修正 dx
        int16_t dx = ((int16_t)delta - (int16_t)w) / 2 - (int16_t)xo;
        //库内部绘制时会再应用 xo/yo：字形左=x+dx+xo，字形顶=(y+dy)-(h+yo)
        int16_t tx = x + dx;
        x += u8g2_DrawGlyph(&u8g2Fonts.u8g2, tx, y + dy, (uint16_t)enc);  //返回值=advance，与库同口径步进
        p += len;
        continue;
      }
    }
    //非汉字/缺字形：绘制与 drawUTF8 一致，步进=advance（与库 drawUTF8 的 drawGlyph 同口径）
    //4 字节字符（如 emoji）：字库无此字形，只消费不绘制，避免按截断编码画错字
    if (len == 4)
    {
      p += len;
      continue;
    }
    char tmp[4];
    memcpy(tmp, p, len);
    tmp[len] = 0;
    u8g2Fonts.drawUTF8(x, y, tmp);
    x += u8g2_GetGlyphWidth(&u8g2Fonts.u8g2, (uint16_t)enc);
    p += len;
  }
}

//画电池图标：x_start/y_start 为电池头左上角；percentage 为电量百分比
extern void draw_battery_icon(int16_t x_start, int16_t y_start, uint8_t percentage)
{
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

//系统休眠
extern void esp_sleep(uint32_t minutes)
{
  Serial.printf("esp deep sleep: %d minutes\n", minutes);
  display.hibernate(); //关闭电源屏幕并进入深度睡眠 display.powerOff()为仅关闭电源
  if (minutes > 70) minutes = 70;
  ESP.deepSleep(minutes * 60 * 1000000, WAKE_NO_RFCAL);  //WAKE_RF_DEFAULT  WAKE_RFCAL  WAKE_NO_RFCAL  WAKE_RF_DISABLED
}

//led控制，放在 display 前会导致Busy Timeout不显示，原因不明
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


//带长度保护的拷贝：源串超长时截断，保证 dst 一定有 '\0'；dstSize 为 0 时什么都不做
extern void strcpy_safe(char *dst, size_t dstSize, const char *src)
{
  if (dstSize == 0) return;
  strncpy(dst, src, dstSize - 1);
  dst[dstSize - 1] = 0;
}

// 读取 EEPROM 中的配置
// EEPROM 储存格式为1字节长度+内容
extern uint8_t eeprom_read(uint16_t addr, uint8_t *p)
{
  uint16_t i;
  uint8_t len;

  len = EEPROM.read(addr);

  for (i = 0; i < len; i++)
  {
    *(p + i) = EEPROM.read(addr + 1 + i);
  }

  return len;
}

//写入 EEPROM 中的配置
// EEPROM 储存格式为1字节长度+内容
extern uint8_t eeprom_write(uint16_t addr, uint8_t * p, uint8_t len)
{
  uint16_t i;

  EEPROM.write(addr, len);

  for (i = 0; i < len; i++)
  {
    EEPROM.write(addr + 1 + i, *(p + i));
  }

  return len;
}

// 清空 EEPROM 中的配置
// EEPROM 储存格式为1字节长度+内容
extern void eeprom_clear(uint16_t addr)
{
  uint8_t len = 0;

  EEPROM.write(addr, len);
}