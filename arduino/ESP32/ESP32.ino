void setup() {
    Serial.begin(115200);

    initLauncherMotors();
    initTFTScreen();
}


void loop() {
    handleTFTScreenTouch();
    handleLauncherMotors();
}