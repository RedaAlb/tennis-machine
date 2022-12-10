#include <TFT_eSPI.h>
#include <TFT_eWidget.h>
#include "FS.h"
#include "Free_Fonts.h"


TFT_eSPI tft = TFT_eSPI();
TFT_eSprite knob = TFT_eSprite(&tft);
SliderWidget sw = SliderWidget(&tft, &knob);


typedef struct {
    int x, y, w, h;
    String text;
    int textC, bgC;
} Button;

typedef struct {
    int x, y, minValue, maxValue, & value, step;
    bool useMidpoint;  // To make slider middle value 0, with either side +-maxValue.
    String name;
    Button btn1;
    Button btn2;
    SliderWidget slider;
} Slider;


Slider SLIDERS[]{
    {0, 45, MOTORS_MIN_RPM, MOTORS_MAX_RPM, motorTopRpm, 100, true, "Top M RPM:", {}, {}, sw},
    {0, 105, MOTORS_MIN_RPM, MOTORS_MAX_RPM, motorBotRpm, 100, true, "Bottom M RPM:", {}, {}, sw},
};


// Button to stop all motors.
Button stopMotorsBtn = { 199, 0, 40, 39, "S", TFT_WHITE, TFT_RED };


uint16_t tX = 9999, tY = 9999;  // To store the touch coordinates.


// Function declarations are needed here because of the arduino ide auto-prototype generation.
void makeSlider(Slider& slider);
void drawSliderValue(Slider& slider);
void setSliderValue(Slider& slider, int newValue);
void drawButton(Button& btn);
bool buttonClicked(Button& btn);


/// @brief Setup TFT screen and draw initial ui.
void initTFTScreen() {
    tft.begin();
    tft.setRotation(2);

    // Obtain numbers below by calibrating your tft screen, see TFT_eSPI library examples.
    uint16_t calData[5] = { 324, 3415, 445, 3464, 2 };
    tft.setTouch(calData);

    tft.fillScreen(TFT_BLACK);
    tft.setFreeFont(FF17);

    drawUi();
}


/// @brief Draw the static UI on the TFT screen.
void drawUi() {
    drawControlsView();
}


/// @brief Draw the controls needed to control the machine manually.
void drawControlsView() {
    drawButton(stopMotorsBtn);

    int numOfSliders = sizeof(SLIDERS) / sizeof(SLIDERS[0]);

    for (int i = 0; i < numOfSliders; i++) {
        makeSlider(SLIDERS[i]);
    }
}


/// @brief Draw the slider value based on slider location.
/// @param slider The slider to display the value of.
void drawSliderValue(Slider& slider) {
    tft.setTextColor(TFT_WHITE, TFT_BLUE);
    tft.setTextSize(1);
    tft.setTextPadding(60);
    tft.drawNumber(slider.value, slider.x + 139, slider.y + 1);
    tft.setTextPadding(0);
}


/// @brief Draw a button.
/// @param btn The button struct to draw.
void drawButton(Button& btn) {
    tft.fillRect(btn.x, btn.y, btn.w, btn.h, btn.bgC);
    tft.setTextColor(btn.textC);
    tft.setTextSize(2);
    tft.setTextDatum(MC_DATUM);
    tft.drawString(btn.text, btn.x + (btn.w / 2), btn.y + (btn.h / 2), 2);
    tft.setTextDatum(TL_DATUM);
}


