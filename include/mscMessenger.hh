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
  mscMessenger(std::vector<double> *);
  ~mscMessenger();
  
  void SetPriGen ( mscPrimaryGeneratorAction *pg ){ fPriGen  = pg; }
  void SetDetCon ( mscDetectorConstruction *dc )  { fDetCon  = dc; }
  void SetEvAct  ( mscEventAction *ev )           { fEvAct   = ev; }
  void SetStepAct( mscSteppingAction *st )        { fStepAct = st; }
  
  void SetNewValue(G4UIcommand* cmd, G4String newValue);
  
private:
  std::vector< double >* asymInfo;
  mscDetectorConstruction *fDetCon;
  mscEventAction *fEvAct;
  mscPrimaryGeneratorAction *fPriGen;
  mscSteppingAction *fStepAct;

  G4UIcmdWithAnInteger *nrUnitsCmd;
  G4UIcmdWithADoubleAndUnit *radThickCmd;
  G4UIcmdWithAString *polCmd;
  G4UIcmdWithAnInteger *writeTreeCmd;
  G4UIcmdWithAnInteger *writeANCmd;
  G4UIcmdWithAnInteger *initOutCmd;
  G4UIcmdWithADoubleAndUnit *stepSizeG4Cmd;
  G4UIcmdWithADoubleAndUnit *beamEnergyCmd;
  G4UIcmdWithAnInteger *physProcCmd;
  
};

#endif//mscMessenger_HH























