#include "mscSteppingAction.hh"

#include "G4SteppingManager.hh"
#include "G4Track.hh"
#include "G4Step.hh"
#include "G4StepPoint.hh"
#include "G4TrackStatus.hh"
#include "G4VPhysicalVolume.hh"
#include "G4ParticleDefinition.hh"
#include "G4ParticleTypes.hh"



mscSteppingAction::mscSteppingAction(G4int *evN)		
{
  //eventID pointer from the mscEventAction.cc file
  evNr=evN;

  for(int i=0;i<3;i++) interactionNr.push_back(0);
  
  /*Create root file and initialize what I want to put in it*/
  fout=new TFile("o_mscSteppingAction.root","RECREATE");
  tout=new TTree("t","Stepping action event tree");

  tout->Branch( "prePosX", &prePosX, "prePosX/D");
  tout->Branch( "prePosY", &prePosY, "prePosY/D");
  tout->Branch( "prePosZ", &prePosZ, "prePosZ/D");
  tout->Branch("postPosX",&postPosX,"postPosX/D");
  tout->Branch("postPosY",&postPosY,"postPosY/D");
  tout->Branch("postPosZ",&postPosZ,"postPosZ/D");

  tout->Branch( "preMomX", &preMomX, "preMomX/D");
  tout->Branch( "preMomY", &preMomY, "preMomY/D");
  tout->Branch( "preMomZ", &preMomZ, "preMomZ/D");
  tout->Branch("postMomX",&postMomX,"postMomX/D");
  tout->Branch("postMomY",&postMomY,"postMomY/D");
  tout->Branch("postMomZ",&postMomZ,"postMomZ/D");

  tout->Branch("evNr",&eventNr,"evNr/I");
  tout->Branch("material",&material,"material/I");
  tout->Branch("volume",&volume,"volume/I");
  tout->Branch("pType",&pType,"pType/I");
  tout->Branch("trackID",&trackID,"trackID/I");
  tout->Branch("parentID",&parentID,"parentID/I");
  tout->Branch("intNr",&intNr,"intNr/I");

  
}


mscSteppingAction::~mscSteppingAction()
{
  /*Write out root file*/
  fout->cd();
  tout->Write();
  fout->Close();
}


void mscSteppingAction::UserSteppingAction(const G4Step* theStep)
{
  static G4int currentEvent(-1);
  
  G4Track*              theTrack     = theStep->GetTrack();
  G4ParticleDefinition* particleType = theTrack->GetDefinition();
  G4StepPoint*          thePrePoint  = theStep->GetPreStepPoint();
  G4StepPoint*          thePostPoint = theStep->GetPostStepPoint();
  G4VPhysicalVolume*    thePostPV    = thePostPoint->GetPhysicalVolume();
  G4String              particleName = theTrack->GetDefinition()->GetParticleName();
  
  //get material
  G4Material* theMaterial = theTrack->GetMaterial();

  InitVar();

  if(currentEvent!=eventNr){ //new event
    interactionNr.clear();
  }

  if(theMaterial){
    
    if(theMaterial->GetName().compare("detectorMat")==0){
      
      G4cout<<" In  detector " << *evNr<<" "
	    <<theTrack->GetTrackID()<<" "<< theTrack->GetParentID()<< " "
	    <<thePrePoint->GetPosition().getX()<<" "
	    <<thePrePoint->GetMomentum().getX()<<" "
	    <<thePostPoint->GetPosition().getY()<<" "
	    <<thePostPoint->GetMomentum().getZ()<<" "
	    <<thePrePoint->GetTotalEnergy()<<" "
	    <<particleType->GetPDGEncoding()<<G4endl;    
    }
    else if(theMaterial->GetName().compare("PBA")==0){
      G4cout<<" In  radiator " << *evNr<<" "
	    <<theTrack->GetTrackID()<<" "<< theTrack->GetParentID()<< " "
	    <<thePrePoint->GetPosition().getX()<<" "
	    <<thePrePoint->GetMomentum().getX()<<" "
	    <<thePostPoint->GetPosition().getY()<<" "
	    <<thePostPoint->GetMomentum().getZ()<<" "
	    <<thePrePoint->GetTotalEnergy()<<" "
	    <<particleType->GetPDGEncoding()<<G4endl;   
    }
    
  }

  
  /*fill tree*/ 

  prePosX = thePrePoint->GetPosition().getX();
  postPosX = thePostPoint->GetPosition().getX();

  tout->Fill();

}

void mscSteppingAction::InitVar(){
   prePosX = -999;
   prePosY = -999;
   prePosZ = -999;
  postPosX = -999;
  postPosY = -999;
  postPosZ = -999;

   preMomX = -999;
   preMomY = -999;
   preMomZ = -999;
  postMomX = -999;
  postMomY = -999;
  postMomZ = -999;

  eventNr = -999;
  material = -999;
  volume = -999;
  pType = -999;
  trackID = -999;
  parentID = -999;
  intNr = -999;
}




