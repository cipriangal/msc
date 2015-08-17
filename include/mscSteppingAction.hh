#ifndef mscSteppingAction_hh
#define mscSteppingAction_hh 1

#include "G4UserSteppingAction.hh"
#include "globals.hh"

#include "TFile.h"
#include "TTree.h"

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

  //tree variables
  G4double  prePosX; //position
  G4double  prePosY;
  G4double  prePosZ;
  G4double postPosX;
  G4double postPosY;
  G4double postPosZ;  
  
  G4double  preMomX; //momentum
  G4double  preMomY;
  G4double  preMomZ;
  G4double postMomX;
  G4double postMomY;
  G4double postMomZ;
  G4double postPhi;
  G4double postTheta;
  G4double postAngX; //angle along x (long section of the detector)
  G4double postAngY; //angle along y (short section of the detector)
  
  G4double  preE;
  G4double postE;

  
  G4int eventNr;
  G4int material; //PBA:0 ; detectorMat:1
  G4int volume;   //Radiator:0 ; Det1:1 ; Det2:2
  G4int pType;    
  G4int trackID;
  G4int parentID;
  G4int intNr;
  G4int process; //0:N/A 1:msc 2:CoulombScat 3:eBrem 4:Transportation 5:eIon 6:coupledTransportation

  //number of interactions in 0:PBA | 1:det1 | 2:det2
  std::vector<G4int> interactionNr; 
};

#endif
