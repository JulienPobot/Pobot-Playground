#ifndef PARAMPROCESS_H
#define PARAMPROCESS_H

class ParamProcess {
    private:
        double lowerHueValue;
        double upperHueValue;
        double lowerSaturationValue;
        double upperSaturationValue;
        double lowerBrightnessValue;
        double upperBrightnessValue;

    public:
        ParamProcess();
        ParamProcess(double, double, double, double, double, double);
        ~ParamProcess();
        void initValues();
        void updateValues(double, double, double, double, double, double);

        double getLowerHueValue();
        double getUpperHueValue();
        double getLowerSaturationValue();
        double getUpperSaturationValue();
        double getLowerBrightnessValue();
        double getUpperBrightnessValue();

        void setLowerHueValue(double);
        void setUpperHueValue(double);
        void setLowerSaturationValue(double);
        void setUpperSaturationValue(double);
        void setLowerBrightnessValue(double);
        void setUpperBrightnessValue(double);
};

#endif // PARAMPROCESS_H
