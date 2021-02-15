#ifdef ARDUINO_ESP32_DEV

namespace DisplayLedMatrix {

    #define P_LAT 22
    #define P_A 19
    #define P_B 23
    #define P_C 18
    #define P_D 5
    #define P_E 15
    #define P_OE 16
    hw_timer_t * timer = NULL;
    portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;


    // Pins for LED MATRIX
    #define matrix_width 64
    #define matrix_height 64


    // This defines the 'on' time of the display is us. The larger this number,
    // the brighter the display. If too large the ESP will crash
    uint8_t display_draw_time=50; //10-50 is usually fine


    // PxMATRIX display(matrix_width,matrix_height,P_LAT, P_OE,P_A,P_B,P_C);
    //PxMATRIX display(64,32,P_LAT, P_OE,P_A,P_B,P_C,P_D);
    PxMATRIX display(matrix_width,matrix_height,P_LAT, P_OE,P_A,P_B,P_C,P_D,P_E);
  
    void IRAM_ATTR display_updater() {
        // Increment the counter and set the time of ISR
        portENTER_CRITICAL_ISR(&timerMux);
        
        display.display(display_draw_time);
        portEXIT_CRITICAL_ISR(&timerMux);
    }

    void display_update_enable(bool is_enable)
    {
        if (is_enable)
        {
            timer = timerBegin(0, 40, true);
            timerAttachInterrupt(timer, &display_updater, true);
            timerAlarmWrite(timer, 4000, true);
            timerAlarmEnable(timer);
        }
        else
        {
            timerDetachInterrupt(timer);
            timerAlarmDisable(timer);
        }
    }
    
    void initialize()
    {
        // do nothing
    }

    void setup()
    {
        display.begin(32);  
        display.clearDisplay();
        display_update_enable(true);

        display.setBrightness(20);
    }

    extern Display _display(&display);
}

#endif