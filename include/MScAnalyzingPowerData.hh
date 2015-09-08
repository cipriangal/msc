#ifndef MScAnalyzingPowerData_h
#define MScAnalyzingPowerData_h

struct AnaPower{
  double energy;
  double theta;
  double power;
  bool updated;

  void Init(){
    energy = -999;
    theta  = -999;
    power  = -999;
    updated=false;
  }
};

AnaPower info;

#endif
