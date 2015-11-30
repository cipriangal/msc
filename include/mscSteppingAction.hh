#ifndef mscSteppingAction_hh
#define mscSteppingAction_hh 1

#include "G4UserSteppingAction.hh"
#include "globals.hh"

#include "TFile.h"
#include "TH3I.h"
#include "TTree.h"
#include "TGraph.h"

class mscSteppingAction : public G4UserSteppingAction
{
public:
  mscSteppingAction(G4int*);
  ~mscSteppingAction();
  
  void UserSteppingAction(const G4Step*);
  void InitVar();
  
private:
  G4int *evNr;
  TFile *fout;
  TTree *tout;
  static const G4int nrUnit=15;
  TH3I *hdistPe[nrUnit];//pos, ang, E
  TH3I *hdistAe[nrUnit];

  //tree variables
  G4int eventNr;
  G4int material; //PBA:0 ; detectorMat:1
  G4int unitNo;
  
  G4double stepSize;
  
  G4int pType;    
  G4int trackID;
  G4int parentID;

  G4double  preE; // total energy
  G4double  preKE;// kinetic energy

  G4double  prePosX; //position
  G4double  prePosY;
  G4double  prePosZ;
  G4double  preMomX; //momentum
  G4double  preMomY;
  G4double  preMomZ;  
  
  G4double preAngX; //angle along x (long section of the detector)
  G4double preAngY; //angle along y (short section of the detector)

  G4double  projPosX; //position at MD face (z=5 cm)
  G4double  projPosY;

  G4double powE;
  G4double powTh;
  G4double powPow;
  
  TGraph perpDepol;

};

#endif
