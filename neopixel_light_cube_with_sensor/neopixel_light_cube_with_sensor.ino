// Подключаем библиотеки:
#include <Wire.h>                                                         //  Для работы с шиной I2C
#include <SparkFun_APDS9960.h>                                            //  Для работы с датчиком APDS-9960
#include <iarduino_NeoPixel.h>                                            //  Подключаем библиотеку iarduino_NeoPixel для работы со светодиодами NeoPixel
uint8_t       neo_pin      = 10;                                          //  Вывод, к которому подключены модули NeoPixel
uint16_t      modul_number = 8;                                           //  Количество модулей
iarduino_NeoPixel led(neo_pin, modul_number*4 );                          //  Объявляем объект LED указывая (№ вывода Arduino к которому подключён модуль NeoPixel, количество используемых светодиодов)
SparkFun_APDS9960 apds = SparkFun_APDS9960();                             //  Определяем объект apds, экземпляр класса SparkFun_APDS9960
// Объявляем переменные:
uint8_t       j;                                                          //  Объявляем переменную для хранения значения сдвига спектра цветов для всех светодиодов (от 0 до 255)
uint8_t       k;                                                          //  Объявляем переменную для хранения положения сдвига спектра цвета для каждого светодиода на спектре j (зависит от количества светодиодов)
uint8_t       r, g, b;                                                    //  Объявляем переменную для хранения цветов RGB для каждого светодиода
uint8_t       z             = 1;                                          //  Определяем константу указывающую задержку в мс (чем выше значение, тем медленнее перелив цветов)
uint8_t       flg, setting  = 0;                                          //  Флаг включения/выключения света
uint8_t       mode          = 0;                                          //  Флаг режима работы светодиодов
uint8_t       proximityData = 0;                                          //  Переменная, хранящая значение расстояния до модуля APDS9960
uint8_t       brightness    = 255;                                        //  Переменная значения яркости
uint32_t      waiting_time  = 7000;                                       //  Время ожидания при настройке яркости
uint32_t      work_time;                                                  //  Счётчик времени при настройке яркости
byte          on_off        = 0;                                          //  Триггер включения/выключения света

void setup() {
  //  Инициализируем адресные светодиоды:
  led.begin();
  //  Инициализируем модуль жестов:
  apds.init();                                                            //  Если инициализация модуля жестов прошла успешно, то ...
  //  Устанавливаем коэффициент усиления приёмника:                       //  Доступные значения: 1х, 2х, 4х, 8х (GGAIN_1X, GGAIN_2X, GGAIN_4X, GGAIN_8X). Чем выше коэффициент тем выше чувствительность
  apds.setGestureGain(GGAIN_4X);                                          //  Если установлен коэффициент усиления приёмника в режиме обнаружения жестов, то ...
  //  Устанавливаем силу тока драйвера ИК-светодиода:                     //  Доступные значения: 100мА, 50мА, 25мА, 12.5мА (LED_DRIVE_100MA, LED_DRIVE_50MA, LED_DRIVE_25MA, LED_DRIVE_12_5MA). Чем выше сила тока, тем выше чувствительность.
  apds.setGestureLEDDrive(LED_DRIVE_100MA);                               //  Если устанавлена сила тока драйвера (яркость) ИК-светодиода для обнаружения жестов, то ...
  //  Разрешаем режим обнаружение жестов:
  apds.enableGestureSensor(false);                                        //  Если инициализация режима обнаружения жестов прошла успешно, то
  led.setColor(NeoPixelAll,0, 0, 0);                                      //  Выключаем светодиоды
  led.write();                                                            //  
  //  Ждём завершение инициализации и калибровки:
  delay(500);                                                             //  Задержка 0,5с
}

