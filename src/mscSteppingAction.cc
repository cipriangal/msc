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

mscSteppingAction::mscSteppingAction(G4int *evN,std::vector<double> *asInfo):
  asymInfo(asInfo)
{
  //eventID pointer from the mscEventAction.cc file
  evNr=evN;
  nrUnits=10;
  writeANdata=0;
  writeTree=0;
  currentEv=-1;

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
			101,0,101);
    
    hdistAe[i]=new TH3I(Form("distAe_%d",i),Form("all e @ unit %d;pos [cm];angle [deg];E [MeV]",i),
			201,-100.5,100.5,
			180,-90,90,
			101,0,101);    
  }

  if(nrUnits<=0){
    hdistPe[0]=new TH3I("distPe","primaries @ MD ;pos [cm];angle [deg];E [MeV]",
			201,-100.5,100.5,
			180,-90,90,
			101,0,101);
    
    hdistAe[0]=new TH3I("distAe","all e @ MD;pos [cm];angle [deg];E [MeV]",
			201,-100.5,100.5,
			180,-90,90,
			101,0,101);    
  }
  
  if(writeTree){
    tout=new TTree("t","Stepping action event tree");
    
    tout->Branch("evNr",&eventNr,"evNr/I");
    tout->Branch("stpNr",&stpNr,"stpNr/I");
    tout->Branch("material",&material,"material/I");
    tout->Branch("unitNo",&unitNo,"unitNo/I");
    
    tout->Branch("stepSize", &stepSize, "stepSize/D");
    
    tout->Branch("pType",&pType,"pType/I");
    tout->Branch("trackID",&trackID,"trackID/I");
    tout->Branch("parentID",&parentID,"parentID/I");
    
    tout->Branch("polX", &polX, "polX/D");
    tout->Branch("polY", &polY, "polY/D");
    tout->Branch("polZ", &polZ, "polZ/D");

    tout->Branch("stpCth", &stpCth, "stpCth/D");
    tout->Branch("stpAN", &stpAN, "stpAN/D");
    tout->Branch("stpPhi", &stpPhi, "stpPhi/D");
    tout->Branch("stpPolPhi", &stpPolPhi, "stpPolPhi/D");
    tout->Branch("stpPhiRotated", &stpPhiRotated, "stpPhiRotated/D");

    tout->Branch("asymInfoPP", &asymInfoPP, "asymInfoPP/D");
    tout->Branch("asymInfoPM", &asymInfoPM, "asymInfoPM/D");
    
    tout->Branch("postE",&postE,"postE/D");
    tout->Branch("postKE",&postKE,"postKE/D");
    
    tout->Branch("postPosX", &postPosX, "postPosX/D");
    tout->Branch("postPosY", &postPosY, "postPosY/D");
    tout->Branch("postPosZ", &postPosZ, "postPosZ/D");
    tout->Branch("postMomX", &postMomX, "postMomX/D");
    tout->Branch("postMomY", &postMomY, "postMomY/D");
    tout->Branch("postMomZ", &postMomZ, "postMomZ/D");
    
    tout->Branch("postAngX",&postAngX,"postAngX/D");
    tout->Branch("postAngY",&postAngY,"postAngY/D");
    
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
  if(nrUnits<=0){
    hdistPe[0]->Write();
    hdistAe[0]->Write();
  }
  if(writeTree)
    tout->Write();
  fout->Close();
}


