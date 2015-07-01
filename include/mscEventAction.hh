#ifndef mscEventAction_h
#define mscEventAction_h 1

#include "G4UserEventAction.hh"
#include "globals.hh"

class G4GenericMessenger;

class mscEventAction : public G4UserEventAction
{
public:
  mscEventAction(G4int *evN);
  virtual ~mscEventAction();

  virtual void  BeginOfEventAction(const G4Event* event);
  virtual void    EndOfEventAction(const G4Event* event);
                     
  // set methods
  void SetPrintModulo(G4int value);
    
private:
  
  // data members                   
  G4GenericMessenger*  fMessenger;
  G4int  fPrintModulo;
  G4int *evNr;
};

// inline functions

inline void mscEventAction::SetPrintModulo(G4int value) {
  fPrintModulo = value;
}
                     
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif

    
