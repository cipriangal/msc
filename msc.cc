#include "mscDetectorConstruction.hh"
#include "mscPrimaryGeneratorAction.hh"
#include "mscRunAction.hh"
#include "mscEventAction.hh"
#include "mscSteppingAction.hh"
#include "mscMessenger.hh"

#include "G4RunManager.hh"
#include "G4ScoringManager.hh"
#include "G4UImanager.hh"

#include "Randomize.hh"

#include "G4Version.hh"
#if G4VERSION_NUMBER < 1000
#include "G4StepLimiterBuilder.hh"
#else
#include "G4StepLimiterPhysics.hh"
#endif

#ifdef G4VIS_USE
#include "G4VisExecutive.hh"
#endif

#ifdef G4UI_USE
#include "G4UIExecutive.hh"
#endif

#include "G4PhysListFactory.hh"

#define USE_CUSTOM_NUCLEAR_SCATTERING 1
#if USE_CUSTOM_NUCLEAR_SCATTERING
#include "physics_lists/constructors/electromagnetic/QweakSimEmStandardPhysics.hh"
#include "physics_lists/constructors/electromagnetic/QweakSimEmLivermorePhysics.hh"
#endif

#include <time.h>

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

namespace {
  void PrintUsage() {
    G4cerr << " Usage: " << G4endl;
    G4cerr << " msc [-m macro ] [-u UIsession]" << G4endl;
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

int main(int argc,char** argv)
{
  clock_t tStart=clock();

  // Evaluate arguments
  //
  if ( argc > 5 ) {
    PrintUsage();
    return 1;
  }
  
  G4String macro;
  G4String session;
  for ( G4int i=1; i<argc; i=i+2 ) {
    if      ( G4String(argv[i]) == "-m" ) macro = argv[i+1];
    else if ( G4String(argv[i]) == "-u" ) session = argv[i+1];
    else {
      PrintUsage();
      return 1;
    }
  }  

  // Choose the Random engine
  //
  CLHEP::HepRandom::setTheEngine(new CLHEP::RanecuEngine);
  
  // Construct the default run manager
  //
  G4RunManager * runManager = new G4RunManager;

  mscMessenger *mscMess = new mscMessenger();  
  
  // Activate command-based scorer
  G4ScoringManager* scoringManager = G4ScoringManager::GetScoringManager(); 
  scoringManager->SetVerboseLevel(1);

  // Set mandatory initialization classes
  //
  mscDetectorConstruction* detConstruction = new mscDetectorConstruction();
  runManager->SetUserInitialization(detConstruction);
  mscMess->SetDetCon( detConstruction );

  // Calls a reference physics list for the simulation
  G4PhysListFactory factory;
  G4VModularPhysicsList* physlist = factory.GetReferencePhysList("QGSP_BERT_LIV");
#if G4VERSION_NUMBER < 1000
  physlist->RegisterPhysics(new G4StepLimiterBuilder());
#else
  physlist->RegisterPhysics(new G4StepLimiterPhysics());
#endif

  // Replace the standard EM with the customized version to add Pb A_T
#if USE_CUSTOM_NUCLEAR_SCATTERING
  physlist->ReplacePhysics(new QweakSimEmLivermorePhysics());
#endif

  runManager->SetUserInitialization(physlist);

  
  /* Event number */
  G4int evNumber(0);

  // Set user action classes
  //
  mscPrimaryGeneratorAction *prigen=new mscPrimaryGeneratorAction();
  runManager->SetUserAction( prigen );
  mscMess->SetPriGen(prigen);
  //
  runManager->SetUserAction(new mscRunAction());
  //
  runManager->SetUserAction(new mscEventAction(&evNumber));
  //
  mscSteppingAction *stepAct=new mscSteppingAction(&evNumber);
  runManager->SetUserAction(stepAct);
  mscMess->SetStepAct(stepAct);

  // Initialize G4 kernel
  //
  runManager->Initialize();
  
#ifdef G4VIS_USE
  // Initialize visualization
  G4VisManager* visManager = new G4VisExecutive;
  // G4VisExecutive can take a verbosity argument - see /vis/verbose guidance.
  // G4VisManager* visManager = new G4VisExecutive("Quiet");
  visManager->Initialize();
#endif

  // Get the pointer to the User Interface manager
  G4UImanager* UImanager = G4UImanager::GetUIpointer();

  if ( macro.size() ) {
    // batch mode
    G4String command = "/control/execute ";
    UImanager->ApplyCommand(command+macro);
  }
  else  {  
    // interactive mode : define UI session
#ifdef G4UI_USE
    G4UIExecutive* ui = new G4UIExecutive(argc, argv, session);
#ifdef G4VIS_USE
    UImanager->ApplyCommand("/control/macroPath macros"); 
    UImanager->ApplyCommand("/control/execute init_vis.mac"); 
#else
    UImanager->ApplyCommand("/control/macroPath macros"); 
    UImanager->ApplyCommand("/control/execute init.mac"); 
#endif
    if (ui->IsGUI()){
      UImanager->ApplyCommand("/control/macroPath macros"); 
      UImanager->ApplyCommand("/control/execute gui.mac");
    }
    ui->SessionStart();
    delete ui;
#endif
  }

  // Job termination
  // Free the store: user actions, physics_list and detector_description are
  // owned and deleted by the run manager, so they should not be deleted 
  // in the main() program !

#ifdef G4VIS_USE
  delete visManager;
#endif
  delete runManager;

  G4cout<<" Running time[s]: "<< (double) ((clock() - tStart)/CLOCKS_PER_SEC)<<G4endl;
  return 0;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo.....
