#include "mscMessenger.hh"

#include "G4UIcmdWithAnInteger.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"
#include "G4UIcmdWithABool.hh"

#include "mscDetectorConstruction.hh"
#include "mscEventAction.hh"
#include "mscPrimaryGeneratorAction.hh"
#include "mscSteppingAction.hh"

#include "G4UImanager.hh"
#include "G4RunManager.hh"

#include <iostream>

mscMessenger::mscMessenger(){
    /*  Initialize all the things it talks to to NULL */

    fDetCon       = NULL;
    fEvAct        = NULL;
    fPriGen       = NULL;
    fStepAct      = NULL;

    nrUnitsCmd = new G4UIcmdWithAnInteger("/msc/setNrUnits",this);
    nrUnitsCmd->SetGuidance("Set number of units for segmentation");
    nrUnitsCmd->SetGuidance("-1: for QweakSimG4 bar");
    nrUnitsCmd->SetGuidance("0: for simple Pb bar with detMaterial");
    nrUnitsCmd->SetGuidance("n>0: n Stacked detectors with 2mm Pb and 0.1mm detMaterial");
    nrUnitsCmd->SetParameterName("nrUnits", false);

    radThickCmd = new G4UIcmdWithADoubleAndUnit("/msc/det/setRadiatorThickness",this);
    radThickCmd->SetGuidance("Used only for the simple simulation");
    radThickCmd->SetGuidance(" use before setNrUnits");
    radThickCmd->SetParameterName("radThickness", false);

    polCmd = new G4UIcmdWithAString("/msc/PrimaryEventGen/setPolarization",this);
    polCmd->SetGuidance(" Set the polarization direction for each event:");
    polCmd->SetGuidance(" default L; accepted: L, mL, V, mV");
    polCmd->SetParameterName("radThickness", false);

    writeTreeCmd = new G4UIcmdWithAnInteger("/msc/SteppingAction/setWriteTree",this);
    writeTreeCmd->SetGuidance("Output tree from stepping action?");
    writeTreeCmd->SetGuidance(" 0: no");
    writeTreeCmd->SetGuidance(" 1: write tree for detector material");
    writeTreeCmd->SetGuidance(" 2: write tree for all steps");
    writeTreeCmd->SetParameterName("writeTree", false);

    writeANCmd = new G4UIcmdWithAnInteger("/msc/SteppingAction/setANdata",this);
    writeANCmd->SetGuidance("Output text file with AN at each call to phys process?");
    writeANCmd->SetGuidance(" 0: no");
    writeANCmd->SetGuidance(" else: yes");
    writeANCmd->SetParameterName("writeANdata", false);

    initOutCmd = new G4UIcmdWithAnInteger("/msc/SteppingAction/initOutput",this);
    initOutCmd->SetGuidance("Initialize the output");
    initOutCmd->SetParameterName("initOutput", false);
}

mscMessenger::~mscMessenger(){
  delete nrUnitsCmd;
  delete radThickCmd;
  delete polCmd;
  delete writeTreeCmd;
  delete writeANCmd;
}


void mscMessenger::SetNewValue(G4UIcommand* cmd, G4String newValue){

  if( cmd == nrUnitsCmd ){
    G4int val = nrUnitsCmd->GetNewIntValue(newValue);
    fDetCon->SetNrUnits(val);
    fDetCon->UpdateGeometry();
    if(val>=15){
      G4cerr<< __PRETTY_FUNCTION__<<" your number of units is larger than max 15 "<<val<<G4endl;
      G4cerr<<" You will need to modify the code! Exiting!"<<G4endl;
      exit(1);
    }
    fStepAct->SetNrUnits(val);
  }else if( cmd == radThickCmd ){
    G4double val = radThickCmd->GetNewDoubleValue(newValue);
    fDetCon->SetRadiatorThickness( val );
  }else if( cmd == polCmd ){
    fPriGen->SetPolarization( newValue );
  }else if( cmd == writeANCmd ){
    G4int val = writeANCmd->GetNewIntValue(newValue);
    fStepAct->SetWriteANdata(val);
  }else if( cmd == writeTreeCmd ){
    G4int val = writeTreeCmd->GetNewIntValue(newValue);
    fStepAct->SetWriteTree(val);
  }else if( cmd == initOutCmd ){
    fStepAct->InitOutput();
  }

}
