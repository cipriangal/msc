#include "mscRunAction.hh"

#include "G4Run.hh"
#include "G4RunManager.hh"
#include "G4UnitsTable.hh"
#include "G4SystemOfUnits.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

mscRunAction::mscRunAction()
 : G4UserRunAction()
{ 
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

mscRunAction::~mscRunAction()
{
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void mscRunAction::BeginOfRunAction(const G4Run* run)
{ 
  G4cout << "### Run " << run->GetRunID() << " start." << G4endl;

  //inform the runManager to save random number seed
  G4RunManager::GetRunManager()->SetRandomNumberStore(true);
  
<<<<<<< HEAD
  // Book histograms, ntuple
  //
  
  // Create analysis manager
  // The choice of analysis technology is done via selectin of a namespace
  // in mscAnalysis.hh
  G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();
  G4cout << "Using " << analysisManager->GetType() 
         << " analysis manager" << G4endl;

  // Create directories 
  analysisManager->SetHistoDirectoryName("histograms");
  analysisManager->SetNtupleDirectoryName("ntuple");
  
  // Open an output file
  //
  G4String fileName = "MSC_Output1";
  analysisManager->OpenFile(fileName);
  analysisManager->SetFirstHistoId(1);

  //analysisManager->SetVerboseLevel(1);
  //analysisManager->SetFileName("MSC_Output");

  // Creating histograms
  //
  //analysisManager->CreateH1("1","Edep in absorber", 100, 0., 800*MeV);
  //analysisManager->CreateH1("2","Edep in gap", 100, 0., 100*MeV);
  //analysisManager->CreateH1("3","trackL in absorber", 100, 0., 1*m);
  //analysisManager->CreateH1("4","trackL in gap", 100, 0., 50*cm);
  //analysisManager->CreateH1("5","Energy", 100, 0., 500*MeV);

  analysisManager->CreateH1("Momentum", "Particle Momentum", 50, -100, 100);
  analysisManager->CreateH2("XYPosition","Particle Position",50, -1000., 1000, 50, -300., 300)

  // Creating ntuple
  //
  //analysisManager->CreateNtuple("MSC_Output", "Edep and TrackL");
  //analysisManager->CreateNtupleDColumn("Eabs");
  //analysisManager->CreateNtupleDColumn("Egap");
  //analysisManager->CreateNtupleDColumn("Labs");
  //analysisManager->CreateNtupleDColumn("Lgap");
  //analysisManager->FinishNtuple();


delete G4AnalysisManager::Instance();

=======
>>>>>>> 55555208103a7c2588f7c01ac5fb724336932c56
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void mscRunAction::EndOfRunAction(const G4Run* run)
{
<<<<<<< HEAD
  G4int nofEvents = aRun->GetNumberOfEvent();
  if ( nofEvents == 0 ) return;
  
  // print histogram statistics
  //
  /*  G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();
  if ( analysisManager->GetH1(1) ) {
    G4cout << "\n ----> print histograms statistic \n" << G4endl;
    
    G4cout 
       << " EAbs : mean = " << G4BestUnit(analysisManager->GetH1(1)->mean(), "Energy") 
               << " rms = " << G4BestUnit(analysisManager->GetH1(1)->rms(),  "Energy") 
               << G4endl;
    G4cout                
       << " EGap : mean = " << G4BestUnit(analysisManager->GetH1(2)->mean(), "Energy") 
               << " rms = " << G4BestUnit(analysisManager->GetH1(2)->rms(),  "Energy") 
               << G4endl;
    G4cout 
       << " LAbs : mean = " << G4BestUnit(analysisManager->GetH1(3)->mean(), "Length") 
               << " rms = " << G4BestUnit(analysisManager->GetH1(3)->rms(),  "Length") 
               << G4endl;
    G4cout 
       << " LGap : mean = " << G4BestUnit(analysisManager->GetH1(4)->mean(), "Length") 
               << " rms = " << G4BestUnit(analysisManager->GetH1(4)->rms(),  "Length") 
               << G4endl;
     } */
  
  // save histograms 
  //
  //analysisManager->Write();
  //analysisManager->CloseFile();
  
  // complete cleanup
  //
  //delete G4AnalysisManager::Instance();  
=======
  G4cout << "### Run " << run->GetRunID() << " stop." << G4endl;
>>>>>>> 55555208103a7c2588f7c01ac5fb724336932c56
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
