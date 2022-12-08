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
    String name;
    Button btn1;
    Button btn2;
    SliderWidget slider;
} Slider;


Slider SLIDERS[]{
    {0, 40, MOTORS_MIN_RPM, MOTORS_MAX_RPM, motorTopRpm, 100, "Top M RPM:", {}, {}, sw},
    {0, 100, MOTORS_MIN_RPM, MOTORS_MAX_RPM, motorBotRpm, 100, "Bottom M RPM:", {}, {}, sw},
};


uint16_t t_x = 9999, t_y = 9999;  // To store the touch coordinates.


// Function declarations are needed here because of the arduino ide auto-prototype generation.
void makeSlider(Slider& slider);
void drawSliderValue(Slider& slider);
void setSliderValue(Slider& slider, int newValue);
void drawButton(Button& btn);
bool buttonClicked(Button& btn, uint16_t x, uint16_t y);


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
    // TODO: drawMenuBar();

    drawControlsView();
}


/// @brief Draw the controls needed to control the machine manually.
void drawControlsView() {
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
    tft.setTextPadding(44);
    tft.drawNumber(slider.value, slider.x + 160 - 5, slider.y + 1);
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


    slider.slider.setSliderPosition(slider.value);
}


/// @brief Check if a button is clicked or not.
/// @param btn The button to check.
/// @param x X location of the click.
/// @param y Y location of the click.
/// @return A boolean whether the button is pressed or not.
bool buttonClicked(Button& btn, uint16_t x, uint16_t y) {
    if ((x > btn.x) && (x < (btn.x + btn.w))) {
        if ((y > btn.y) && (y <= (btn.y + btn.h))) {
            return true;
        }
    }

    return false;
}


/// @brief Set a new slider value.
/// @param slider The slider to change value of.
/// @param newValue The new slider value.
void setSliderValue(Slider& slider, int newValue) {
    if (newValue > slider.maxValue || newValue < 0) {
        return;
    }

    slider.value = newValue;
    slider.slider.setSliderPosition(newValue);
    drawSliderValue(slider);
}


/// @brief Handle all TFT screen touches that the user makes.
void handleTFTScreenTouch() {
    static uint32_t scanTime = millis();

    if (millis() - scanTime >= 20) {
        if (tft.getTouch(&t_x, &t_y, 250)) {
            int numOfSliders = sizeof(SLIDERS) / sizeof(SLIDERS[0]);

            for (int i = 0; i < numOfSliders; i++) {
                int sliderPos = SLIDERS[i].slider.getSliderPosition();

                // Remove remainder to make slider value move in exact increments.
                int roundedValue = sliderPos - (sliderPos % SLIDERS[i].step);

                if (SLIDERS[i].slider.checkTouch(t_x, t_y)) {
                    SLIDERS[i].value = roundedValue;
                    drawSliderValue(SLIDERS[i]);
                }

                // Handle slider increment and decrement buttons.
                if (buttonClicked(SLIDERS[i].btn1, t_x, t_y)) {
                    setSliderValue(SLIDERS[i], roundedValue + SLIDERS[i].step);
                }
                if (buttonClicked(SLIDERS[i].btn2, t_x, t_y)) {
                    setSliderValue(SLIDERS[i], roundedValue - SLIDERS[i].step);
                }
            }
        }
        scanTime = millis();
    }
}