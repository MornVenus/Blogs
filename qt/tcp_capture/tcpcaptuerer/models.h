#ifndef MODELS_H
#define MODELS_H

struct PowerModel
{
    int channel;
    double setV;
    double ocp;
    int waitTime;
    double ovp;
    double uvp;
    int delay;
    double stepVolt;
    double stepDelay;
};

#endif // MODELS_H
