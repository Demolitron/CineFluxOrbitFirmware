#include <math.h>
#include "main.h"
#include "settings.h"
#include "MotionEngine.h"

double MinTimeToMove(double _Distance) {
    double D;
    double HalfD;        

    if (_Distance < 0) D = -_Distance;
    else D = _Distance;
    HalfD = D * 0.5;

    double AccelTime = Config.MaxSpeed / Config.SystemAcceleration;
    double AccelDist = ((Config.SystemAcceleration * AccelTime) * AccelTime)*0.5;
    if (AccelDist > HalfD) {
        AccelTime = sqrt(D / Config.SystemAcceleration);
        AccelDist = ((Config.SystemAcceleration * AccelTime) * AccelTime)*0.5;
    }
    double RealSpeed = AccelTime * Config.SystemAcceleration;
    double CoastDistance = D - (2 * AccelDist);
    double CoastTime = CoastDistance / RealSpeed;
    double TotalTime = AccelTime * 2;
    TotalTime += CoastTime;
    return TotalTime;
}

double SpeedRequiredToMoveInTime(double _Distance, double _Time) {
    double D;   
    double T;  

    if (_Distance < 0) D = _Distance * -0.5;
    else D = _Distance * 0.5;

    if (_Time < 0) T = _Time * -0.5;
    else T = _Time * 0.5;

    if (T > 4096) return D / T;

    double RootPrime = (T * T)-(2 * D / Config.SystemAcceleration);
    if (RootPrime < 0) return -1;

    double AccelTime = T - sqrt(RootPrime);
    double Speed = AccelTime*Config.SystemAcceleration;
    if (Speed > Config.MaxSpeed) return -2;
    return Speed;
}

double GetCurrentLinearPosition() {
    double MotorPos;

    bLock_Motor_Position = 1;
    MotorPos = Motor_Position;
    bLock_Motor_Position = 0;

    MotorPos *= Config.Degrees_Per_Count;
    return MotorPos;
}

double GetCurrentAngle() {
    double MotorPos;

    bLock_Motor_Position = 1;
    MotorPos = Motor_Position;
    bLock_Motor_Position = 0;

    MotorPos *= Config.Degrees_Per_Count;

    if (MotorPos > 0) {
        while (MotorPos > 360) MotorPos -= 360;
    } else {
        while (MotorPos<-360) MotorPos += 360;
        MotorPos += 360;
    }
    return MotorPos;
}

char MoveToLinear(double DestinationPostion, double Speed_Deg_Sec) {
    double CurrentPosition = GetCurrentLinearPosition();
    double Distance = DestinationPostion - CurrentPosition;
    return Move(Distance, Speed_Deg_Sec);
}

char MoveToAngle(double _DestinationAngle, double _Speed_Deg_Sec) {
    double DestinationAngle;
    double Speed_Deg_Sec;
    
    if (_Speed_Deg_Sec < 0) Speed_Deg_Sec = -_Speed_Deg_Sec;
    else Speed_Deg_Sec = _Speed_Deg_Sec;

    if (_DestinationAngle < 0) {
        DestinationAngle = _DestinationAngle;
        while (DestinationAngle<-360) DestinationAngle += 360;
        DestinationAngle += 360;
    } else {
        DestinationAngle = _DestinationAngle;
        while (DestinationAngle > 360) DestinationAngle -= 360;
    }


    double CurrentAngle;
    CurrentAngle = GetCurrentAngle();

    double CW_Distance;
    double CCW_Distance;

    if (DestinationAngle > CurrentAngle) {
        CW_Distance = DestinationAngle - CurrentAngle;
        CCW_Distance = CurrentAngle + (360 - DestinationAngle);
    } else {
        CW_Distance = (360 - CurrentAngle) + DestinationAngle;
        CCW_Distance = CurrentAngle - DestinationAngle;
    }

    if (CW_Distance < CCW_Distance)
        return Move(CW_Distance, Speed_Deg_Sec);
    else
        return Move(-CCW_Distance, Speed_Deg_Sec);
}

char Move(double Distance_deg, double _Speed_Deg_Sec){
    return MoveAdvanced(Distance_deg, _Speed_Deg_Sec, Config.SystemAcceleration);
}

char MoveAdvanced(double Distance_deg, double _Speed_Deg_Sec, double _Acceleration_Deg_Sec_Sec) {
    if (bMove_InProgress) return -1;

    signed long CurrentPosition_counts;
    double Speed_Deg_Sec;
    double Accel_Deg_Sec_Sec;
        
    if (_Speed_Deg_Sec < 0) Speed_Deg_Sec = -_Speed_Deg_Sec;
    else Speed_Deg_Sec = _Speed_Deg_Sec;

    if (_Acceleration_Deg_Sec_Sec < 0) Accel_Deg_Sec_Sec = -_Acceleration_Deg_Sec_Sec;
    else Accel_Deg_Sec_Sec = _Acceleration_Deg_Sec_Sec;

    signed long Distance_counts = (signed long) (Distance_deg * Config.Counts_Per_Degree);
    if (Distance_counts == 0) return 0;

    bLock_Motor_Position = 1;
    CurrentPosition_counts = Motor_Position;
    bLock_Motor_Position = 0;

    Move_FinalPosition = CurrentPosition_counts + Distance_counts;
    Move_Origin = CurrentPosition_counts;

    double DD;
    if (Distance_deg < 0) {
        DD = -Distance_deg;
        bMove_Neg = 1;
    } else {
        DD = Distance_deg;
        bMove_Neg = 0;
    }

    double AccelTime = Speed_Deg_Sec / Accel_Deg_Sec_Sec;
    double AccelDist = (0.5 * Accel_Deg_Sec_Sec * AccelTime * AccelTime);

    if ((AccelDist * 2) > DD) {
        AccelDist = DD / 2;
    }

    double DP = (DD - AccelDist) * Config.Counts_Per_Degree;
    Move_DecelPosition = (unsigned long) DP;

    double AV = Accel_Deg_Sec_Sec * Config.Counts_Per_Degree * Conversion_Q24_500Hz_500Hz;
    Move_AccelValueQ24 = (unsigned long) AV;

    double CS = Speed_Deg_Sec * Config.Counts_Per_Degree * Conversion_Q24_500Hz;
    Move_CoastSpeedQ24 = (unsigned long) CS;

    Move_shifted_position.ul = 0;
    Move_speedQ24 = 0;
    Move_position[0].ul = 0;
    Move_position[1].ul = 0;

    bMove_InProgress = 1;
    return 0;
}