// Включение необходимых библиотек
#include <Arduino_GFX_Library.h> // Библиотека для работы с графикой
#include <qrcode_gen.h> // Библиотека для создания QR-кодов

// Определение констант и пинов
#define GFX_BL DF_GFX_BL // Пин подсветки экрана
#define TFT_BL 2 // Пин подсветки TFT экрана
#define SD_CS 10 // Пин для CS SD карты

// Инициализация RGB панели и дисплея
Arduino_ESP32RGBPanel *rgbpanel = new Arduino_ESP32RGBPanel( 
    40, 41, 39, 42, 
    45, 48, 47, 21, 14, 
    5, 6, 7, 15, 16, 4, 
    8, 3, 46, 9, 1, 
    0, 8, 4, 8, 
    0, 8, 4, 8 
); 
 
Arduino_RGB_Display *gfx = new Arduino_RGB_Display( 
    800, 480, rgbpanel, 0, true 
); 
 
// Определение цветов
uint16_t darkGreen = gfx->color565(69, 182, 59);
uint16_t darkRed = gfx->color565(220, 26, 11);

// Настройка инициализации
void setup() {
    gfx->begin(); // Начало работы с дисплеем
    gfx->fillScreen(WHITE); // Заполнение экрана белым цветом

    Serial.begin(9600); // Инициализация последовательного порта
    pinMode(TFT_BL, OUTPUT); // Установка пина подсветки экрана как выход
    digitalWrite(TFT_BL, HIGH); // Включение подсветки экрана

    gfx->setRotation(1); // Установка ориентации экрана
}

 
// Основной цикл программы
void loop() {
    // Проверка доступности данных на последовательном порту
    if (Serial.available() > 0) {
        // Чтение данных до символа новой строки
        String receivedData = Serial.readStringUntil('\n');
        // Проверка принятых данных
        if (receivedData.equals("ok")) {
            // Вывод статуса "ok" на экран
            printStatus(92, 371, 244, 523, 80, 413, 297, 187, 523, 80, darkGreen);
        } else if (receivedData.equals("error")) {
            // Вывод статуса "error" на экран
            printStatus(105, 275, 375, 535, 80, 375, 275, 105, 535, 80, darkRed);
        } else {
            // Вывод QR-кода на экран
            displayQRCode(receivedData);
        }
    }
}
 

// Функция для вывода статуса на экран
void printStatus(int x, int y, int x1, int y1, int size, int xx, int yx, int xx1, int yx1, int xsize, uint16_t color) {
    // Очистка экрана
    gfx->fillScreen(WHITE);
    // Вычисление координат для рисования фигур
    float dx = (size / 2.0) * (y - y1) / sqrt(sq(x - x1) + sq(y - y1));
    float dy = (size / 2.0) * (x - x1) / sqrt(sq(x - x1) + sq(y - y1));
    float dxx = (xsize / 2.0) * (yx - yx1) / sqrt(sq(xx - xx1) + sq(yx - yx1));
    float dyx = (xsize / 2.0) * (xx - xx1) / sqrt(sq(xx - xx1) + sq(yx - yx1));
    // Рисование треугольников
    gfx->fillTriangle(x + dx, y - dy, x - dx, y + dy, x1 + dx, y1 - dy, color);
    gfx->fillTriangle(x - dx, y + dy, x1 - dx, y1 + dy, x1 + dx, y1 - dy, color);
    gfx->fillTriangle(xx + dxx, yx - dyx, xx - dxx, yx + dyx, xx1 + dxx, yx1 - dyx, color);
    gfx->fillTriangle(xx - dxx, yx + dyx, xx1 - dxx, yx1 + dyx, xx1 + dxx, yx1 - dyx, color);
    gfx->flush(); // Применение изменений на экране
    delay(3000); // Задержка для отображения статуса
    gfx->fillScreen(WHITE); // Очистка экрана после отображения
}




// Функция для отображения QR-кода на экране
void displayQRCode(String data) {
    gfx->fillScreen(WHITE); // Очистка экрана
    QRCode qrcode;
    uint8_t qrcodeData[qrcode_getBufferSize(5)];
    qrcode_initText(&qrcode, qrcodeData, 4, ECC_LOW, data.c_str());
    int size = qrcode.size;
    int moduleSize = 10;
    int offsetX = (gfx->width() - size * moduleSize) / 2;
    int offsetY = (gfx->height() - size * moduleSize) / 2;
    // Рисование QR-кода
    for (int y = 0; y < size; y++) {
        for (int x = 0; x < size; x++) {
            if (qrcode_getModule(&qrcode, x, y)) {
                gfx->fillRect(x * moduleSize + offsetX, y * moduleSize + offsetY, moduleSize, moduleSize, BLACK);
            }
        }
    }
    gfx->flush(); // Применение изменений на экране
}