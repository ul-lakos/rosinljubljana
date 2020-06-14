#include "Lidar.h"

/**
 * @brief Construct a new Lidar object
 * Initialises the servo and the Sharp sensor.
 */
Lidar::Lidar()
{
    ledcSetup(SERVO_CH, 50, 16); //*< Channel X, 50 Hz, 16-bit depth */
    ledcAttachPin(SERVO, SERVO_CH);
    pinMode(DIST, INPUT_PULLUP);
}

/**
 * @brief Returns the current angle of the Lidar.
 * 
 * @return int Lidar angle.
 */
int Lidar::currentPosition()
{
    return _usToAngle(_dutyToUs(ledcRead(SERVO_CH)));
}

/**
 * @brief Rotates the servo for a perscribed angle.
 * The units of the angle (step) are degrees. 
 * 
 * @param step Angle to rotate, in degrees.
 */
void Lidar::nextPosition(int step)
{
    if (_servoVal >= 180)
        _servoDir = -step;
    else if (_servoVal <= 0)
        _servoDir = +step;
    _servoVal += _servoDir;
    ledcWrite(SERVO_CH, _usToDuty(_angleToUs(_servoVal)));
}

/**
 * @brief Returns the distance at the current servo angle.
 * 
 * @return float Distance in metres.
 */
float Lidar::getDistance()
{
    return _adcToDist(analogRead(DIST));
}

/**
 * @brief Returns the distance, averaged over multiple measurements.
 * 
 * @param measurements Number of measurements.
 * @return float Averaged distance.
 */
float Lidar::getDistanceAverage(int measurements)
{
    float distance = 0.0;
    for (int i = 0; i < measurements; i++)
    {
        distance += getDistance();
    }
    return distance / measurements;
}

/**
 * @brief Transforms an analog value from the Sharp distance sensor to distance in metres.
 * Uses linear interpolation between calibrated points (table in the header).
 * 
 * @param adcVal Analog value.
 * @return float Distance in metres.
 */
float Lidar::_adcToDist(float adcVal)
{
    for (int i = 1; i < (sizeof(_distAdcMap) / sizeof(S_DIST_ADC_MAP)); i++)
    {
        if (adcVal > _distAdcMap[i].adcVal)
        {
            float factor = (adcVal - _distAdcMap[i].adcVal) / (_distAdcMap[i - 1].adcVal - _distAdcMap[i].adcVal);
            _distance = factor * (_distAdcMap[i - 1].distance - _distAdcMap[i].distance) + _distAdcMap[i].distance;
            break;
        }
        else
            _distance = 0.0;
    }
    return _distance;
}