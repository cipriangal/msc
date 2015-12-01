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

mscSteppingAction::mscSteppingAction(G4int *evN)
{
  //eventID pointer from the mscEventAction.cc file
  evNr=evN;
  nrUnits=10;
  writeANdata=0;
  writeTree=0;

  for(int i=0;i<perpNval;i++) perpDepol.SetPoint(i,perpXDepol[i],perpYDepol[i]);   
}

void  mscSteppingAction::InitOutput(){

  if(writeANdata){
    std::ofstream ofs;
    ofs.open("o_msc_ANdata.txt",std::ofstream::out);
    ofs<<"energy[MeV] cos(theta) anaPower polarization"<<G4endl;
    ofs.close();
  }
  
  
  /*Create root file and initialize what I want to put in it*/
  fout=new TFile("o_msc.root","RECREATE");

  for(int i=0;i<nrUnits;i++){
    hdistPe[i]=new TH3I(Form("distPe_%d",i),Form("primaries @ unit %d;pos [cm];angle [deg];E [MeV]",i),
			201,-100.5,100.5,
			180,-90,90,
			301,0,301);
    
    hdistAe[i]=new TH3I(Form("distAe_%d",i),Form("all e @ unit %d;pos [cm];angle [deg];E [MeV]",i),
			201,-100.5,100.5,
			180,-90,90,
			301,0,301);    
  }

  if(nrUnits==0){
    hdistPe[0]=new TH3I("distPe","primaries @ MD ;pos [cm];angle [deg];E [MeV]",
			201,-100.5,100.5,
			180,-90,90,
			301,0,301);
    
    hdistAe[0]=new TH3I("distAe","all e @ MD;pos [cm];angle [deg];E [MeV]",
			201,-100.5,100.5,
			180,-90,90,
			301,0,301);    
  }
  
  if(writeTree){
    tout=new TTree("t","Stepping action event tree");
    
    tout->Branch("evNr",&eventNr,"evNr/I");
    tout->Branch("material",&material,"material/I");
    tout->Branch("unitNo",&unitNo,"unitNo/I");
    
    tout->Branch("stepSize", &stepSize, "stepSize/D");
    
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
}


mscSteppingAction::~mscSteppingAction()
{
  /*Write out root file*/
  fout->cd();
  for(int i=0;i<nrUnits;i++){
    hdistPe[i]->Write();
    hdistAe[i]->Write();
  }
  if(nrUnits==0){
    hdistPe[0]->Write();
    hdistAe[0]->Write();
  }
  if(writeTree)
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

  eventNr=*evNr;
  
  if(theMaterial){    
    if(theMaterial->GetName().compare("detectorMat")==0)
      material=1;
    else if(theMaterial->GetName().compare("PBA")==0){
      material=0;

      eLossPercent = 1. - thePostPoint->GetKineticEnergy()/thePrePoint->GetKineticEnergy();

      if(_pn.compare("eBrem")==0 &&                           // only for eBrem
	 (fabs(_polarization.getX())>0 || fabs(_polarization.getY())>0)){ // only for transverse polarization
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

    const G4double MDposZ = 5 * CLHEP::cm;
    projPosX = prePosX + (MDposZ - prePosZ) * tan(preAngX * CLHEP::pi/180.);
    projPosY = prePosY + (MDposZ - prePosZ) * tan(preAngY * CLHEP::pi/180.);
  }

  stepSize=theStep->GetStepLength();
  
  /*fill histo*/
  if(unitNo!=-999){
    if(unitNo>=MaxNrUnits || unitNo<0){
      G4cerr<<__PRETTY_FUNCTION__<<":"<<G4endl;
      G4cerr<<" you have too many segmentation units defined "<<unitNo<<" max="<<MaxNrUnits<<G4endl;
      exit(1);
    }
    
    G4double histE = (preE>300) ? 300.5 : preE;
    if( fabs(projPosX/10.)<100 && fabs(preAngX)<90 && material==1 && pType==11){
      hdistAe[unitNo]->Fill(projPosX/10.,preAngX,histE);
      if(trackID==1 && parentID==0)
	hdistPe[unitNo]->Fill(projPosX/10.,preAngX,histE);
    }
  }
  
  /*fill tree*/ 
  if(writeTree==1 && material==1) tout->Fill();
  else if(writeTree==2) tout->Fill();

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
  stepSize = -999;
}




