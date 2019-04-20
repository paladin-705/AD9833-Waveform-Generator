#ifndef _BUTTON_h
#define _BUTTON_h

#include <Arduino.h>

class Button
{
public:
    Button(uint8_t pin, bool _inverted = false, int noise_time_ms = 50, int long_click_time_ms = 1000);
    void clickHandler();
    void resetButton();

    bool is_clicked;
    bool is_long_clicked;

private:
    uint8_t _pin;
    bool _inverted;

    bool _clicked_state = false;
    uint32_t _clicked_time_ms = 0;
    uint32_t _prev_time_ms = 0;

    int _noise_dur_ms = 50;
    int _long_click_dur_ms = 1000;
};

#endif