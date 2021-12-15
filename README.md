# eInkCalendar
基于4.2寸三色墨水屏及ESP8266实现的天气日历。心血来潮，咸鱼上捡了一块墨水屏，参考了一些大佬的代码，写了一个天气日历。

## 展示
![workday](https://raw.githubusercontent.com/217heidai/eInkCalendar/main/doc/workday.jpeg)
![holiday](https://raw.githubusercontent.com/217heidai/eInkCalendar/main/doc/holiday.jpeg)
![battery](https://raw.githubusercontent.com/217heidai/eInkCalendar/main/doc/battery.jpeg)

## 功能
1. 日历信息展示，包括日期、星期、农历、节假日；
2. 节假日信息展示，红色字体突出显示；
3. 天气信息展示，包括天气、最低温度、最高温度、紫外线；
4. “一言”信息展示；
5. 为了省电，每次执行完任务后，深度休眠 60 分钟（具体见代码中 `SLEEP_TIME` 定义）；
6. 每 8 小时（具体见代码中 `REFRESH_FREQUENCY` 定义）联网刷新一次日期、天气信息。如日期发生变化，则强制刷新。

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
2. `eInkCalendar.ino`中修改心知天气 API KEY 信息，设置 `url_FutureWeather`、`url_LifeIndex`;
3. `eInkCalendar.ino`中修改位置信息，支持到城市级别，设置 `url_FutureWeather`、`url_LifeIndex`。

## 更新日志
### 0.0.3【2021/12/15】
1. 增加电量显示，绘制电池图标
   
### 0.0.2【2021/12/08】
1. 参考`甘草酸不酸`大佬的源码，修复了 `U8g2_for_Adafruit_GFX` 库崩溃问题，源码详见 lib 目录；
2. 为了省电，调整为每 8 小时（具体见代码中 `REFRESH_FREQUENCY` 定义）联网刷新一次日期、天气信息。当然如日期发生变化，则会强制刷新；
3. 拆机发现板子上没有温湿度模块，只能问候下 JS 了。

### 0.0.1【2021/08/19】
1. 初始版本。

