// Launcher motors, top and bottom.
const int MOTOR_T_ENA_PIN = 32;  // T = top
const int MOTOR_T_IN1_PIN = 33;  // Directional input 1 of the motor (forward).
const int MOTOR_T_IN2_PIN = 25;  // Directional input 2 of the motor (backward).

const int MOTOR_B_ENA_PIN = 26;  // B = bottom
const int MOTOR_B_IN1_PIN = 27;
const int MOTOR_B_IN2_PIN = 14;


const int MOTORS_MIN_RPM = 0;
const int MOTORS_MAX_RPM = 10000;


const int MOTOR_T_PWM_CHANNEL = 1;
const int MOTOR_B_PWM_CHANNEL = 2;
const int MOTORS_PWM_FREQ = 100;
const int MOTORS_PWM_BITS = 13;


int motorTopRpm = 0;
int motorBotRpm = 0;


void setup() {
    Serial.begin(115200);

    initMotorPins();
    initPwm();
    initTFTScreen();
}


void loop() {
    handleTFTScreenTouch();

    setTopMotorSpeed(motorTopRpm);
    setBotMotorSpeed(motorBotRpm);
}


/// @brief Initialise pulse-width modulation (PWM) signals.
void initPwm() {
    ledcSetup(MOTOR_T_PWM_CHANNEL, MOTORS_PWM_FREQ, MOTORS_PWM_BITS);
    ledcAttachPin(MOTOR_T_ENA_PIN, MOTOR_T_PWM_CHANNEL);

    ledcSetup(MOTOR_B_PWM_CHANNEL, MOTORS_PWM_FREQ, MOTORS_PWM_BITS);
    ledcAttachPin(MOTOR_B_ENA_PIN, MOTOR_B_PWM_CHANNEL);
}


/// @brief Initialise motor directional pins.
void initMotorPins() {
    pinMode(MOTOR_T_IN1_PIN, OUTPUT);
    pinMode(MOTOR_T_IN2_PIN, OUTPUT);

    pinMode(MOTOR_B_IN1_PIN, OUTPUT);
    pinMode(MOTOR_B_IN2_PIN, OUTPUT);
}


/// @brief Set the speed and spin direction of the top motor.
/// @param rpm The speed of the motor, (+rpm = forward, -rpm = backward).
void setTopMotorSpeed(int rpm) {
    if (rpm > 0) {
        digitalWrite(MOTOR_T_IN1_PIN, 1);
        digitalWrite(MOTOR_T_IN2_PIN, 0);
    }
    else if (rpm < 0) {
        digitalWrite(MOTOR_T_IN1_PIN, 0);
        digitalWrite(MOTOR_T_IN2_PIN, 1);
        rpm = abs(rpm);
    }
    else {
        digitalWrite(MOTOR_T_IN1_PIN, 0);
        digitalWrite(MOTOR_T_IN2_PIN, 0);
        rpm = 0;
    }

    int pwmSignal = map(rpm, MOTORS_MIN_RPM, MOTORS_MAX_RPM, 0, pow(2, MOTORS_PWM_BITS));
    ledcWrite(MOTOR_T_PWM_CHANNEL, pwmSignal);
}


/// @brief Set the speed and spin direction of the bottom motor.
/// @param rpm The speed of the motor, (+rpm = forward, -rpm = backward).
void setBotMotorSpeed(int rpm) {
    if (rpm > 0) {
        digitalWrite(MOTOR_B_IN1_PIN, 1);
        digitalWrite(MOTOR_B_IN2_PIN, 0);
    }
    else if (rpm < 0) {
        digitalWrite(MOTOR_B_IN1_PIN, 0);
        digitalWrite(MOTOR_B_IN2_PIN, 1);
        rpm = abs(rpm);
    }
    else {
        digitalWrite(MOTOR_B_IN1_PIN, 0);
        digitalWrite(MOTOR_B_IN2_PIN, 0);
        rpm = 0;
    }

    int pwmSignal = map(rpm, MOTORS_MIN_RPM, MOTORS_MAX_RPM, 0, pow(2, MOTORS_PWM_BITS));
    ledcWrite(MOTOR_B_PWM_CHANNEL, pwmSignal);
}