#ifndef ENCODER_H
#define ENCODER_H

#include <Arduino.h>
#include "Button.h"

class Encoder
{
public:
    Encoder(uint8_t pinA, uint8_t pinB, uint8_t pinButton, 
        int32_t minPos, int32_t maxPos, bool inverted = false, unsigned int pause = 50);
    
    virtual bool setPos(int32_t pos) = 0;
    virtual int32_t getPos() = 0;

    virtual void check() = 0;
    virtual void checkButton() = 0;

private:
    

protected:
    // Переменные для хранения портов к которым подключён энкодер
    uint8_t _pinA;
    uint8_t _pinB;
    uint8_t _pinButton;
    
    // Минимальное и максимальное значение регулируемого параметра
    int32_t _minPos;
    int32_t _maxPos;

    // Текущее значение регулируемого параметра, мкс
    volatile int32_t _pos;

     /*
        Флаг, определяющий какое направление вращения энкодера считать положительным. 
        По умолчанию false (положительное направление вращения по часовой стрелке)
    */
    bool _inverted;

    /*
        Переменные для программного устранения дребезга, путём установки минимального времени между 
        изменениями положения энкодера (_pause). lastTurn - Время последнего изменения состояния выводов энкодера, мкс
    */
    unsigned int _pause;
    volatile uint64_t _lastTurn;

    /*
        Обьект класса Button. Позволяет обрабатывать короткие и длинные клики на кнопку
    */
    Button button;

};
 
#endif