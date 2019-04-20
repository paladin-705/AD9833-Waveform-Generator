#include "Button.h"

Button::Button(uint8_t pin, bool inverted = false, int noise_time_ms = 50, int long_click_time_ms = 1000)
{
    _pin = pin;
    pinMode(_pin, INPUT);
    _inverted = inverted;
	
    is_clicked = false;
    is_long_clicked = false;

    _noise_dur_ms = noise_time_ms;
    _long_click_dur_ms = long_click_time_ms;
}


void Button::clickHandler()
{
    uint32_t time_ms = millis();
    
    bool pin_state;
    !_inverted ? pin_state = digitalRead(_pin) : pin_state = !digitalRead(_pin);

    // Ловим нажатие на кнопку
    if (pin_state && !_clicked_state && time_ms - _prev_time_ms > _noise_dur_ms)
    {
        _clicked_state = true;        // Устанавливаем служебный флаг, что кнопка была нажата
        _clicked_time_ms = time_ms;
    }

    // Ловим отпускание кнопки
    // Если время нажатия кнопки превысило время долгого нажатия, то устанавливаем флаг, долгого нажатия на кнопку
    // Если при этом кнопка была предварительно нажата, то устанавливаем флаг, нажатия на кнопку
    if(!pin_state && _clicked_state && ( time_ms - _clicked_time_ms ) > _long_click_dur_ms)
    {
        is_clicked = false;
        is_long_clicked = true;

        _clicked_state = false;
        _prev_time_ms = time_ms;
    }
    else if (!pin_state && _clicked_state)
    {
        is_clicked = true;
        is_long_clicked = false;

        _clicked_state = false;
        _prev_time_ms = time_ms;
    }
}

// Сброс состояний  
void Button::resetButton()
{
  is_clicked = false;
  is_long_clicked = false;
}