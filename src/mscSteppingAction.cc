#include "mscSteppingAction.hh"

#include "G4SteppingManager.hh"
#include "G4Track.hh"
#include "G4Step.hh"
#include "G4StepPoint.hh"
#include "G4TrackStatus.hh"
#include "G4VPhysicalVolume.hh"
#include "G4ParticleDefinition.hh"
#include "G4ParticleTypes.hh"

#include <fstream>
/*
  Brem depolarization: PhysRev.114.887
  implemented only for transverse polarization
  will decrease the overall polarization by a functional form from figure 5 in the paper      
*/
static const int perpNval=128;
static const double perpXDepol[perpNval]={0.053,0.066,0.075,0.084,0.097,0.112,0.126,0.137,0.148,0.160,0.169,0.180,0.190,0.200,0.210,0.220,0.226,0.236,0.244,0.251,0.258,0.267,0.274,0.283,0.291,0.300,0.308,0.316,0.324,0.332,0.340,0.350,0.357,0.364,0.371,0.379,0.387,0.395,0.403,0.411,0.417,0.425,0.434,0.443,0.453,0.460,0.468,0.477,0.488,0.499,0.509,0.518,0.525,0.532,0.541,0.548,0.555,0.563,0.570,0.576,0.581,0.587,0.594,0.600,0.608,0.616,0.623,0.628,0.635,0.640,0.646,0.653,0.661,0.668,0.676,0.683,0.688,0.694,0.699,0.706,0.709,0.715,0.722,0.728,0.734,0.738,0.745,0.749,0.752,0.757,0.762,0.769,0.776,0.782,0.789,0.795,0.799,0.806,0.810,0.815,0.824,0.830,0.836,0.843,0.847,0.853,0.858,0.864,0.868,0.874,0.881,0.887,0.893,0.900,0.908,0.915,0.920,0.930,0.935,0.945,0.953,0.963,0.969,0.974,0.979,0.985,0.990,0.994};
static const double perpYDepol[perpNval]={0.203,0.277,0.399,0.522,0.764,0.890,1.250,1.608,1.966,2.208,2.565,2.924,3.281,3.639,4.114,4.472,4.945,5.421,5.895,6.251,6.485,6.960,7.556,8.030,8.504,9.097,9.671,10.145,10.620,11.094,11.804,12.396,12.988,13.579,14.161,14.871,15.580,16.407,17.234,17.709,18.417,19.127,20.072,21.021,21.968,22.794,23.744,24.689,25.871,27.288,28.116,29.179,30.006,30.838,31.901,32.968,34.030,34.975,35.920,36.746,37.455,38.164,38.874,39.935,40.998,41.944,43.007,43.933,44.878,45.822,46.884,47.770,48.951,50.132,51.195,52.376,53.437,54.265,55.096,56.041,56.867,57.694,58.757,59.584,60.529,61.573,62.519,63.227,63.935,64.644,65.472,66.653,67.599,68.662,69.725,70.788,71.615,72.449,73.275,74.107,75.290,76.359,77.304,78.251,78.959,79.906,80.733,81.679,82.506,83.334,84.281,85.109,86.055,87.120,88.068,89.015,89.961,91.028,91.862,92.929,94.000,95.421,96.132,96.725,97.436,98.387,98.862,99.219};

