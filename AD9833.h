#ifndef AD9833_H
#define AD9833_H

#include <Arduino.h>
#include <SPI.h>

enum SignalType 
{ 
    SINE_WAVE = 0x2000, 
    TRIANGLE_WAVE = 0x2002,
    SQUARE_WAVE = 0x2028, 
    HALF_SQUARE_WAVE = 0x2020 
};

class AD9833
{
public:
    AD9833(uint8_t FSYNC, uint32_t refFreq = 250000000);

    void reset();
    void sleepMode();

    void setSignal(SignalType signalType);
    void setSignal(SignalType signalType, int32_t freq);

    void setFrequency(int32_t freq);
    void setPhase(int16_t phase);

    inline uint32_t getFrequency() { return _freq; };
    inline uint16_t getPhase() { return _phase; };
    inline SignalType getSignal() { return _signal; };

private:
    void write(uint16_t data);

    // Разделяет переменную где хранится исходная частота (28 бит) на две 14 битные
    void parseFreq(uint32_t freq, uint16_t &lsPart, uint16_t &msPart);

    uint8_t _FSYNC;             // Выбор устройства SPI. Перед началом передачи данных должен быть установлен в 0, по завершении в 1.
    uint32_t _refFreq;          // Частота тактирования AD9833. Она не в Герцах! По умолчанию 1 ед. = 100 мГц.

    uint32_t _freq;
    uint16_t _phase;

    SignalType _signal;

    // Константы для регистров (Взято из даташита)
    static const uint16_t RESET_CMD = 0x0100;		            // Команда сброса. Сбрасывает внутренние регистры генератора в 0. Сброс не затрагивает регистры управления, частоты и фазы.
    static const uint16_t SLEEP_MODE = 0x00C0;                  // Запрещается внутреннее тактирование, приостанавливается работа NCO и выход генератора остается в своем текущем состоянии. Отключается внутренний ЦАП.

    static const uint16_t FREQ0_FULL_OVERWRITE_REG = 0x2000;    // Команда полной перезаписи регистра FREQ0 двумя последовательными 16 битными словами
    static const uint16_t FREQ0_WRITE_REG = 0x4000;		        // Команда записи значения в FREQ0

    static const uint16_t PHASE_WRITE_CMD = 0xC000;		        // Команда записи значения в фазовый регистр

    static const uint32_t F2POW28 = 0x10000000;                 // 2 в 28 степени для расчёта частоты
    double FREQ_K;                                              // Коэффициент для преобразования частоты в значение регистра: FREQ_K = F2POW28 / _refFreq

    static const double BITS_PER_DEG = 11.3777777777778;        // Шаг фазового регистра
};
 
#endif