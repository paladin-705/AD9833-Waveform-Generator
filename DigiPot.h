#ifndef DIGIPOT_H
#define DIGIPOT_H

#include <Arduino.h>
#include <SPI.h>

class DigiPot
{
public:
    DigiPot(uint8_t SS, uint8_t address = 0, uint8_t minPos = 0, uint8_t maxPos = 255);

    void write(uint8_t pos);        // Запись значения положения потенциометра

    void incPos();                  // Увеличивает позицию на 1
    void decPos();                  // Уменьшает позицию на 1

    inline uint8_t getPos() { return _pos; };

private:
    uint8_t _SS;                    // Slave Select для SPI

    volatile uint8_t _pos;          // Текущее положение потенциометра
    uint8_t _address;               // Адрес канала (если он многоканальный)

    uint8_t _minPos;
    uint8_t _maxPos;
};
 
#endif