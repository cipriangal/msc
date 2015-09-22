#include "mscSteppingAction.hh"

#include "G4SteppingManager.hh"
#include "G4Track.hh"
#include "G4Step.hh"
#include "G4StepPoint.hh"
#include "G4TrackStatus.hh"
#include "G4VPhysicalVolume.hh"
#include "G4ParticleDefinition.hh"
#include "G4ParticleTypes.hh"

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

  for(int i=0;i<perpNval;i++) perpDepol.SetPoint(i,perpXDepol[i],perpYDepol[i]);   
  
  /*Create root file and initialize what I want to put in it*/
  fout=new TFile("o_msc.root","RECREATE");
  hPosAngUnit=new TH3D("hPosAngUnit","E>2 primaries;pos [cm];angle [deg];unit number",
		       200,-30,30,
		       180,-90,90,
		       20,0,20);
  
  tout=new TTree("t","Stepping action event tree");

  tout->Branch("evNr",&eventNr,"evNr/I");
  tout->Branch("material",&material,"material/I");
  tout->Branch("unitNo",&unitNo,"unitNo/I");

  tout->Branch("pType",&pType,"pType/I");
  tout->Branch("trackID",&trackID,"trackID/I");
  tout->Branch("parentID",&parentID,"parentID/I");

  tout->Branch("preE",&preE,"preE/D");
  tout->Branch("preKE",&preKE,"preKE/D");

  tout->Branch("prePosX", &prePosX, "prePosX/D");
  tout->Branch("prePosY", &prePosY, "prePosY/D");
  tout->Branch("prePosZ", &prePosZ, "prePosZ/D");
  tout->Branch("preMomX", &preMomX, "preMomX/D");
  tout->Branch("preMomY", &preMomY, "preMomY/D");
  tout->Branch("preMomZ", &preMomZ, "preMomZ/D");

  tout->Branch("preAngX",&preAngX,"preAngX/D");
  tout->Branch("preAngY",&preAngY,"preAngY/D");

  tout->Branch("projPosX",&projPosX,"projPosX/D");
  tout->Branch("projPosY",&projPosY,"projPosY/D");
  
}


mscSteppingAction::~mscSteppingAction()
{
  /*Write out root file*/
  fout->cd();
  hPosAngUnit->Write();
  tout->Write();
  fout->Close();
}


void mscSteppingAction::UserSteppingAction(const G4Step* theStep)
{
  G4Track*              theTrack     = theStep->GetTrack();
  G4ParticleDefinition* particleType = theTrack->GetDefinition();
  G4StepPoint*          thePrePoint  = theStep->GetPreStepPoint();
  G4StepPoint*          thePostPoint = theStep->GetPostStepPoint();
  G4String              particleName = theTrack->GetDefinition()->GetParticleName();

  //get material
  G4Material* theMaterial = theTrack->GetMaterial();
  G4ThreeVector _polarization=theTrack->GetPolarization();
  G4String _pn=thePostPoint->GetProcessDefinedStep()->GetProcessName();
  G4double depol(0),eLossPercent(0);
  
  InitVar();

  const G4double MDposZ = 5 * CLHEP::cm;

  eventNr=*evNr;
  
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

  G4TouchableHandle theTouchable = thePrePoint->GetTouchableHandle();
  if(theTouchable->GetVolume(0)->GetName().compare("detector")==0)
    unitNo = theTouchable->GetCopyNumber(1);
  else
    unitNo = -999;
 
  pType = particleType->GetPDGEncoding();
  trackID = theStep->GetTrack()->GetTrackID();
  parentID = theStep->GetTrack()->GetParentID();

  preE  =  thePrePoint->GetTotalEnergy();
  preKE = thePostPoint->GetKineticEnergy();

  prePosX  =  thePrePoint->GetPosition().getX();
  prePosY  =  thePrePoint->GetPosition().getY();
  prePosZ  =  thePrePoint->GetPosition().getZ();
  preMomX  =  thePrePoint->GetMomentum().getX();
  preMomY  =  thePrePoint->GetMomentum().getY();
  preMomZ  =  thePrePoint->GetMomentum().getZ();
  
  if(thePrePoint->GetMomentum().getR()>0){
    G4double prePhi = thePrePoint->GetMomentum().getPhi();
    G4double preTheta = thePrePoint->GetMomentum().getTheta();
    preAngX = atan2(sin(preTheta)*cos(prePhi),cos(preTheta)) * 180. / CLHEP::pi;
    preAngY = atan2(sin(preTheta)*sin(prePhi),cos(preTheta)) * 180. / CLHEP::pi;

    projPosX = prePosX + (MDposZ - prePosZ) * tan(preAngX * CLHEP::pi/180.);
    projPosY = prePosY + (MDposZ - prePosZ) * tan(preAngY * CLHEP::pi/180.);
    
  }

  /*fill histo*/
  double zpos=2.+2.1*unitNo;  
  if( fabs(projPosX)<30 && fabs(preAngX)<90 && unitNo<20 && unitNo>=0 &&
      trackID==1 && parentID==0 && material==1 && preE>=2 && fabs(prePosZ-zpos)<0.05 && pType==11)
    hPosAngUnit->Fill(projPosX,preAngX,unitNo);
  
  /*fill tree*/ 
  G4int fillTree=0;
  if(material==1 && fillTree){
    tout->Fill();
  }
}

void mscSteppingAction::InitVar(){
  eventNr = -999;
  material = -999;
  unitNo = -999;
  pType = -999;
  trackID = -999;
  parentID = -999;

  preE  = -999;
  preKE = -999;

  prePosX  = -999;
  prePosY  = -999;
  prePosZ  = -999;
  preMomX  = -999;
  preMomY  = -999;
  preMomZ  = -999;

  preAngX  = -999;
  preAngY  = -999;

  projPosX = -999;
  projPosY = -999;
  
}




