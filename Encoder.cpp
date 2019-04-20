#include "Encoder.h"

Encoder::Encoder(uint8_t pinA, uint8_t pinB, uint8_t pinButton, 
    int32_t minPos, int32_t maxPos, bool inverted = false, unsigned int pause = 50) : button(pinButton, true, 50, 1000)
{
    _pinA = pinA;
    _pinB = pinB;
    _pinButton = pinButton;

    _minPos = minPos;
    _maxPos = maxPos;

    _inverted = inverted;

    _pause = pause;

    pinMode (_pinA, INPUT);
    pinMode (_pinB, INPUT);

    _lastTurn = 0;
}