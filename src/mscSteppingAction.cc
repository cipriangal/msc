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

  /*Create root file and initialize what I want to put in it*/
  fout=new TFile("o_mscSteppingAction.root","RECREATE");
  tout=new TTree("t","Stepping action event tree");

  tout->Branch("prePosX",&pre_pos_x,"prePosX/D");
  tout->Branch("postPosX",&post_pos_x,"postPosX/D");
  
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


  G4Track*              theTrack     = theStep->GetTrack();
  G4ParticleDefinition* particleType = theTrack->GetDefinition();
  G4StepPoint*          thePrePoint  = theStep->GetPreStepPoint();
  G4StepPoint*          thePostPoint = theStep->GetPostStepPoint();
  //G4VPhysicalVolume*    thePostPV    = thePostPoint->GetPhysicalVolume();
  G4String              particleName = theTrack->GetDefinition()->GetParticleName();
  
  //get material
  G4Material* theMaterial = theTrack->GetMaterial();



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
    if(theMaterial->GetName().compare("PBA")==0){
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

  pre_pos_x = thePrePoint->GetPosition().getX();
  post_pos_x = thePostPoint->GetPosition().getX();

  tout->Fill();

}




