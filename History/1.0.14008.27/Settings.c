#include <htc.h>
#include "main.h"
#include "UserInterface.h"
#include "Settings.h"
#include "WaypointProgram.h"
#include "OrbitProgram.h"

void FactoryDefault() {
    Config.Version = CONFIG_SCHEMA_VERSION;
    Config.BatteryLowVoltageLevel = 1470;
    Config.Battery12V_Q8 = 44288;
    Config.PowerResponseLimiter_Slope = 6;
    Config.PowerResponseLimiter_Intercept = 122;
    Config.Counts_per_Volt = 231.782986;
    Config.Volts_per_Count = 0.0043143805193735;

    Config.BacklightIdleTimeout = 5000;
    Config.BacklightPWM_Duty = 1;

    Config.SystemAcceleration = 180.0;
    Config.MaxSpeed = 180.0;

    Config.PID_MaxError = 32;
    Config.PID_Kp = 32;
    Config.PID_Kd = 64;

    Config.Counts_Per_Degree = 17.5;
    Config.Degrees_Per_Count = 0.05714286;

    PRESET Empty;
    Empty.Orbit.Type = 0;
    SavePreset(0, (unsigned char *) &Empty);
    SavePreset(1, (unsigned char *) &Empty);
    SavePreset(2, (unsigned char *) &Empty);
    SavePreset(3, (unsigned char *) &Empty);
    SavePreset(4, (unsigned char *) &Empty);
}

void SaveSettings() {
    unsigned int addr = 0;
    di();
    EEprom_write(&addr, (unsigned char *) &Config, sizeof (Config));
    ei();
}

void LoadSettings() {
    unsigned int addr = 0;
    di();
    EEprom_read(&addr, (unsigned char *) &Config, sizeof (Config));
    ei();

    if (!(Config.Version == CONFIG_SCHEMA_VERSION)) {
        FactoryDefault();
        SaveSettings();
    }
}

void EEprom_write(unsigned int *addr, unsigned char *dat, unsigned char bCount) {
    while (bCount) {
        eeprom_write(*addr, *dat);
        (*addr)++;
        dat++;
        bCount--;
    }
}

void EEprom_read(unsigned int *addr, unsigned char *dat, unsigned char bCount) {
    while (bCount) {
        (*dat) = eeprom_read(*addr);
        (*addr)++;
        dat++;
        bCount--;
    }
}

unsigned char GetPresetType(char pnum) {
    unsigned char ret;
    unsigned int addr;
    addr = sizeof (ConfigStruct)+(pnum * sizeof (PRESET));
    EEprom_read(&addr, &ret, 1);
    return ret;
}

void SavePreset(char pnum, unsigned char *dat) {
    unsigned int addr;
    addr = sizeof (ConfigStruct)+(pnum * sizeof (PRESET));
    EEprom_write(&addr, dat, sizeof (PRESET));
}

void LoadPreset(char pnum, unsigned char *dat) {
    unsigned int addr;
    addr = sizeof (ConfigStruct)+(pnum * sizeof (PRESET));
    EEprom_read(&addr, dat, sizeof (PRESET));
}

