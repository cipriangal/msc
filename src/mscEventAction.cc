#include "mscEventAction.hh"

#include "G4Event.hh"
#include "G4HCofThisEvent.hh"
#include "G4GenericMessenger.hh"
#include "G4UnitsTable.hh"

#include "Randomize.hh"
#include <iomanip>

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

mscEventAction::mscEventAction(G4int *evN)
 : G4UserEventAction(),
   fMessenger(0),
   fPrintModulo(1)
{
  evNr=evN;
  // Define /msc/event commands using generic messenger class
  fMessenger = new G4GenericMessenger(this, "/msc/event/", "Event control");

  // Define /msc/event/setPrintModulo command
  G4GenericMessenger::Command& setPrintModulo 
    = fMessenger->DeclareProperty("setPrintModulo", 
                                  fPrintModulo, 
				  "Print event number modulo n");
  setPrintModulo.SetRange("value>0");                                
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

mscEventAction::~mscEventAction()
{
  delete fMessenger;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void mscEventAction::BeginOfEventAction(const G4Event* event)
{  
  G4int eventID = event->GetEventID();
  *evNr = eventID;
  
  if ( eventID % fPrintModulo == 0 )  { 
    G4cout << "\n---> Begin of event: " << eventID << G4endl;
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......


void mscEventAction::EndOfEventAction(const G4Event* event)
{   

  G4int eventID = event->GetEventID();
  if ( eventID % fPrintModulo == 0) {
    G4cout << "---> End of event: " << eventID << G4endl;     
  }

}  

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
