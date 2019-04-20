#ifndef FREQUENCY_ENCODER_H
#define FREQUENCY_ENCODER_H

#include <Arduino.h>
#include "Encoder.h"

class FrequencyEncoder : protected Encoder
{
public:
    FrequencyEncoder(uint8_t pinButton, bool inverted = false, unsigned int pause = 50);

    bool setPos(int32_t pos);
    inline int32_t getPos() { return _pos; };

    void check();
    void checkButton();

private:
    // Т.к. энкодер использует прерывания, то его порты нельзя задать пользователю
    static const uint8_t _pinA = 2;
    static const uint8_t _pinB = 3;

    volatile int8_t _state;

    /*
        Переменные для определения скорости вращения энкодера
        _speed - скорость вращения
        _lastPosTime - время предидущей установки позиции, мс
    */
    volatile uint64_t _lastPosTime;
    volatile double _speed;

    /*
        Множитель позволяющий при высокой скорости вращения выбирать частоту с большим шагом.

        Формула расчёта: _speedMultiplier = exp(T/T0)
        где:
            T - измеренное время вращения
            T0 - время нормального (не слишком быстрого, не слишком медленного) вращения. Константа.

    */
    volatile int32_t _speedMultiplier;

    // ---------------------------------------------------------------------------------
    // ---------------------------- РЕГУЛИРОВКА ЧАСТОТЫ --------------------------------
    // ---------------------------------------------------------------------------------
    /*
        Одна еденица, соответсвует 100 мГц
        Максимальная частота принята за 12.5 МГц
        Минимальная частота принята за 0.1 Гц
    */
    static const int32_t _maxFreq = 125000000;
    static const int32_t _minFreq = 1;

    /*
        Множитель позволяющий выбирать частоту с большим удобством и скоростью:
        При множителе равном 10 - выбираются Гц, при 10000 - кГц и т.д.
        Допустимые значения 1 (100 мГц), 10 (1 Гц), 100, 1000, 
                            10 000 (1 кГц), 100 000, 1 000 000, 
                            10 000 000 (1 МГц)
    */
    volatile int32_t _freqMultiplier;
};
 
#endif