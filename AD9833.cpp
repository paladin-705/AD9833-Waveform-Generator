#include "AD9833.h"

AD9833::AD9833(uint8_t FSYNC, uint32_t refFreq = 250000000)
{
    _FSYNC = FSYNC;
    _refFreq = refFreq;

    pinMode (_FSYNC, OUTPUT);
    digitalWrite(_FSYNC, HIGH);

    FREQ_K = (double)(F2POW28) / (double)(_refFreq);
}

//  ===============================================
//  ================ Private ======================
//  ===============================================

void AD9833::write(uint16_t data)
{
    SPI.beginTransaction(SPISettings(SPI_CLOCK_DIV2, MSBFIRST, SPI_MODE2));

    digitalWrite(_FSYNC, LOW);

    SPI.transfer16(data);

    digitalWrite(_FSYNC, HIGH);
    SPI.endTransaction();

    // Костыль для схемы компаратора
    SPI.beginTransaction(SPISettings(SPI_CLOCK_DIV128, MSBFIRST, SPI_MODE0));
    SPI.endTransaction();
}

void AD9833::parseFreq(uint32_t freq, uint16_t &lsPart, uint16_t &msPart)
{
    uint32_t mask = 0x3fff;
    
    lsPart = freq & mask;
    msPart = (freq >> 14) & mask;
}

//  ===============================================
//  ================= Public ======================
//  ===============================================

void AD9833::reset()
{
    write(RESET_CMD);
}

void AD9833::sleepMode()
{
    write(SLEEP_MODE);
}

void AD9833::setSignal(SignalType signalType)
{
    _signal = signalType;

    write(_signal);
}

void AD9833::setSignal(SignalType signalType, int32_t freq)
{
    setSignal(signalType);
}

void AD9833::setFrequency(int32_t freq)
{
    _freq = freq;

    uint32_t regVal = (uint32_t)((double)(FREQ_K) * (double)(_freq));

    uint16_t lsPart = 0;                    // Переменная для хранения младших разрядов регистра частоты
    uint16_t msPart = 0;                    // Переменная для хранения старших разрядов регистра частоты

    // Разбиение переменной для хранения значения частоты на две части - каждая по 14 бит.
    parseFreq(regVal, lsPart, msPart);

    lsPart |= FREQ0_WRITE_REG;
    msPart |= FREQ0_WRITE_REG;

    write(FREQ0_FULL_OVERWRITE_REG);        // Выбираем перезапись FREQ0, а также использование FREQ0 и PHASE0
    write(lsPart);
    write(msPart);
}

void  AD9833::setPhase(int16_t phase)
{
    _phase = phase;

    uint16_t phaseVal = (uint16_t)(BITS_PER_DEG * _phase) & 0x0FFF;

    uint16_t phaseReq = phaseVal | PHASE_WRITE_CMD;

    write(phaseReq);
}
