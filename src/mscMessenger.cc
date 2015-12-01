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

    fdetcon       = NULL;
    fevact        = NULL;
    fprigen       = NULL;
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

}

mscMessenger::~mscMessenger(){
  delete nrUnitsCmd;
  delete radThickCmd;
  delete polCmd;
}


void mscMessenger::SetNewValue(G4UIcommand* cmd, G4String newValue){

  if( cmd == nrUnitsCmd ){
    G4int val = nrUnitsCmd->GetNewIntValue(newValue);
    fdetcon->SetNrUnits( val );
    fdetcon->UpdateGeometry();
  }else if( cmd == radThickCmd ){
    G4double val = radThickCmd->GetNewDoubleValue(newValue);
    fdetcon->SetRadiatorThickness( val );
  }else if( cmd == polCmd ){
    fprigen->SetPolarization( newValue );
  }

}
