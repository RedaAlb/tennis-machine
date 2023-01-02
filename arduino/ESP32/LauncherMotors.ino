// Launcher motors, top and bottom.
const int MOTOR_T_PWM1_PIN = 32;  // T = top, forward direction.
const int MOTOR_T_PWM2_PIN = 33;  // Backward direction.

const int MOTOR_B_PWM1_PIN = 25;  // B = bottom
const int MOTOR_B_PWM2_PIN = 26;


const int MOTORS_MIN_RPM = 0;
const int MOTORS_MAX_RPM = 7000;


const int MOTOR_T_PWM1_CHANNEL = 1;
const int MOTOR_T_PWM2_CHANNEL = 2;
const int MOTOR_B_PWM1_CHANNEL = 3;
const int MOTOR_B_PWM2_CHANNEL = 4;
const int MOTORS_PWM_FREQ = 2000;
const int MOTORS_PWM_BITS = 12;


// Integers to represent top and bottom launcher motors.
const int LAUNCHER_TOP_MOTOR = 0;
const int LAUNCHER_BOT_MOTOR = 1;


// Current launcher motors rpm, top and bottom.
int motorTopRpm = 0;
int motorBotRpm = 0;


/// @brief Initialise motors pulse-width modulation (PWM) signals.
void initLauncherMotors() {
    // Top motor
    ledcSetup(MOTOR_T_PWM1_CHANNEL, MOTORS_PWM_FREQ, MOTORS_PWM_BITS);
    ledcAttachPin(MOTOR_T_PWM1_PIN, MOTOR_T_PWM1_CHANNEL);
    ledcSetup(MOTOR_T_PWM2_CHANNEL, MOTORS_PWM_FREQ, MOTORS_PWM_BITS);
    ledcAttachPin(MOTOR_T_PWM2_PIN, MOTOR_T_PWM2_CHANNEL);

    // Bottom motor
    ledcSetup(MOTOR_B_PWM1_CHANNEL, MOTORS_PWM_FREQ, MOTORS_PWM_BITS);
    ledcAttachPin(MOTOR_B_PWM1_PIN, MOTOR_B_PWM1_CHANNEL);
    ledcSetup(MOTOR_B_PWM2_CHANNEL, MOTORS_PWM_FREQ, MOTORS_PWM_BITS);
    ledcAttachPin(MOTOR_B_PWM2_PIN, MOTOR_B_PWM2_CHANNEL);
}


/// @brief Handle the top and bottom launcher dc motors.
void handleLauncherMotors() {
    setLauncherMotorSpeed(LAUNCHER_TOP_MOTOR, motorTopRpm);
    setLauncherMotorSpeed(LAUNCHER_BOT_MOTOR, motorBotRpm);
}


/// @brief Set the speed and spin direction of launcher motor.
/// @param motor The motor to control, 0 = top motor, 1 = bottom motor.
/// @param rpm The speed of the motor, (+rpm = forward, -rpm = backward).
void setLauncherMotorSpeed(int motor, int rpm) {
    int pwmSignal = map(abs(rpm), MOTORS_MIN_RPM, MOTORS_MAX_RPM, 0, pow(2, MOTORS_PWM_BITS));

    int motorPwmChannel1, motorPwmChannel2;

    if (motor == 0) {
        motorPwmChannel1 = MOTOR_T_PWM1_CHANNEL;
        motorPwmChannel2 = MOTOR_T_PWM2_CHANNEL;
    }
    else if (motor == 1) {
        motorPwmChannel1 = MOTOR_B_PWM1_CHANNEL;
        motorPwmChannel2 = MOTOR_B_PWM2_CHANNEL;
    }

    if (rpm > 0) {
        ledcWrite(motorPwmChannel1, pwmSignal);
        ledcWrite(motorPwmChannel2, 0);
    }
    else if (rpm < 0) {
        ledcWrite(motorPwmChannel1, 0);
        ledcWrite(motorPwmChannel2, pwmSignal);
    }
    else {
        ledcWrite(motorPwmChannel1, 0);
        ledcWrite(motorPwmChannel2, 0);
    }
}