mscSteppingAction::mscSteppingAction(G4int *evN)		
{
  //eventID pointer from the mscEventAction.cc file
  evNr=evN;

  std::ofstream ofs;
  ofs.open("o_msc_ANdata.txt",std::ofstream::out);
  ofs<<"energy[MeV] cos(theta) anaPower polarization"<<G4endl;
  ofs.close();
  
  for(int i=0;i<perpNval;i++) perpDepol.SetPoint(i,perpXDepol[i],perpYDepol[i]);   
  
  for(int i=0;i<3;i++) interactionNr.push_back(0);
  
  /*Create root file and initialize what I want to put in it*/
  fout=new TFile("o_msc.root","RECREATE");
  tout=new TTree("t","Stepping action event tree");

  tout->Branch("postE",&postE,"postE/D");
  tout->Branch("postPosX",&postPosX,"postPosX/D");
  tout->Branch("postPosY",&postPosY,"postPosY/D");
  tout->Branch("postPosZ",&postPosZ,"postPosZ/D");
  tout->Branch("postMomX",&postMomX,"postMomX/D");
  tout->Branch("postMomY",&postMomY,"postMomY/D");
  tout->Branch("postMomZ",&postMomZ,"postMomZ/D");

  tout->Branch( "prePosX", &prePosX, "prePosX/D");
  tout->Branch( "prePosY", &prePosY, "prePosY/D");
  tout->Branch( "prePosZ", &prePosZ, "prePosZ/D");
  tout->Branch( "preMomX", &preMomX, "preMomX/D");
  tout->Branch( "preMomY", &preMomY, "preMomY/D");
  tout->Branch( "preMomZ", &preMomZ, "preMomZ/D");

  tout->Branch("prePhi"  ,&prePhi  ,"prePhi/D"  );
  tout->Branch("preTheta",&preTheta,"preTheta/D");
  tout->Branch("preAngX",&preAngX,"preAngX/D");
  tout->Branch("preAngY",&preAngY,"preAngY/D");

  tout->Branch("preE",&preE,"preE/D");
 
  tout->Branch("powE",  &powE,"powE/D");
  tout->Branch("powTh", &powTh,"powTh/D");
  tout->Branch("powPow",&powPow,"powPow/D");

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
  G4ThreeVector _polarization=theTrack->GetPolarization();
  G4String _pn=thePostPoint->GetProcessDefinedStep()->GetProcessName();
  G4double depol(0),eLossPercent(0);

  InitVar();

  eventNr=*evNr;
  if(currentEvent!=eventNr){ //new event
    for(int i=0;i<3;i++) interactionNr[i]=0;
    currentEvent=eventNr;
  }
  
  if(theMaterial){    
    if(theMaterial->GetName().compare("detectorMat")==0)
      material=1;
    else if(theMaterial->GetName().compare("PBA")==0){
      material=0;

      eLossPercent = 1. - thePostPoint->GetKineticEnergy()/thePrePoint->GetKineticEnergy();
      
      if(_pn.compare("eBrem")==0 &&                           // only for eBrem
	 (_polarization.getX()>0 || _polarization.getY()>0)){ // only for transverse polarization
	depol=0;
	if( eLossPercent > perpXDepol[perpNval-1]) depol = 1.;
	else if( eLossPercent >= perpXDepol[0] ) depol = perpDepol.Eval(eLossPercent,0,"S")/100.;
	else depol = 0.;
      }
      
      _polarization *= (1.-depol);
      theStep->GetTrack()->SetPolarization(_polarization);
    }
  }

  if(thePostPV){
    if(thePostPV->GetName().compare("radiator")==0){
      volume=0;
      interactionNr[0]++;
      intNr=interactionNr[0];
    }else if(thePostPV->GetName().compare("detector1")==0){
      volume=1;
      interactionNr[1]++;           
      intNr=interactionNr[1];
    }else if(thePostPV->GetName().compare("detector2")==0){
      volume=2;
      interactionNr[2]++;     
      intNr=interactionNr[2];
    }    
  }

  pType = particleType->GetPDGEncoding();
  trackID = theStep->GetTrack()->GetTrackID();
  parentID = theStep->GetTrack()->GetParentID();

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

  // preE  =  thePrePoint->GetKineticEnergy();
  // postE = thePostPoint->GetKineticEnergy();
  preE  =  thePrePoint->GetTotalEnergy();
  postE = thePostPoint->GetTotalEnergy();
  
  if(thePrePoint->GetMomentum().getR()>0){
    prePhi = thePrePoint->GetMomentum().getPhi();
    preTheta = thePrePoint->GetMomentum().getTheta();
    preAngX = atan2(sin(preTheta)*cos(prePhi),cos(preTheta)) * 180. / CLHEP::pi;
    preAngY = atan2(sin(preTheta)*sin(prePhi),cos(preTheta)) * 180. / CLHEP::pi;
    prePhi   *= 180. / CLHEP::pi;
    preTheta *= 180. / CLHEP::pi;
  }

  // if(fabs(preAngX)<200 && trackID==1 && parentID==0 && prePosZ<40.5 && prePosZ>30){    
  //   G4cout<<G4endl<<preAngX<<G4endl<<" Pos: "<<prePosX<<" "<<prePosY<<" "<<prePosZ<<" "<<G4endl;
  //   G4cout<<" Mom: "<<preMomX<<" "<<preMomY<<" "<<preMomZ<<" "<<G4endl;
  //   G4cout<<" Ang: "<<prePhi<<" "<<preTheta<<G4endl;
  // }
  
  /*fill tree*/ 
  if(material==1){
    tout->Fill();
    //theTrack->SetTrackStatus(fStopAndKill);
  }
}

void mscSteppingAction::InitVar(){
  prePosX  = -999;
  prePosY  = -999;
  prePosZ  = -999;
  postPosX = -999;
  postPosY = -999;
  postPosZ = -999;

  preMomX  = -999;
  preMomY  = -999;
  preMomZ  = -999;
  postMomX = -999;
  postMomY = -999;
  postMomZ = -999;

  prePhi   = -999;
  preTheta = -999;
  preAngX  = -999;
  preAngY  = -999;

  preE  = -999;
  postE = -999;
  
  eventNr = -999;
  material = -999;
  volume = -999;
  pType = -999;
  trackID = -999;
  parentID = -999;
  intNr = -999;
  process = -999;

  powE = -999;
  powTh= -999;
  powPow=-999;
}




