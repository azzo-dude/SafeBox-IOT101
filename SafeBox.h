#ifndef SAFEBOX_H
#define SAFEBOX_H

#include "SafeBoxConfig.h"

class SafeBox {
  private:
    Led ledGreen_(LED_GREEN_PIN);
    Led ledRed_(LED_RED_PIN);
    Buzzer buzzer_(BUZZER_PIN);

  public:
    void init();
};

#endif