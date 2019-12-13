# AVR-Neopixel-Equalizer
Neopixel ws2812 ledpanel + atmega128 + joystick + MIC sensor<br>
atmega128보드 ,네오픽셀(ws2812), 사운드 센서를 이용한 이퀄라이저

# FUSEBIT
EXTENDED : 0xFF<br>
HIGH     : 0xC9<br>
LOW      : 0xEF<br>

Ext.16MHz Crystal,16MHz,4ms

# CIRCUIT
|Atmega128|Part Pin|Part|
|:------:|:----:|:----:|
|PF1|OUT|Sound Sensor(analog)|
|PF3|Y|Joystick y axis|
|PF4|X|Joystick x axis|
|PD0|B|Joystick button|
|PB2|Di|Neopixel 16x16|

# Parts
* [Atmega128 Development Board](http://www.newtc.co.kr/dpshop/shop/item.php?it_id=1314599855)
![atmega128](./img/atmega128.jpg)
* [Neopixel](http://www.devicemart.co.kr/goods/view?no=1328622)
![neopixel](./img/neopixel.jpg)
* [Joystick](http://www.devicemart.co.kr/goods/view?no=10916338)
![neopixel](./img/joystick.gif)
* [Sound sensor](http://www.devicemart.co.kr/goods/view?no=28451)
![neopixel](./img/sensor.jpg)

# CODE
## python
|File||
|:------:|:----:|
|font.py|밝기숫자를 배열데이터로 생성</br>(making brightness number data)|
|rgb generator.py|16x16 픽셀의 png이미지를 배열데이터로 생성</br>(Convert 16x16 pixel png to array data)|
