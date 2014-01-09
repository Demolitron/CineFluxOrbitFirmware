
typedef struct {
    unsigned long Version;
    unsigned int BatteryLowVoltageLevel;
    unsigned int Battery12V_Q8;
    unsigned char PowerResponseLimiter_Slope;
    unsigned char PowerResponseLimiter_Intercept;
    double Counts_per_Volt;
    double Volts_per_Count;
    unsigned char BacklightPWM_Duty;
    unsigned int BacklightIdleTimeout;
    double SystemAcceleration;
    double MaxSpeed;
    signed int PID_MaxError;
    unsigned int PID_Kp;
    unsigned int PID_Kd;
    double Counts_Per_Degree;
    double Degrees_Per_Count;
} ConfigStruct;

typedef struct {
    char Type;
    unsigned int Origin_deg;
    char EndMode;
    char IsClockWise;
    double ProgramRunTime_sec;
    double CycleCount_rev;
    double CycleTime_sec;
    double Speed_deg_sec;
    char SpeedMode;
} OrbitProgramStruct;

typedef struct {
    char Type;
    unsigned int Origin_deg;
    unsigned char PointCount;
    unsigned char Bounce;
    unsigned int LoopCount;
    signed int Distances_deg[18];
    unsigned int TravelTimes_sec[18];
    unsigned int DwellTimes_sec[19];
} WaypointProgramStruct;

typedef union {
    unsigned char ub[120];
    WaypointProgramStruct WayPoint;
    OrbitProgramStruct Orbit;
} PRESET;

ConfigStruct Config;
WaypointProgramStruct CurrentPath;
OrbitProgramStruct CurrentOrbitProgram;

void FactoryDefault();
void SaveSettings();
void LoadSettings();
void EEprom_write(unsigned int *addr, unsigned char *dat, unsigned char bCount);
void EEprom_read(unsigned int *addr, unsigned char *dat, unsigned char bCount);
unsigned char GetPresetType(char pnum);
void SavePreset(char pnum, unsigned char *dat);
void LoadPreset(char pnum, unsigned char *dat);

#define CONFIG_SCHEMA_VERSION 3