void mscSteppingAction::UserSteppingAction(const G4Step* theStep)
{
  static G4int stepNr;
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
  if(currentEv!=eventNr){
    savedTracks.clear();
    savedParents.clear();
    currentEv=eventNr;
  }

  
  if(theMaterial){
    
    if(theMaterial->GetName().compare("detectorMat")==0)       material=1;
    else if(theMaterial->GetName().compare("Aluminum")==0)     material=2;
    else if(theMaterial->GetName().compare("Tyvek")==0)        material=3;
    else if(theMaterial->GetName().compare("Lead")==0)         material=4;
    else if(theMaterial->GetName().compare("Quartz")==0)       material=5;
    else if(theMaterial->GetName().compare("SiElast_Glue")==0) material=6;
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

  polX  =  _polarization.getX();
  polY  =  _polarization.getY();
  polZ  =  _polarization.getZ();

  if(_polarization.getR()>0.001){
    asymInfoPP = asymInfo->at(0);
    asymInfoPM = asymInfo->at(1);
  }

  if(asymInfo->at(0) == -2)
    stepNr = 0;
  else
    stepNr++;
  stpNr = stepNr;
  
  pType = particleType->GetPDGEncoding();
  trackID = theStep->GetTrack()->GetTrackID();
  parentID = theStep->GetTrack()->GetParentID();
  
  postE  =  thePostPoint->GetTotalEnergy();
  postKE = thePostPoint->GetKineticEnergy();

  postPosX  =  thePostPoint->GetPosition().getX();
  postPosY  =  thePostPoint->GetPosition().getY();
  postPosZ  =  thePostPoint->GetPosition().getZ();
  postMomX  =  thePostPoint->GetMomentum().getX();
  postMomY  =  thePostPoint->GetMomentum().getY();
  postMomZ  =  thePostPoint->GetMomentum().getZ();
  
  if(thePostPoint->GetMomentum().getR()>0){
    G4double postPhi = thePostPoint->GetMomentum().getPhi();
    G4double postTheta = thePostPoint->GetMomentum().getTheta();
    postAngX = atan2(sin(postTheta)*cos(postPhi),cos(postTheta)) * 180. / CLHEP::pi;
    postAngY = atan2(sin(postTheta)*sin(postPhi),cos(postTheta)) * 180. / CLHEP::pi;

    const G4double MDposZ = 5 * CLHEP::cm;
    projPosX = postPosX + (MDposZ - postPosZ) * tan(postAngX * CLHEP::pi/180.);
    projPosY = postPosY + (MDposZ - postPosZ) * tan(postAngY * CLHEP::pi/180.);
  }

  stepSize=theStep->GetStepLength();
  
  /*fill histo*/
  if(unitNo!=-999){
    if(unitNo>=MaxNrUnits || unitNo<0){
      G4cerr<<__PRETTY_FUNCTION__<<":"<<G4endl;
      G4cerr<<" you have too many segmentation units defined "<<unitNo<<" max="<<MaxNrUnits<<G4endl;
      exit(1);
    }
    
    G4double histE = (postE>100) ? 100.5 : postE;
    if( fabs(projPosX/10.)<100 && fabs(postAngX)<90 && abs(pType)==11 && material==1 ){
      hdistAe[unitNo]->Fill(projPosX/10.,postAngX,histE);
      if(trackID==1 && parentID==0)
	hdistPe[unitNo]->Fill(projPosX/10.,postAngX,histE);
    }
  }

  if(nrUnits==-1){
    G4double histE = (postE>100) ? 100.5 : postE;
    if( fabs(postPosX/10.)<100 && fabs(postAngX)<90 && abs(pType)==11 &&
	material==5 && recordTrack(trackID,parentID) ){
      hdistAe[0]->Fill(postPosX/10.,postAngX,histE);
      if(trackID==1 && parentID==0)
	hdistPe[0]->Fill(postPosX/10.,postAngX,histE);
    }
  }

  stpCth = asymInfo->at(4);
  stpPhi = asymInfo->at(5);
  stpPolPhi = asymInfo->at(6);
  stpPhiRotated = asymInfo->at(7);
  stpAN = asymInfo->at(8);

  // G4cout<<__LINE__<<" "<<__PRETTY_FUNCTION__<<G4endl;
  // G4cout<<postE<<"\t"<<stpCth<<"\t"<<stpPhi<<"\t"<<stpPolPhi<<"\t"<<stpAN<<"\t"
  // 	<<1+stpAN*sqrt(polX*polX+polY*polY)*sin(stpPolPhi)<<"\t"<<asymInfoPP<<G4endl;
  // std::cin.ignore();
  /*fill tree*/ 
  if(writeTree==1 && material==1) tout->Fill();
  else if(writeTree==2) tout->Fill();

}

void mscSteppingAction::InitVar(){
  eventNr = -999;
  stpNr = -999;
  material = -999;
  unitNo = -999;
  pType = -999;
  trackID = -999;
  parentID = -999;

  polX  = -999;
  polY  = -999;
  polZ  = -999;

  stpCth = -999;
  stpPhi = -999;
  stpAN  = -999;
  stpPolPhi = -999;
  stpPhiRotated = -999;

  asymInfoPP  = -999;
  asymInfoPM  = -999;

  postE  = -999;
  postKE = -999;

  postPosX  = -999;
  postPosY  = -999;
  postPosZ  = -999;
  postMomX  = -999;
  postMomY  = -999;
  postMomZ  = -999;

  postAngX  = -999;
  postAngY  = -999;

  projPosX = -999;
  projPosY = -999;
  stepSize = -999;
}

G4int mscSteppingAction::recordTrack(G4int trID, G4int parID){
  for(int i=0;i<int(savedTracks.size());i++)
    if(trID==savedTracks[i] && parID==savedParents[i])
      return 0;
  savedTracks.push_back(trID);
  savedParents.push_back(parID);
  return 1;
}


