# eInkCalendar
基于4.2寸三色墨水屏及ESP8266实现的天气日历。心血来潮，咸鱼上捡了一块墨水屏，参考了一些大佬的代码，写了一个天气日历。

## 展示
![workday](../eInkCalendar/doc/workday.jpeg)
![holiday](../eInkCalendar/doc/holiday.jpeg)

## 功能
1. 日历信息展示，包括日期、星期、农历、节假日；
2. 节假日信息展示，红色字体突出显示；
3. 天气信息展示，包括天气、最低温度、最高温度、紫外线；
4. “一言”信息展示；
5. 每隔1小时自动联网刷新。

## 计划
1. 采集、展示电量；
2. 采集、展示环境温度、湿度；
3. 翻页展示更多功能...

## 代码编译说明
1. `eInkCalendar.ino`中修改 WiFi 信息，设置 `ssid`、`password`;
2. `eInkCalendar.ino`中修改心知天气 API KEY 信息，设置 `url_FutureWeather`、`url_LifeIndex`;
3. `eInkCalendar.ino`中修改位置信息，支持到城市级别，设置 `url_FutureWeather`、`url_LifeIndex`。

