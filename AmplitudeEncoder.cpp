#include "AmplitudeEncoder.h"

AmplitudeEncoder::AmplitudeEncoder(uint8_t pinA, uint8_t pinB, uint8_t pinButton, 
    bool inverted = false, unsigned int pause = 50) : Encoder (pinA, pinB, pinButton, 0, 255, inverted, pause)
{
    // Инициализация внутренних параметров
    _lastStateA = digitalRead(_pinA);

    _counter = 0;

     // Инициализация начального положения энкодера
     setPos(_maxPos/2 + 1);
}

bool AmplitudeEncoder::setPos(int32_t pos)
{
    if(pos >= _minPos && pos <= _maxPos)
    {
        _pos = pos;
        return true;
    }
    else
    {
        return false;
    }
}

void AmplitudeEncoder::check()
{
    uint32_t time_ms = millis();

    bool stateA = digitalRead(_pinA);
    bool stateB = digitalRead(_pinB);

    /*
        Если вход A изменил своё состояние, то начинаем проверку,
        если прошло минимальное время между изменениями состояний 
        входа A (т.е. измение не считется дребезгом)

        Этот блок срабатывает 1 раз из двух (для этого и нужна переменная _counter),
        из-за работы без прерываний (иначе бы при одном повороте энкодера, положение 
        менялось бы сразу на два деления) 

    */
    if (stateA != _lastStateA && _counter >= 1 && time_ms - _prev_time_ms > _noise_dur_ms)
    {
        /* 
            Если состояние входа B отличается от состояния входа A, 
            то энкодер вращается по часовой стрелке, иначе, энкодер
            вращается против часовой стрелки 
        */
        int32_t delta = stateB != stateA ? 1 : -1;

        // Расчитываем новое положение энкодера
        int32_t newPos = !_inverted ? getPos() + delta : getPos() - delta;
        
        setPos(newPos);

        // Обновляем значение переменной хранящей последнее состояние входа A 
        _lastStateA = stateA;
    
        // Обновление времени последнего изменения состояния входа A
        _prev_time_ms = time_ms;

        // Сбрасываем счётчик
        _counter = 0;
    }
    else if (stateA != _lastStateA && time_ms - _prev_time_ms > _noise_dur_ms)
    {
        // Увеличиваем значение счётчика
        _counter++;

        // Обновляем значение переменной хранящей последнее состояние входа A 
        _lastStateA = stateA;
    
        // Обновление времени последнего изменения состояния входа A
        _prev_time_ms = time_ms;
    }
}

void AmplitudeEncoder::checkButton()
{
    button.clickHandler();
}