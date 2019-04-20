#ifndef AMPLITUDE_ENCODER_H
#define AMPLITUDE_ENCODER_H

#include <Arduino.h>
#include "Encoder.h"

class AmplitudeEncoder : protected Encoder
{
public:
    AmplitudeEncoder(uint8_t pinA, uint8_t pinB, uint8_t pinButton, 
        bool inverted = false, unsigned int pause = 50);

    bool setPos(int32_t pos);
    inline int32_t getPos() { return _pos; };

    void check();
    void checkButton();

    inline void resetButton() { button.resetButton(); };

    inline bool is_clicked() { return button.is_clicked; };
    inline bool is_long_clicked() { return button.is_long_clicked; };

private:
    bool _lastStateA;

    uint8_t _counter;

    // Защита от дребезга
    const uint32_t _noise_dur_ms = 50;
    uint32_t _prev_time_ms = 0;
};
 
#endif