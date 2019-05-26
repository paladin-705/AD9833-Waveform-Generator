/*
                    Назначение выводов

    ┌─────┬──────────────────────────────────────────────┐
    │ Pin │ Назначение                                   │
    ├─────┼──────────────────────────────────────────────┤
    │   2 │ Регулировка частоты. DT                      │
    │   3 │ Регулировка частоты. CLK                     │
    │   4 │ Регулировка частоты. SW                      │
    ├─────┼──────────────────────────────────────────────┤
    │   5 │ Регулировка амплитуды и формы сигнала. DT    │
    │   6 │ Регулировка амплитуды и формы сигнала. CLK   │
    │   7 │ Регулировка амплитуды и формы сигнала. SW    │
    ├─────┼──────────────────────────────────────────────┤
    │   8 │ Потенциометр. SPI SS Pin                     │
    ├─────┼──────────────────────────────────────────────┤
    │   9 │ AD9833. SPI SS Pin                           │
    ├─────┼──────────────────────────────────────────────┤    
    │  11 │ SPI MOSI Pin                                 │
    │  13 │ SPI SCK Pin                                  │
    ├─────┼──────────────────────────────────────────────┤
    │  A4 │ Дисплей. SDA                                 │
    │  A5 │ Дисплей. SCL                                 │
    └─────┴──────────────────────────────────────────────┘

    Схема управления:
    1. freqEnc - служит для регулировки частоты выходного сигнала
        1.1. Ручка потенциометра - служит для установки значения частоты. COMING SOON: При быстром кручении ручки, шаг изменения увеличивается
        1.2. Короткое нажатие на кнопку - изменяет шаг регулировки частоты. По умолчанию шаг 1 Гц. Каждое нажатие увеличивает его в 10 раз, 
             изменяется циклически - после достижения максимального значения, следуюший шелчок, устанавливае шаг равным 100 мГц
        1.3. Длинное нажатие на кнопку - стирает (устанавливает в ноль) все разряды меньшие множителя частоты. Пример: Частота равна 12 345 Гц,
             Множитель равен 1 кГц. После длинного нажатия частота станет равной 12 000 Гц.
    2. ampEnc - регулирует амплитуду и форму выходного сигнала (знаю, что странно, но сейчас я ограничен в кнопках. Потом разнесу эти функции 
       на разные регуляторы)
        2.1. Ручка потенциометра - служит для установки амплитуды выходного сигнала
        2.2. Корткое нажатие на кнопку - изменяет форму сигнала. У AD9833 доступны: синусоидальный, треугольный, прямоугольный и прямоугольный
             прошедший через делитель частоты на 2 сигналы. Принажатиях изменяются циклически
        2.3. Длинное нажатие на кнопку - TO DO
    3. Управление с компьютера по Serial порту - TO DO

    Для того, чтобы включить вывод отладочной информации по Serial порту, необхоодимо добавить: #define DEBUG;
*/

#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

#include "FrequencyEncoder.h"
#include "AmplitudeEncoder.h"
#include "DigiPot.h"
#include "AD9833.h"

#define DEBUG

// ================================================
//          Назначение выводов
// ================================================
uint8_t FreqEnc_SW = 4;

uint8_t AmpEnc_DT = 5;
uint8_t AmpEnc_CLK = 6;
uint8_t AmpEnc_SW = 7;

uint8_t DigiPot_SS = 8;
uint8_t AD9833_SS = 9;


// ================================================
//          Инициализация обьектов
// ================================================
FrequencyEncoder freqEnc(FreqEnc_SW);                           // Работает на прерываниях
AmplitudeEncoder ampEnc(AmpEnc_DT, AmpEnc_CLK, AmpEnc_SW);      // Необходимо опрашивать вручную

DigiPot digiPot(DigiPot_SS);
AD9833 dds(AD9833_SS);

LiquidCrystal_I2C lcd(0x27,16,2);  // Устанавливаем дисплей


// ================================================
//          Глобальные переменные
// ================================================

// Перменные для Генератора
volatile uint32_t freq;                                         // Установленная частота. Не в Герцах! 1 еденица частоты = 100 мГц

// Переменные для потенциометра
uint8_t digiPotPos;                                             // Положение потенциометра

// Константы для расчёта амплитуды по положению потенциометра и наоборот

// Допустимые сигналы
SignalType signals[2] = { SINE_WAVE, TRIANGLE_WAVE };
uint8_t singalPos = 0;

// Потенциометр
const double coefA = 32.64;
const double coefB = 32.64;

const double coefR_AB = 1200.0 / 256.0;
const int coefR1 = 53 + 100;
const int coefR2 = 53 + 100;

const int slaveSelectPin = 10;

// Прототипы функций
float OpAmpK(int potPos);

void printFreq(uint32_t freq);
void printSignalType(SignalType signal);
void printDigiPotPos(uint8_t pos);
void printGainAndAmplitude(uint8_t pos);

void freqEncInterupt();


