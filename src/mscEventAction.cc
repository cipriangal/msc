#include "mscEventAction.hh"
#include "mscAnalysis.hh"

#include "G4RunManager.hh"
#include "G4Event.hh"
#include "G4SDManager.hh"
#include "G4HCofThisEvent.hh"
#include "G4GenericMessenger.hh"
#include "G4UnitsTable.hh"

#include "Randomize.hh"
#include <iomanip>

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

mscEventAction::mscEventAction()
 : G4UserEventAction(),
   fMessenger(0),
   fPrintModulo(1)
{
  // Define /msc/event commands using generic messenger class
  fMessenger = new G4GenericMessenger(this, "/msc/event/", "Event control");

  // Define /msc/event/setPrintModulo command
  G4GenericMessenger::Command& setPrintModulo 
    = fMessenger->DeclareProperty("setPrintModulo", 
                                  fPrintModulo, 
                                 "Print events modulo n");
  setPrintModulo.SetRange("value>0");                                
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

mscEventAction::~mscEventAction()
{
  delete fMessenger;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4THitsMap<G4double>* 
mscEventAction::GetHitsCollection(const G4String& hcName,
                                  const G4Event* event) const
{
  G4int hcID 
    = G4SDManager::GetSDMpointer()->GetCollectionID(hcName);
  G4THitsMap<G4double>* hitsCollection 
    = static_cast<G4THitsMap<G4double>*>(
        event->GetHCofThisEvent()->GetHC(hcID));
  
  if ( ! hitsCollection ) {
    G4cerr << "Cannot access hitsCollection " << hcName << G4endl;
    exit(1);
  }         

  return hitsCollection;
}    

G4double mscEventAction::GetSum(G4THitsMap<G4double>* hitsMap) const
{
  G4double sumValue = 0;
  std::map<G4int, G4double*>::iterator it;
  for ( it = hitsMap->GetMap()->begin(); it != hitsMap->GetMap()->end(); it++) {
    sumValue += *(it->second);
  }
  return sumValue;  
}  

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

/*
void mscEventAction::PrintEventStatistics(
                            G4double absoEdep, G4double absoTrackLength,
                            G4double gapEdep, G4double gapTrackLength) const
{
  // Print event statistics
  //
  G4cout
     << "   Absorber: total energy: " 
     << std::setw(7) << G4BestUnit(absoEdep, "Energy")
     << "       total track length: " 
     << std::setw(7) << G4BestUnit(absoTrackLength, "Length")
     << G4endl
     << "        Gap: total energy: " 
     << std::setw(7) << G4BestUnit(gapEdep, "Energy")
     << "       total track length: " 
     << std::setw(7) << G4BestUnit(gapTrackLength, "Length")
     << G4endl;
}
*/
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void mscEventAction::BeginOfEventAction(const G4Event* event)
{  

  G4int eventID = event->GetEventID();
  if ( eventID % fPrintModulo == 0 )  { 
    G4cout << "\n---> Begin of event: " << eventID << G4endl;
    //CLHEP::HepRandom::showEngineStatus();
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......


void mscEventAction::EndOfEventAction(const G4Event* event)
{  

  G4double position
    = GetSum(GetHitsCollection("Position", event));
 
  analysisManager->FillH2(XYPosition, localPos.x(), localPos.y());
 
  G4double momentum
    = GetSum(GetHitsCollection("Momentum", event));  

  analysisManager->FillH1(Momentum, Detector1);


  // Get sum value from hits collections
  //
/*G4double absoEdep 
    = GetSum(GetHitsCollection("Absorber/Edep", event));

  G4double gapEdep 
    = GetSum(GetHitsCollection("Gap/Edep", event));

  G4double absoTrackLength 
    = GetSum(GetHitsCollection("Absorber/TrackLength", event));

  G4double gapTrackLength 
    = GetSum(GetHitsCollection("Gap/TrackLength", event));

  // get analysis manager
  G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();

  // fill histograms
  //  
  analysisManager->FillH1(1, absoEdep);
  
  // fill ntuple
  //
  analysisManager->FillNtupleDColumn(0, absoEdep);
  analysisManager->FillNtupleDColumn(1, gapEdep);
  analysisManager->FillNtupleDColumn(2, absoTrackLength);
  analysisManager->FillNtupleDColumn(3, gapTrackLength);
  analysisManager->AddNtupleRow();  
  
  //print per event (modulo n)
  //
  G4int eventID = event->GetEventID();
  if ( eventID % fPrintModulo == 0) {
    G4cout << "---> End of event: " << eventID << G4endl;     

    PrintEventStatistics(absoEdep, absoTrackLength, gapEdep, gapTrackLength);
    }*/ 
}  

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
