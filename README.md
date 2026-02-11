# eInkCalendar
基于4.2寸三色墨水屏及ESP8266实现的天气日历。心血来潮，咸鱼上捡了一块墨水屏，参考了一些大佬的代码，写了一个天气日历。

## 展示
![daily1](https://raw.githubusercontent.com/217heidai/eInkCalendar/main/doc/daily1.png)
![daily2](https://raw.githubusercontent.com/217heidai/eInkCalendar/main/doc/daily2.png)

## 功能
1. 日历信息展示，包括日期、星期、农历、宜忌、节假日；
2. 节假日信息展示，红色字体突出显示；
3. 天气信息展示，包括天气、最低温度、最高温度、体感舒适度；
4. “一言”信息展示；
5. 为了省电，每次联网刷新后，深度休眠 XX 分钟（具体见代码中 `SLEEP_TIME` 定义）；
6. 每 X 小时（具体见代码中 `REFRESH_FREQUENCY` 定义）联网刷新一次日期、天气信息。如日期发生变化，则强制刷新。

## 计划
1. 翻页展示更多功能...

## 电路图
1. 电路图`doc/SCH_三色墨水屏日历_2021-07-23.json`，请使用 立创EDA 打开

## 代码编译说明
### 依赖库
1. GxEPD2
2. U8g2_for_Adafruit_GFX，需要调整，具体见 lib 目录下源码（感谢`甘草酸不酸`大佬的源码）
3. NTPClient
4. ArduinoJson
### 配置修改
1. `eInkCalendar.ino`中修改 WiFi 信息，设置 `ssid`、`password`;
2. `eInkCalendar.ino`中修改天气链接中 location 信息为当地城市名称拼音（首字母大写，如上海 `Shanghai`），设置 `url_Weather`;

## 更新日志
### 0.0.6【2026/02/11】
1. 支持 SmartConfig 配网。开机自动连接 WiFi，若失败则进入 SmartConfig 配网模式，展示二维码，等待微信扫码配网。

### 0.0.5【2024/06/11】
1. 增加加密货币 BTC、ETH、BNB 金额显示。

### 0.0.4【2023/11/24】
1. 界面调整，增加横线分割；
2. 更换农历、天气、一言获取服务；
3. 增加农历宜忌。

### 0.0.3【2021/12/15】
1. 增加电量显示，绘制电池图标。
   
### 0.0.2【2021/12/08】
1. 参考`甘草酸不酸`大佬的源码，修复了 `U8g2_for_Adafruit_GFX` 库崩溃问题，源码详见 lib 目录；
2. 为了省电，调整为每 8 小时（具体见代码中 `REFRESH_FREQUENCY` 定义）联网刷新一次日期、天气信息。当然如日期发生变化，则会强制刷新；
3. 拆机发现板子上没有温湿度模块，只能问候下 JS 了。

### 0.0.1【2021/08/19】
1. 初始版本。

