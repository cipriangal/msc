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
  tout->Branch("postPhi"  ,&postPhi  ,"postPhi/D"  );
  tout->Branch("postTheta",&postTheta,"postTheta/D");

  tout->Branch("evNr",&eventNr,"evNr/I");
  tout->Branch("material",&material,"material/I");
  tout->Branch("volume",&volume,"volume/I");
  tout->Branch("pType",&pType,"pType/I");
  tout->Branch("trackID",&trackID,"trackID/I");
  tout->Branch("parentID",&parentID,"parentID/I");
  tout->Branch("intNr",&intNr,"intNr/I");
  tout->Branch("process",&process,"process/I");
  
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

  eventNr=*evNr;
  if(currentEvent!=eventNr){ //new event
    for(int i=0;i<3;i++) interactionNr[i]=0;
    currentEvent=eventNr;
  }

  if(theMaterial){    
    if(theMaterial->GetName().compare("detectorMat")==0)
      material=1;
    else if(theMaterial->GetName().compare("PBA")==0)
      material=0;
  }

  if(thePostPV){
    if(thePostPV->GetName().compare("Radiator")==0){
      volume=0;
      interactionNr[0]++;
      intNr=interactionNr[0];
    }else if(thePostPV->GetName().compare("Detector1")==0){
      volume=1;
      interactionNr[1]++;           
      intNr=interactionNr[1];
    }else if(thePostPV->GetName().compare("Detector2")==0){
      volume=2;
      interactionNr[2]++;     
      intNr=interactionNr[2];
    }    
  }

  pType = particleType->GetPDGEncoding();
  trackID = theStep->GetTrack()->GetTrackID();
  parentID = theStep->GetTrack()->GetParentID();

  G4String _pn=thePostPoint->GetProcessDefinedStep()->GetProcessName();
  if(_pn.compare("msc")==0)
    process = 1;
  else if(_pn.compare("CoulombScat")==0)
    process = 2;
  else if(_pn.compare("eBrem")==0)
    process = 3;
  else if(_pn.compare("Transportation")==0)
    process = 4;
  else if(_pn.compare("eIoni")==0)
    process = 5;
  else if(_pn.compare("CoupledTransportation")==0){
    process = 6;
  }else{
    //G4cout<<" ~~"<<_pn<<G4endl;
    process = 0;
  }
  

  prePosX  =  thePrePoint->GetPosition().getX();
  prePosY  =  thePrePoint->GetPosition().getY();
  prePosZ  =  thePrePoint->GetPosition().getZ();
  postPosX = thePostPoint->GetPosition().getX();
  postPosY = thePostPoint->GetPosition().getY();
  postPosZ = thePostPoint->GetPosition().getZ();

  preMomX  =  thePrePoint->GetMomentum().getX();
  preMomY  =  thePrePoint->GetMomentum().getY();
  preMomZ  =  thePrePoint->GetMomentum().getZ();
  postMomX = thePostPoint->GetMomentum().getX();
  postMomY = thePostPoint->GetMomentum().getY();
  postMomZ = thePostPoint->GetMomentum().getZ();

  if(thePostPoint->GetMomentum().getR()>0){
    postPhi = thePostPoint->GetMomentum().getPhi();
    postTheta = thePostPoint->GetMomentum().getTheta();
  }

  /*fill tree*/ 
  tout->Fill();
  if(material==1)
    theTrack->SetTrackStatus(fStopAndKill);
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
  postPhi = -999;
  postTheta = -999;
  
  eventNr = -999;
  material = -999;
  volume = -999;
  pType = -999;
  trackID = -999;
  parentID = -999;
  intNr = -999;
  process = -999;
}




