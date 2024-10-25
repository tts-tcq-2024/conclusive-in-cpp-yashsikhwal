#include "typewise-alert.h"
#include <stdio.h>

typedef struct {
    double lowerLimit;
    double upperLimit;
} BreachLimits;

BreachLimits getBreachLimits(CoolingType coolingType) {
    static const BreachLimits limits[] = {
        {0, 35}, // PASSIVE_COOLING
        {0, 45}, // HI_ACTIVE_COOLING
        {0, 40}  // MED_ACTIVE_COOLING
    };

    if (coolingType >= PASSIVE_COOLING && coolingType <= MED_ACTIVE_COOLING) {
        return limits[coolingType];
    }
    return {0, 0}; // Default case
}

BreachType classifyTemperatureBreach(CoolingType coolingType, double temperatureInC) {
    BreachLimits limits = getBreachLimits(coolingType);
    return inferBreach(temperatureInC, limits.lowerLimit, limits.upperLimit);
}

void handleAlert(AlertTarget alertTarget, BreachType breachType) {
    if (alertTarget == TO_CONTROLLER) {
        sendToController(breachType);
    } else if (alertTarget == TO_EMAIL) {
        sendToEmail(breachType);
    }
}

void checkAndAlert(AlertTarget alertTarget, BatteryCharacter batteryChar, double temperatureInC) {
    BreachType breachType = classifyTemperatureBreach(batteryChar.coolingType, temperatureInC);
    handleAlert(alertTarget, breachType);
}

void sendToController(BreachType breachType) {
    const unsigned short header = 0xfeed;
    printf("%x : %x\n", header, breachType);
}

void sendToEmail(BreachType breachType) {
    const char* recipient = "a.b@c.com";
    if (breachType == TOO_LOW) {
        printf("To: %s\nHi, the temperature is too low\n", recipient);
    } else if (breachType == TOO_HIGH) {
        printf("To: %s\nHi, the temperature is too high\n", recipient);
    }
}

BreachType inferBreach(double value, double lowerLimit, double upperLimit) {
    if (value < lowerLimit) {
        return TOO_LOW;
    }
    if (value > upperLimit) {
        return TOO_HIGH;
    }
    return NORMAL;
}
