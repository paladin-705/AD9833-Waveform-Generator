#include "FrequencyEncoder.h"

FrequencyEncoder::FrequencyEncoder(uint8_t pinButton, bool inverted = false, 
    unsigned int pause = 50) : Encoder (_pinA, _pinB, pinButton, _minFreq, _maxFreq, inverted, pause)
{
    // Инициализация внутренних параметров
    _state = 0;

    _speed = 0;
    _lastPosTime = 0;

    _freqMultiplier = 10;                       // По умолчанию выбираются Гц
    _speedMultiplier = 1;

     // Инициализация начального положения энкодера - 1 Гц
     setPos(10);

}

bool FrequencyEncoder::setPos(int32_t pos)
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

void FrequencyEncoder::check()
{
    // Защита от дребезга
    if (micros() - _lastTurn < _pause) return;
    
    
    bool pinAVal = digitalRead(_pinA);
    bool pinBVal = digitalRead(_pinB);
    
    // Запрещаем обработку прерываний
    cli();

    /* PIN A */

    // Если выполняется условие, инкрементируем переменную _state
    if (_state == 0  && !pinAVal &&  pinBVal || _state == 2  && pinAVal && !pinBVal) 
    {
        _state += 1; 
        _lastTurn = micros();
    }

    // Если выполняется условие, декрементируем переменную _state
    if (_state == -1 && !pinAVal && !pinBVal || _state == -3 && pinAVal &&  pinBVal)
    {
        _state -= 1; 
        _lastTurn = micros();
    }

    /* PIN B */

    // Если выполняется условие, инкрементируем переменную _state
    if (_state == 1 && !pinAVal && !pinBVal || _state == 3 && pinAVal && pinBVal) 
    {
        _state += 1; 
        _lastTurn = micros();
    }

    // Если выполняется условие, декрементируем переменную _state
    if (_state == 0 && pinAVal && !pinBVal || _state == -2 && !pinAVal && pinBVal) 
    {
        _state -= 1; 
        _lastTurn = micros();
    }

    // Проверяем не было ли полного шага из 4 изменений сигналов (2 импульсов)
    // Если переменная state приняла заданное значение приращения
    if (_state == 4 || _state == -4) 
    {
        int64_t newPos;

        uint32_t K = _freqMultiplier * _speedMultiplier;

        newPos = !_inverted ? getPos() + K*(int)(_state / 4) : getPos() - K*(int)(_state / 4);

        if (setPos(newPos))
        {
            _speed = millis() - _lastPosTime;
            _lastPosTime = millis();
        }
    }
    
    // Если что-то пошло не так, возвращаем статус в исходное состояние
    if (pinAVal && pinBVal && _state != 0) _state = 0;

    // Разрешаем обработку прерываний
    sei();
}

void FrequencyEncoder::checkButton()
{
    button.clickHandler();

    // При коротком одиночном нажатии меняется множитель частоты
    if(button.is_clicked)
    {
        _freqMultiplier = (10 * _freqMultiplier) % 10000000;
        _freqMultiplier = !_freqMultiplier ? 1 : _freqMultiplier;

        button.resetButton();
    }

    // При долгом нажатии сбрасываются (устанавливаются в ноль) разряды меньше частотного множителя
    if(button.is_long_clicked)
    {
        int32_t newPos = getPos() - getPos() % _freqMultiplier;
        setPos(newPos);

        button.resetButton();
    }
}
