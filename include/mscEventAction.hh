#ifndef mscEventAction_h
#define mscEventAction_h 1

#include "G4UserEventAction.hh"

#include "G4THitsMap.hh"
#include "globals.hh"

class G4GenericMessenger;

/// Event action class
///
/// In EndOfEventAction(), it prints the accumulated quantities of the energy 
/// deposit and track lengths of charged particles in Absober and Gap layers 
/// stored in the hits collections.
///
/// The data member fPrintModulo defines the frequency of printing
/// the accumulated quantities. Its value can be changed via a command
/// defined using G4GenericMessenger class:
/// - /B4/event/setPrintModulo value

class mscEventAction : public G4UserEventAction
{
public:
  mscEventAction();
  virtual ~mscEventAction();

  virtual void  BeginOfEventAction(const G4Event* event);
  virtual void    EndOfEventAction(const G4Event* event);
                     
  // set methods
  void SetPrintModulo(G4int value);
    
private:
  // methods
  G4THitsMap<G4double>* GetHitsCollection(const G4String& hcName,
                                          const G4Event* event) const;
  G4double GetSum(G4THitsMap<G4double>* hitsMap) const;
  void PrintEventStatistics(G4double absoEdep, G4double absoTrackLength,
                            G4double gapEdep, G4double gapTrackLength) const;
  
  // data members                   
  G4GenericMessenger*  fMessenger;
  G4int  fPrintModulo;
};

// inline functions

inline void mscEventAction::SetPrintModulo(G4int value) {
  fPrintModulo = value;
}
                     
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif

    