/// @brief Make a slider and draw it.
/// @param slider The slider struct to draw.
void makeSlider(Slider& slider) {
    // Slider text
    tft.setTextColor(TFT_GREEN, TFT_BLACK);
    tft.setTextSize(1);
    tft.drawString(slider.name, slider.x + 1, slider.y + 1);

    drawSliderValue(slider);


    // Slider parameters
    slider_t param;

    param.slotWidth = 10;
    param.slotLength = 196;
    param.slotColor = TFT_BLUE;
    param.slotBgColor = TFT_BLACK;
    param.orientation = H_SLIDER;

    param.knobWidth = 10;
    param.knobHeight = 25;
    param.knobRadius = 0;
    param.knobColor = TFT_WHITE;
    param.knobLineColor = TFT_WHITE;

    param.sliderDelay = 0;

    param.sliderLT = slider.minValue;
    param.sliderRB = slider.maxValue;
    param.startPosition = 0;

    slider.slider.drawSlider(slider.x, slider.y + 20, param);


    // For slider bounding box.
    int16_t bb_x, bb_y;
    uint16_t bb_w, bb_h;
    slider.slider.getBoundingRect(&bb_x, &bb_y, &bb_w, &bb_h);
    tft.drawRect(slider.x, slider.y, bb_w, bb_h + param.knobHeight, TFT_DARKGREY);


    // Slider increment buttons.
    int btns_w = 40;
    int btns_h = 27;
    Button btn1 = { param.slotLength + 4, slider.y, btns_w, btns_h, "+", TFT_WHITE, TFT_BLUE };
    Button btn2 = { param.slotLength + 4, slider.y + btns_h, btns_w, btns_h, "-", TFT_WHITE, TFT_BLUE };

    drawButton(btn1);
    drawButton(btn2);

    slider.btn1 = btn1;
    slider.btn2 = btn2;

    setSliderValue(slider, slider.value);
}


/// @brief Check if a button is clicked or not.
/// @param btn The button to check.
/// @return A boolean whether the button is pressed or not.
bool buttonClicked(Button& btn) {
    if ((tX > btn.x) && (tX < (btn.x + btn.w))) {
        if ((tY > btn.y) && (tY <= (btn.y + btn.h))) {
            return true;
        }
    }

    return false;
}


/// @brief Set a new slider value.
/// @param slider The slider to change value of.
/// @param newValue The new slider value.
void setSliderValue(Slider& slider, int newValue) {
    if (newValue > slider.maxValue ||
        (!slider.useMidpoint && newValue < slider.minValue) ||
        (slider.useMidpoint && newValue < -slider.maxValue)) {
        return;
    }

    slider.value = newValue;
    int sliderPos = newValue;

    if (slider.useMidpoint) {
        // Convert the slider value (-maxValue to maxValue) to a slider position.
        sliderPos = (newValue / 2) + (slider.maxValue / 2);
    }

    slider.slider.setSliderPosition(sliderPos);

    drawSliderValue(slider);
}


/// @brief Handle all TFT screen touches that the user makes.
void handleTFTScreenTouch() {
    static uint32_t scanTime = millis();

    if (millis() - scanTime >= 20) {
        if (tft.getTouch(&tX, &tY, 250)) {
            int numOfSliders = sizeof(SLIDERS) / sizeof(SLIDERS[0]);

            if (buttonClicked(stopMotorsBtn)) {
                for (int i = 0; i < numOfSliders; i++) {
                    setSliderValue(SLIDERS[i], 0);
                }

                return;
            }


            for (int i = 0; i < numOfSliders; i++) {
                int sliderValue = SLIDERS[i].slider.getSliderPosition();

                if (SLIDERS[i].useMidpoint) {
                    // Convert the slider position to a value between -slider.maxValue to +slider.maxValue.
                    sliderValue = (SLIDERS[i].slider.getSliderPosition() - (SLIDERS[i].maxValue / 2)) * 2;
                }

                // Remove remainder to make slider value move in exact steps.
                int roundedValue = sliderValue - (sliderValue % SLIDERS[i].step);

                if (SLIDERS[i].slider.checkTouch(tX, tY)) {
                    SLIDERS[i].value = roundedValue;
                    drawSliderValue(SLIDERS[i]);
                }

                // Handle slider increment and decrement buttons.
                if (buttonClicked(SLIDERS[i].btn1)) {
                    setSliderValue(SLIDERS[i], SLIDERS[i].value + SLIDERS[i].step);
                }
                if (buttonClicked(SLIDERS[i].btn2)) {
                    setSliderValue(SLIDERS[i], SLIDERS[i].value - SLIDERS[i].step);
                }
            }
        }
        scanTime = millis();
    }
}