void loop() {
  // Проверяем наличие движения
  if (apds.isGestureAvailable()) {                                        //  Если зафиксировано движение, то ...
    switch (apds.readGesture()) {                                         //  Сверяем значение соответствующее жесту ...
      // Проверяем жест:
//==========================================================//
      // "СЕВЕР" - УВЕЛИЧЕНИЕ СКОРОСТИ АНИМАЦИИ
      case DIR_UP:
      if(z>=1 && z <=11)    {z += 5;} else                                //  Если параметр скорости задан от 1мс до 11мс, то шаг равен 5мс
      if(z > 11 && z <=101) {z += 20;}else                                //  Если параметр скорости задан от 11мс до 101мс, то шаг равен 20мс
      if(z > 101 && z <=500){z += 50;}else                                //  Если параметр скорости задан от 101мс до 500мс, то шаг равен 50мс
      if(z > 500)           {z = 1;}                                      //  При превышении значения шаг сбрасывается
      break;
//==========================================================//      
      // "ЮГ" - УМЕНЬШЕНИЕ СКОРОСТИ АНИМАЦИИ
      case DIR_DOWN:
      if(z<1)               {z = 500;}else                                //  При выходе из заданного промежутка значений шаг сбрасывается
      if(z >=1 && z <=11)   {z -= 5;} else                                //  Если параметр скорости задан от 1мс до 11мс, то шаг равен 5мс
      if(z > 11 && z <= 101){z -= 20;}else                                //  Если параметр скорости задан от 1мс до 11мс, то шаг равен 5мс
      if(z > 101 && z <=500){z -= 50;}                                    //  Если параметр скорости задан от 1мс до 11мс, то шаг равен 5мс
      break;
//==========================================================//
      // "ЗАПАД" - ЛИСТАЕМ РЕЖИМЫ ВНИЗ
      case DIR_LEFT:
      mode--;                                                             //  Уменьшаем номер режима на единицу
      if(mode < 1){mode = 11;}                                            //  Если значение меньше допустимого, то переходим к максимально большому значению
      break;
//==========================================================//      
      // "ВОСТОК" - ЛИСТАЕМ РЕЖИМЫ ВВЕРХ
      case DIR_RIGHT:
      mode++;                                                             //  Увеличиваем номер режима на единицу
      if(mode > 11){mode = 1;}                                            //  Если значение больше допустимого, то переходим к минимальному значению
      break;
//==========================================================//      
      // "ОТДАЛЕНИЕ" - ПЕРЕХОД В РЕЖИМ ЯРКОСТИ
      case DIR_FAR:
      setting   = 1;                                                      //  Ставим флаг перехода в режим настройки яркости
      flg       = 1;                                                      //  Ставим флаг смены работы режима датчика жестов - с чтения жестов на чтение приближения
      work_time = millis();                                               //  Обновляем счётчик
      break;
//==========================================================//      
      // "ПРИБЛИЖЕНИЕ" - ПЕРЕХОД В РЕЖИМ ВЫКЛЮЧЕНИЯ СВЕТА
      case DIR_NEAR:
      mode = 12;                                                          //  Устанавливаем режим выключения света
      break;
    }
  }
  // РЕЖИМЫ РАБОТЫ СВЕТОДИОДОВ:
  switch (mode) {                                                         //  Сверяем значение соответствующее режиму работы
//==========================================================//    
    //  ПЕРЕЛИВ ВСЕХ ЦВЕТОВ РАДУГИ
    case 1:
    j++;                                                                  //  Смещаем спектр цветов для всех светодиодов
    for(uint16_t i=0; i<led.count(); i++){                                //  Проходим по всем светодиодам
        k=((uint16_t)(i*256/led.count())+j);                              //  Определяем положение очередного светодиода на смещённом спектре цветов
        if(k<85) {        b=0; r=k*3; g=255-r;}else                       //  Перелив от зелёного к красному, через жёлтый
        if(k<170){k-=85;  g=0; b=k*3; r=255-b;}else                       //  Перелив от красного к синему  , через фиолетовый
                 {k-=170; r=0; g=k*3; b=255-g;}                           //  Перелив от синего   к зелёному, через голубой
        led.setColor(i,                                                   //  Устанавливаем выбранный цвет для очередного светодиода
                     map(r, 0, 255, 0, brightness),                       //  Задаём зависимость яркости красного от параметра brightness, который определяется в настройках яркости
                     map(g, 0, 255, 0, brightness),                       //  Задаём зависимость яркости зелёного от параметра brightness, который определяется в настройках яркости
                     map(b, 0, 255, 0, brightness));                      //  Задаём зависимость яркости синего от параметра brightness, который определяется в настройках яркости
    }
    led.write();                                                          //  Записываем цвета в светодиоды
    delay(z);                                                             //  Устанавливаем задержку
    break;
//==========================================================//    
    //  ВКЛЮЧЕНИЕ СЛУЧАЙНЫМ ЦВЕТОМ
    case 2:
    for(uint16_t t=0; t<led.count(); t++){                                //  Проходим по всем светодиодам
      int8_t i=random(3);                                                 //  Определяем какой из трех составляющих цветов будет отсутствовать R, G, или B
      if(i==0){r=0;}else{r=random(0xFF);}                                 //  Определяем случайное значение для переменной r
      if(i==1){g=0;}else{g=random(0xFF);}                                 //  Определяем случайное значение для переменной g
      if(i==2){b=0;}else{b=random(0xFF);}                                 //  Определяем случайное значение для переменной b
      led.setColor(t,                                                     //  Устанавливаем цвет r,g,b для случайного светодиода
                   map(r, 0, 255, 0, brightness),                         //  Задаём зависимость яркости красного от параметра brightness, который определяется в настройках яркости
                   map(g, 0, 255, 0, brightness),                         //  Задаём зависимость яркости зелёного от параметра brightness, который определяется в настройках яркости
                   map(b, 0, 255, 0, brightness));                        //  Задаём зависимость яркости синего от параметра brightness, который определяется в настройках яркости
    }
    led.write();                                                          //  Записываем цвета в светодиоды
    delay(z);                                                             //  Устанавливаем задержку
    break;
//==========================================================//    
    //  ВСЕ СВЕТОДИОДЫ ЗАЖИГАЮТСЯ ОДНИМ ЦВЕТОМ
    case 3:
    uint16_t f;                                                           //  Определяем переменную как коэффициент яркости
    if(f==0  ){f=42; }else                                                //  f=60°  (0 ... 42  ... 255   =   0° ... 60°  ... 360°)
    if(f==42 ){f=85; }else                                                //  f=120° (0 ... 85  ... 255   =   0° ... 120° ... 360°)
    if(f==85 ){f=127;}else                                                //  f=180° (0 ... 127 ... 255   =   0° ... 180° ... 360°)
    if(f==127){f=170;}else                                                //  f=240° (0 ... 170 ... 255   =   0° ... 240° ... 360°)
    if(f==170){f=212;}else                                                //  f=300° (0 ... 212 ... 255   =   0° ... 300° ... 360°)
    if(f==212){f=0;  }                                                    //  f=360° (0 ... 255 ... 255   =   0° ... 360° ... 360°)
    if(f<85) {        b=0; r=f*3; g=255-r;}else                           //  Перелив от зелёного к красному, через жёлтый
    if(f<170){f-=85;  g=0; b=f*3; r=255-b;}else                           //  Перелив от красного к синему  , через фиолетовый
             {f-=170; r=0; g=f*3; b=255-g;}                               //  Перелив от синего   к зелёному, через голубой
    led.setColor(NeoPixelAll,                                             //  Устанавливаем выбранный цвет для очередного светодиода
                 map(r, 0, 255, 0, brightness),                           //  Задаём зависимость яркости красного от параметра brightness, который определяется в настройках яркости
                 map(g, 0, 255, 0, brightness),                           //  Задаём зависимость яркости зелёного от параметра brightness, который определяется в настройках яркости
                 map(b, 0, 255, 0, brightness));                          //  Задаём зависимость яркости синего от параметра brightness, который определяется в настройках яркости
    led.write();                                                          //  Записываем цвета в светодиоды
    delay(z*10);                                                          //  Устанавливаем задержку
    break;
//==========================================================//
    //  ГОРИТ СВЕЧА
    case 4:
    for(uint16_t m=0; m<led.count(); m++){                                //  Проходим по всем светодиодам
        if(m%2==0){                                                       //  Каждый второй светодиод настраиваем гореть красным
        r = random(10,50);                                                //  Определяем случайное значение яркости красного
        led.setColor(m,                                                   //  Устанавливаем выбранный цвет для очередного светодиода
                     map(r, 0, 255, 0, brightness),                       //  Задаём зависимость яркости красного от параметра brightness, который определяется в настройках яркости
                     0,                                                   //  Устанавливаем зелёный равным 0
                     0);                                                  //  Устанавливаем синий равным 0
    }else {                                                               //  Оставшиеся светодиоды настраиваем гореть жёлтым
      r = random(50,256); g = r;                                          //  Определяем случайное значение яркости жёлтого
      led.setColor(m,                                                     //  Устанавливаем выбранный цвет для очередного светодиода
                   map(r, 0, 255, 0, brightness),                         //  Задаём зависимость яркости красного от параметра brightness, который определяется в настройках яркости
                   map(g, 0, 255, 0, brightness),                         //  Задаём зависимость яркости зелёного от параметра brightness, который определяется в настройках яркости
                   0);                                                    //  Устанавливаем синий равным 0
      }
    }
    led.write();                                                          //  Записываем цвета в светодиоды
    delay(random(10,100));                                                //  Устанавливаем случаное значение времени задержки для имитации горения
    break;
//==========================================================//
    //  ГОРИТ КРАСНЫМ
    case 5:
    r = 255; g = 0; b = 0;                                                //  Выставляем значение RGB-спектра в красный цвет
    led.setColor(NeoPixelAll,                                             //  Устанавливаем выбранный цвет для всех светодиодов
                 map(r, 0, 255, 0, brightness),                           //  Задаём зависимость яркости красного от параметра brightness, который определяется в настройках яркости
                 g,                                                       //  Устанавливаем зелёный равным 0
                 b);                                                      //  Устанавливаем синий равным 0
    led.write();                                                          //  Записываем цвета в светодиоды
    break;
//==========================================================//
    //  ГОРИТ ЗЕЛЁНЫМ
    case 6:
    r = 0; g = 255; b = 0;                                                //  Выставляем значение RGB-спектра в зелёный цвет
    led.setColor(NeoPixelAll,                                             //  Устанавливаем выбранный цвет для всех светодиодов
                 r,                                                       //  Устанавливаем красный равным 0
                 map(g, 0, 255, 0, brightness),                           //  Задаём зависимость яркости зелёного от параметра brightness, который определяется в настройках яркости
                 b);                                                      //  Устанавливаем синий равным 0
    led.write();                                                          //  Записываем цвета в светодиоды
    break;
//==========================================================//
    //  ГОРИТ СИНИМ
    case 7:
    r = 0; g = 0; b = 255;                                                //  Выставляем значение RGB-спектра в синий цвет
    led.setColor(NeoPixelAll,                                             //  Устанавливаем выбранный цвет для всех светодиодов
                 r,                                                       //  Устанавливаем красный равным 0
                 g,                                                       //  Устанавливаем зелёный равным 0
                 map(b, 0, 255, 0, brightness));                          //  Задаём зависимость яркости синего от параметра brightness, который определяется в настройках яркости
    led.write();                                                          //  Записываем цвета в светодиоды
    break;
//==========================================================//
    //  ГОРИТ ЖЁЛТЫМ
    case 8:
    r = 255; g = 255; b = 0;                                              //  Выставляем значение RGB-спектра в жёлтый цвет
    led.setColor(NeoPixelAll,                                             //  Устанавливаем выбранный цвет для всех светодиодов
                 map(r, 0, 255, 0, brightness),                           //  Задаём зависимость яркости красного от параметра brightness, который определяется в настройках яркости
                 map(g, 0, 255, 0, brightness),                           //  Задаём зависимость яркости зелёного от параметра brightness, который определяется в настройках яркости
                 b);                                                      //  Устанавливаем синий равным 0
    led.write();                                                          //  Записываем цвета в светодиоды
    break;
//==========================================================//
    //  ГОРИТ ФИОЛЕТОВЫЙ
    case 9:
    r = 255; g = 0; b = 255;                                              //  Выставляем значение RGB-спектра в фиолетовый цвет
    led.setColor(NeoPixelAll,                                             //  Устанавливаем выбранный цвет для всех светодиодов
                 map(r, 0, 255, 0, brightness),                           //  Задаём зависимость яркости красного от параметра brightness, который определяется в настройках яркости
                 g,                                                       //  Устанавливаем зелёный равным 0
                 map(b, 0, 255, 0, brightness));                          //  Задаём зависимость яркости синего от параметра brightness, который определяется в настройках яркости
    led.write();                                                          //  Записываем цвета в светодиоды
    break;
//==========================================================//
    //  ГОРИТ ГОЛУБЫМ
    case 10:
    r = 0; g = 255; b = 255;                                              //  Выставляем значение RGB-спектра в голубой цвет
    led.setColor(NeoPixelAll,                                             //  Устанавливаем выбранный цвет для всех светодиодов
                 r,                                                       //  Устанавливаем красный равным 0
                 map(g, 0, 255, 0, brightness),                           //  Задаём зависимость яркости зелёного от параметра brightness, который определяется в настройках яркости
                 map(b, 0, 255, 0, brightness));                          //  Задаём зависимость яркости синего от параметра brightness, который определяется в настройках яркости
    led.write();                                                          //  Записываем цвета в светодиоды
    break;
//==========================================================//
    //  ГОРИТ БЕЛЫМ
    case 11:
    r = 255; g = 255; b = 255;                                            //  Выставляем значение RGB-спектра в белый цвет
    led.setColor(NeoPixelAll,                                             //  Устанавливаем выбранный цвет для всех светодиодов
                 map(r, 0, 255, 0, brightness),                           //  Задаём зависимость яркости красного от параметра brightness, который определяется в настройках яркости
                 map(g, 0, 255, 0, brightness),                           //  Задаём зависимость яркости зелёного от параметра brightness, который определяется в настройках яркости
                 map(b, 0, 255, 0, brightness));                          //  Задаём зависимость яркости синего от параметра brightness, который определяется в настройках яркости
    led.write();                                                          //  Записываем цвета в светодиоды
    break;
//==========================================================//
    //  ВЫКЛЮЧЕНО
    case 12:
    r = 0; g = 0; b = 0;                                                  //  Выставляем значение RGB-спектра в чёрный цвет (включено)
    led.setColor(NeoPixelAll,                                             //  Устанавливаем выбранный цвет для всех светодиодов
                 r,                                                       //  Устанавливаем красный равным 0
                 g,                                                       //  Устанавливаем зелёный равным 0
                 b);                                                      //  Устанавливаем синий равным 0
    led.write();                                                          //  Записываем цвета в светодиоды
    break;    
  }
//==========================================================//
  // РЕЖИМ НАСТРОЙКИ ЯРКОСТИ:
  if(setting){                                                            //  Если стоит флаг входа в режим настройки, то проверяем...
    if(work_time+waiting_time > millis()){                                //  с момента входа в режим настройки не прошло ли время waiting_time, и если не прошло то...
      if(flg){                                                            //  проверяем, стоит ли флаг смены режима работы датчика жестов. Если стоит, то
        flg = 0;                                                          //  сбрасываем флаг,
        apds.disableGestureSensor();                          delay(500); //  выключаем режим чтения жестов, ждём 0,5с,
        apds.enableProximitySensor(false);                                //  включаем режим чтения приближения,
        led.setColor(NeoPixelAll,0 ,0 ,0 );     led.write();  delay(500); //  выключаем светодиоды, ждём 0,5с,
        led.setColor(NeoPixelAll,25 ,25 ,25 );  led.write();  delay(500); //  включаем светодиоды, ждём 0,5с,
        led.setColor(NeoPixelAll,0 ,0 ,0 );     led.write();              //  выключаем светодиоды.
      }
      if(apds.readProximity(proximityData)){                              //  Если датчик считывает приближение, то...
        brightness = map(proximityData,0, 255,255,10);                    //  регулируем яркость, "перевернув" диапазон значений, приходящий с датчика
      }
    } else {                                                              //  Если время waiting_time прошло, то
      setting = 0;                                                        //  сбрасываем флаг режима настройка яркости,
      apds.disableProximitySensor();                          delay(500); //  выключаем режим чтения приближения, ждём 0,5с,
      apds.enableGestureSensor(false);                                    //  включаем режим чтения жестов,
      led.setColor(NeoPixelAll,0 ,0 ,0 );       led.write();  delay(500); //  выключаем светодиоды, ждём 0,5с,
      led.setColor(NeoPixelAll,25 ,25 ,25 );    led.write();  delay(500); //  включаем светодиоды, ждём 0,5с,
      led.setColor(NeoPixelAll,0 ,0 ,0 );       led.write();              //  выключаем светодиоды.
    }
  }
  delay(100);                                                             //  Задержка, чтобы не перегружать шину I2C постоянными запросами
}