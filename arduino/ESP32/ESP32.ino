// Current launcher motors rpm, top and bottom.
int motorTopRpm = 0;
int motorBotRpm = 0;


void setup() {
    Serial.begin(115200);

    initLauncherMotors();
    initTFTScreen();
}


void loop() {
    handleTFTScreenTouch();

    setTopMotorSpeed(motorTopRpm);
    setBotMotorSpeed(motorBotRpm);
}