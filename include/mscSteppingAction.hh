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
  
private:
  G4int *evNr;
  TFile *fout;
  TTree *tout;

  //tree variables
  G4double pre_pos_x;
  G4double post_pos_x;
};

#endif
