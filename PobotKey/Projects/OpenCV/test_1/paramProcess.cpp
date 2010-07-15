#include "paramProcess.h"



#define MIN_HUE 0
#define MAX_HUE 255
#define MIN_SATURATION 0
#define MAX_SATURATION 255
#define MIN_BRIGHTNESS 0
#define MAX_BRIGHTNESS 255



/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ParamProcess::ParamProcess() {

}



/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ParamProcess::ParamProcess(double lowerHueValue, double upperHueValue, double lowerSaturationValue, double upperSaturationValue, double lowerBrightnessValue, double upperBrightnessValue) {
    this->lowerHueValue = lowerHueValue;
    this->upperHueValue = upperHueValue;
    this->lowerSaturationValue = lowerSaturationValue;
    this->upperSaturationValue = upperSaturationValue;
    this->lowerBrightnessValue = lowerBrightnessValue;
    this->upperBrightnessValue = upperBrightnessValue;
}



/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ParamProcess::~ParamProcess() {

}



/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ParamProcess::initValues() {
    this->lowerHueValue = MIN_HUE;
    this->upperHueValue = MAX_HUE;
    this->lowerSaturationValue = MIN_SATURATION;
    this->upperSaturationValue = MAX_SATURATION;
    this->lowerBrightnessValue = MIN_BRIGHTNESS;
    this->upperBrightnessValue = MAX_BRIGHTNESS;
}



/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ParamProcess::updateValues(double lowerHueValue, double upperHueValue, double lowerSaturationValue, double upperSaturationValue, double lowerBrightnessValue, double upperBrightnessValue) {
    this->lowerHueValue = lowerHueValue;
    this->upperHueValue = upperHueValue;
    this->lowerSaturationValue = lowerSaturationValue;
    this->upperSaturationValue = upperSaturationValue;
    this->lowerBrightnessValue = lowerBrightnessValue;
    this->upperBrightnessValue = upperBrightnessValue;
}



/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
double ParamProcess::getLowerHueValue()        { return this->lowerHueValue; }
double ParamProcess::getUpperHueValue()        { return this->upperHueValue; }
double ParamProcess::getLowerSaturationValue() { return this->lowerSaturationValue; }
double ParamProcess::getUpperSaturationValue() { return this->upperSaturationValue; }
double ParamProcess::getLowerBrightnessValue() { return this->lowerBrightnessValue; }
double ParamProcess::getUpperBrightnessValue() { return this->upperBrightnessValue; }

void ParamProcess::setLowerHueValue(double value)        { this->lowerHueValue = value; }
void ParamProcess::setUpperHueValue(double value)        { this->upperHueValue = value; }
void ParamProcess::setLowerSaturationValue(double value) { this->lowerSaturationValue = value; }
void ParamProcess::setUpperSaturationValue(double value) { this->upperSaturationValue = value; }
void ParamProcess::setLowerBrightnessValue(double value) { this->lowerBrightnessValue = value; }
void ParamProcess::setUpperBrightnessValue(double value) { this->upperBrightnessValue = value; }
