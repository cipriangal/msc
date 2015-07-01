#ifndef mscSteppingAction_hh
#define mscSteppingAction_hh 1

#include "G4UserSteppingAction.hh"
#include "globals.hh"

class mscSteppingAction : public G4UserSteppingAction
{
public:
  mscSteppingAction(G4int*);
  ~mscSteppingAction();
  
  void UserSteppingAction(const G4Step*);
  
private:
  G4int *evNr;
};

#endif
