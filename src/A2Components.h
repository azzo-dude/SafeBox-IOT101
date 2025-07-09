class Led {
  private:
    uint8_t _pin;

  public:
    Led(uint8_t pin) : _pin(pin) {
      pinMode(_pin, OUTPUT);
      digitalWrite(_pin, LOW);
    }

    uint8_t getPin() const {
      return _pin;
    }

    void setPin(uint8_t pin) {
      _pin = pin;
      pinMode(_pin, OUTPUT);
      digitalWrite(_pin, LOW);
    }

    void on() {
      digitalWrite(_pin, HIGH);
    }

    void off() {
      digitalWrite(_pin, LOW);
    }

    void blink(uint16_t ms) {
      static unsigned long previousMillis = 0;
      static bool ledState = false;

      unsigned long currentMillis = millis();
      if (currentMillis - previousMillis >= ms) {
        ledState = !ledState;
        if (ledState) {
          on();
        } else {
          off();
        }
        previousMillis = currentMillis;
      }
    }
};

class Buzzer {
  private:
    uint8_t _pin;

  public:
    Led(uint8_t pin) : _pin(pin) {
      pinMode(_pin, OUTPUT);
      digitalWrite(_pin, LOW);
    }

    uint8_t getPin() const {
      return _pin;
    }

    void setPin(uint8_t pin) {
      _pin = pin;
      pinMode(_pin, OUTPUT);
      digitalWrite(_pin, LOW);
    }

    void tone(int16_t hz) {
      tone(_pin, hz);
    }

    void tone(int16_t hz, int64_t dur) {
      tone(_pin, hz, dur);
    }

    void noTone() {
      noTone(_pin);
    }
};


#endif
