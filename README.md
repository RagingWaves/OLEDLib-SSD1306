# OLEDLib-SSD1306



## 文件结构

Lib：

​	OLED：图形驱动

​		Font.h：字符点阵

​	SPI：SPI函数

​	sys：延时函数

​	main.c：示例程序



------



## 函数说明



### 显示位图

`void OLED_DBMP(int x0, int y0, int bmpx, int bmpy, const char *bmp);`

##### 参数：

x0、y0：坐标起点；

bmpx、bmpy：位图尺寸；

*bmp：位图数据。

##### 说明：

位图示例格式：

![位图格式](https://raw.githubusercontent.com/RagingWaves/OLEDLib-SSD1306/develop/%E4%BD%8D%E5%9B%BE%E6%A0%BC%E5%BC%8F.png?token=AKVGV4RRDMAZYTMFDWOX2XLBIQBBQ)



### 设置字体大小

`void OLED_FSize(int size);`

##### 参数：

size：字体大小，0为6x8，1为8x16。



### 打印字符串

`void OLED_PStr(int x0, int y0, const char *str);`

##### 参数：

x0、y0：坐标起点；

*str：字符串。

##### 说明：

当该行显示不下时将自动换行。



### 打印单个字符

`void OLED_PCh(int x0, int y0, char ch);`

##### 参数：

x0、y0：坐标起点；

ch：字符。

##### 说明：

不会自动换行，可在行末强制显示字符；

支持'\r'、'\n'换行。



### printf

`void OLED_printf(int x0, int y0, const char *str, ...);`

##### 参数：

x0、y0：坐标起点；

*str：字符串；

##### 支持的转义字符：

| 符号 | 说明         |
| ---- | ------------ |
| %c   | 打印单个字符 |
| %s   | 打印字符串   |
| %d   | 打印整数     |
| %f   | 打印浮点数   |

注：丐中丐版printf，只支持上面表格中的用法（不支持%.2f这类用法）。



### 显示画面

`void OLED_Display(void);`

##### 说明：

显示所缓存的画面，必须调用此函数才能显示。



### 清空缓存

`void OLED_Clean(void);`

##### 说明：

将缓存的图形数据清除。



### 初始化

`void OLED_Init(void);`

##### 说明：

OLED驱动初始化。
