char Move(double Distance_deg, double Speed_deg_sec);
char MoveAdvanced(double Distance_deg, double _Speed_Deg_Sec, double _Acceleration_Deg_Sec_Sec);
char MoveToLinear(double DestinationPostion, double Speed_Deg_Sec);
char MoveToAngle(double DestinationAngle, double Speed_Deg_Sec);
double SpeedRequiredToMoveInTime(double Distance, double Time);
double MinTimeToMove(double Distance);