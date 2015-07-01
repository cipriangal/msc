#ifndef mscRunAction_h
#define mscRunAction_h 1

#include "G4UserRunAction.hh"
#include "globals.hh"

class G4Run;

class mscRunAction : public G4UserRunAction
{
  public:
    mscRunAction();
    virtual ~mscRunAction();

    virtual void BeginOfRunAction(const G4Run*);
    virtual void   EndOfRunAction(const G4Run*);
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif

