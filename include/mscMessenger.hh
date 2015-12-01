#ifndef mscMessenger_HH
#define mscMessenger_HH

#include "G4UImessenger.hh"
#include "G4UIcommand.hh"
#include "G4VModularPhysicsList.hh"

class mscDetectorConstruction;
class mscEventAction;
class mscPrimaryGeneratorAction;
class mscSteppingAction;

class G4UIcmdWithAnInteger;
class G4UIcmdWithADoubleAndUnit;
class G4UIcmdWithAString;
class G4UIcmdWithABool;

class mscMessenger : public G4UImessenger {
public:
  mscMessenger();
  ~mscMessenger();
  
  void SetPriGen ( mscPrimaryGeneratorAction *pg ){ fprigen  = pg; }
  void SetDetCon ( mscDetectorConstruction *dc )  { fdetcon  = dc; }
  void SetEvAct  ( mscEventAction *ev )           { fevact   = ev; }
  void SetStepAct( mscSteppingAction *st )        { fStepAct = st; }
  
  void SetNewValue(G4UIcommand* cmd, G4String newValue);
  
private:
  mscDetectorConstruction *fdetcon;
  mscEventAction *fevact;
  mscPrimaryGeneratorAction *fprigen;
  mscSteppingAction *fStepAct;

  G4UIcmdWithAnInteger *nrUnitsCmd;
  G4UIcmdWithADoubleAndUnit *radThickCmd;
  G4UIcmdWithAString *polCmd;
  
  // G4UIcmdWithAnInteger *seedCmd;
  // G4UIcmdWithABool     *kryptCmd;
  // G4UIcmdWithABool     *opticalCmd;
  
  // G4UIcmdWithAString   *detfilesCmd;
  
  // G4UIcmdWithAString   *newfieldCmd;
  // G4UIcmdWithAString   *fieldScaleCmd;
  // G4UIcmdWithAString   *fieldCurrCmd;
  // G4UIcmdWithAString   *genSelectCmd;
  

  // G4UIcmdWithADoubleAndUnit *tgtPosCmd;
  
  // G4UIcmdWithADoubleAndUnit *beamCurrCmd;
  // G4UIcmdWithADoubleAndUnit *beamECmd;
  
  // G4UIcmdWithABool       *rasTypeCmd;
  
  // G4UIcmdWithADoubleAndUnit *rasXCmd;
  // G4UIcmdWithADoubleAndUnit *rasYCmd;
  
  // G4UIcmdWithADoubleAndUnit *beamX0Cmd;
  // G4UIcmdWithADoubleAndUnit *beamY0Cmd;
  
  // G4UIcmdWithADoubleAndUnit *beamth0Cmd;
  // G4UIcmdWithADoubleAndUnit *beamph0Cmd;
  
  // G4UIcmdWithADoubleAndUnit *beamCorrThCmd;
  // G4UIcmdWithADoubleAndUnit *beamCorrPhCmd;
  
  // G4UIcmdWithADoubleAndUnit *beamdthCmd;
  // G4UIcmdWithADoubleAndUnit *beamdphCmd;
  
  // G4UIcmdWithAString   *fileCmd;
  
  // ////////////////////////////////////////////////
  // // To general event generators
  // G4UIcmdWithADoubleAndUnit *thminCmd;
  // G4UIcmdWithADoubleAndUnit *thmaxCmd;
  // G4UIcmdWithADoubleAndUnit *thCoMminCmd;
  // G4UIcmdWithADoubleAndUnit *thCoMmaxCmd;
  // G4UIcmdWithADoubleAndUnit *EminCmd;
  // G4UIcmdWithADoubleAndUnit *EmaxCmd;
  
};

#endif//mscMessenger_HH























