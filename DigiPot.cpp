#include "DigiPot.h"

DigiPot::DigiPot(uint8_t SS, uint8_t address = 0, uint8_t minPos = 0, uint8_t maxPos = 255)
{
    _SS = SS;

    _address = address;
    
    _minPos = minPos;
    _maxPos = maxPos;

    _pos = (_minPos + _maxPos) / 2;

    pinMode (_SS, OUTPUT);
    digitalWrite(_SS, HIGH);
}

void DigiPot::write(uint8_t pos)
{
    if (pos > _maxPos)
    {
        pos = _maxPos;
    }

    if (pos < _minPos)
    {
        pos = _minPos;
    }

    _pos = pos;

    SPI.beginTransaction(SPISettings(SPI_CLOCK_DIV2, MSBFIRST, SPI_MODE0));

    digitalWrite(_SS, LOW);

    SPI.transfer(_address);
    SPI.transfer(_pos);

    digitalWrite(_SS, HIGH);

    SPI.endTransaction();
}

void DigiPot::incPos()
{
    if (_pos >= _maxPos) return;

    write(_pos + 1);
}

void DigiPot::decPos()
{
    if (_pos <= _minPos) return;

    write(_pos - 1);
}