void setup()
{
    Serial.begin(9600);

    SPI.begin();

    // Назначение прерываний
    // TO DO: Использование двух пинов прерываний для этого излишне. Переделать под использование лишь одного
    attachInterrupt(0, freqEncInterupt, CHANGE);
    attachInterrupt(1, freqEncInterupt, CHANGE);

    // Установка начальный параметров энкодера регулировки частоты
    freqEnc.setPos(10000);                  // Устанавливаем частоту 1 кГц

    // Установка начальных параметров амплитуды выходного сигнала
    digiPot.write(ampEnc.getPos());

    // Установка начальных параметров AD9833
    dds.reset();                            // Сбрасываем регистры AD9833
    dds.setFrequency(freqEnc.getPos());     
    dds.setSignal(SINE_WAVE);

    // Экран
    lcd.init();                     
    lcd.backlight();

    printFreq(dds.getFrequency());
    printDigiPotPos(digiPot.getPos());
    printGainAndAmplitude(digiPot.getPos());
    printSignalType(signals[singalPos]);
}

void loop()
{
    // Проверка и обновление состояния энкодера
    ampEnc.check();
    ampEnc.checkButton();

    freqEnc.checkButton();

    // Получение значений положения энкодеров
    uint32_t newFreq = freqEnc.getPos();
    uint8_t newDigiPotPos = ampEnc.getPos();

    // ================================================
    //          Регулировка параметров AD9833
    // ================================================
    if(freq != newFreq)
    {
        dds.setFrequency(newFreq);
        freq = newFreq;

        // LOG
        printFreq(freq);
    }


    // ================================================
    //          Регулировка параметров амплитуды 
    //                  и формы сигнала
    // ================================================
    if(digiPotPos != newDigiPotPos)
    {
        digiPot.write(digiPotPos);
        digiPotPos = newDigiPotPos;

        // LOG
        printDigiPotPos(digiPotPos);
        printGainAndAmplitude(digiPotPos);
    }

    if(ampEnc.is_clicked())
    {
        singalPos = (singalPos + 1) % (sizeof(signals) / sizeof(SignalType));
        dds.setSignal(signals[singalPos]);
        ampEnc.resetButton();

        // LOG
        printSignalType(signals[singalPos]);
    }

    if(ampEnc.is_long_clicked())
    {
        // TO DO
        ampEnc.resetButton();
    }
}


// ================================================
//              Служебные функции
// ================================================
float OpAmpK(uint8_t potPos)
{
    // Подробный расчёт формулы приведён в файле Maple
    return ((25.0f * (float)(potPos) + 816.0f) * (-9936.0f + 25.0f * (float)(potPos))) / (((25.0f * (float)(potPos) + 3536.0f)*(-7216.0f + 25.0f*(float)(potPos))));
}

// ================================================
//              Функции вывода информации
// ================================================
void printFreq(uint32_t freq)
{
    // Так как частота в переменной не в Герцах, то выполняется её преобразование
    int32_t fregHzPart = freq / 10;
    int32_t fregmHzPart = freq % 10;

    lcd.setCursor(0, 0);
    lcd.print("F:           ");
    lcd.setCursor(3, 0);
    lcd.print(fregHzPart);
    lcd.print('.');
    lcd.print(fregmHzPart);

    #ifdef DEBUG
        Serial.print("[ INFO ] ");
        Serial.print("FREQ: ");
        Serial.print(fregHzPart);
        Serial.print('.');
        Serial.println(fregmHzPart);
    #endif
}

void printSignalType(SignalType signal)
{
    lcd.setCursor(13, 0);
    switch(signal)
    {
        case SINE_WAVE:
        {
            lcd.print("SIN");
            break;
        }
        case TRIANGLE_WAVE:
        {
            lcd.print("TRI");
            break;
        }
        case SQUARE_WAVE:
        {
            lcd.print("SQR");
            break;
        }
        case HALF_SQUARE_WAVE:
        {
            lcd.print("HSQ");
            break;
        }
        default:
        {
            lcd.print("WTF");
            break;
        }
    }

    #ifdef DEBUG
        Serial.print("[ INFO ] ");
        Serial.print("SIGNAL: ");
        switch(signal)
        {
            case SINE_WAVE:
            {
                Serial.println("SIN");
                break;
            }
            case TRIANGLE_WAVE:
            {
                Serial.println("TRI");
                break;
            }
            case SQUARE_WAVE:
            {
                Serial.println("SQR");
                break;
            }
            case HALF_SQUARE_WAVE:
            {
                Serial.println("HSQ");
                break;
            }
            default:
            {
                Serial.println("WTF");
                break;
            }
        }
    #endif
}

void printDigiPotPos(uint8_t pos)
{
    lcd.setCursor(0, 1);
    lcd.print("POS:   ");
    lcd.setCursor(4, 1);
    lcd.print(pos);

    #ifdef DEBUG
        Serial.print("[ INFO ] ");
        Serial.print("POS: ");
        Serial.println(pos);
    #endif
}

void printGainAndAmplitude(uint8_t pos)
{
    float Vin = 0.3f;    // Амплитуда сигнала с AD9833

    // Значения усилений звеньев зависят от схемы
    float K1 = 3.0f;
    float K2 = OpAmpK(pos);
    float K3 = 3.03f;

    float K = K1*K2*K3;
    float Vout = K * Vin;

    lcd.setCursor(8, 1);
    lcd.print("Vout:      ");
    lcd.setCursor(13, 1);
    lcd.print(K);

    //lcd.setCursor(12, 1);
    lcd.print(Vout);

    #ifdef DEBUG
        Serial.print("[ INFO ] ");
        Serial.print("K: ");
        Serial.print(K);
        Serial.print(" | Vout: ");
        Serial.println(Vout);
    #endif
}

// ================================================
//              Обработчики прерываний
// ================================================
void freqEncInterupt()
{
    freqEnc.check();
}
