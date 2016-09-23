#include <string>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <math.h>
#include <algorithm>

#include "G4ThreeVector.hh"
#include "G4PolarizationHelper.hh"
#include "G4StokesVector.hh"
#include "CLHEP/Units/PhysicalConstants.h"
#include "TFile.h"
#include "TTree.h"
#include "TRandom3.h"

using namespace std;

G4ThreeVector scatter(G4ThreeVector org, G4ThreeVector pol,G4ThreeVector &finPol);
void printVector(G4ThreeVector vec);
void rotateSpinToLocal(G4ThreeVector ki, G4ThreeVector kf, G4ThreeVector pi, G4ThreeVector &pf);
G4ThreeVector inverseRotateUz(G4ThreeVector v,G4ThreeVector dir);
int debugPrint(0);

int main(int argc, char** argv)
{
 
  if( argc < 4 ) {
    cout<<" usage: build/moreAsym [# events] [#of asymmetry steps] [1 for pos pol: -1 for neg pol]"<<endl;
    cout<<"\t e.g. : build/moreAsym 100000 3 1"<<endl;
    return 1;
  }

  int nEvt=atoi(argv[1]);
  int nStp=atoi(argv[2]);
  int pol=atoi(argv[3]);

  string fOutNm=Form("o_moreAsym_stp%d_",nStp);
  if(pol==1) fOutNm+="V.root";
  else if(pol==-1) fOutNm+="mV.root";
  else{
    cout<<"polarization should be either 1 or -1"<<endl;
    return 2;
  }
  
  TFile *fout=new TFile(fOutNm.c_str(),"RECREATE");
  TTree *t=new TTree("t","Asym toy MC output");
  double phi,th;
  int evNr,stpNr;
  t->Branch("evNr",&evNr,"evNr/I");
  t->Branch("stpNr",&stpNr,"stpNr/I");
  t->Branch("phi",&phi,"phi/D");
  t->Branch("th",&th,"th/D");
  // t->Branch("px",&px,"px/D");
  // t->Branch("py",&px,"py/D");
  // t->Branch("pz",&px,"pz/D");

  vector<G4ThreeVector> prevStp,currStp;
  vector<G4ThreeVector> prevStpPol,currStpPol;
  for(int iEv=0;iEv<nEvt;iEv++){
    currStp.push_back(G4ThreeVector(0,0,1));
    currStpPol.push_back(G4ThreeVector(0,pol,0));
  }
  
  for(int iStp=0;iStp<nStp;iStp++){
    cout<<"at step "<<iStp<<endl;
    prevStp=currStp;
    prevStpPol=currStpPol;
    
    for(int iEv=0;iEv<nEvt;iEv++){
      currStp[iEv]=scatter(prevStp[iEv],prevStpPol[iEv],currStpPol[iEv]);
      evNr  = iEv;
      stpNr = iStp;
      phi   = currStp[iEv].getPhi();
      th    = currStp[iEv].getTheta();
      t->Fill();
    }
  }

  t->Write();
  fout->Close();
  
  return 0;
  
}

G4ThreeVector scatter(G4ThreeVector org, G4ThreeVector pol, G4ThreeVector &finPol){

  //cos(20/180*pi)
  double cth = 9.39692620785908428e-01;
  double sth = 3.42020143325668657e-01;

  TRandom3 a(0);
  double phi = a.Uniform() * CLHEP::twopi;
  G4double amplitude = 0.2;  
  G4double _prob = a.Uniform();

  G4double tdirx1 = sth*cos(phi);
  G4double tdiry1 = sth*sin(phi);    
  G4ThreeVector tnewDirection1(tdirx1,tdiry1,cth);
  G4ThreeVector normalInteraction = G4PolarizationHelper::GetFrame(org,tnewDirection1);
  G4double cosPhi = pol * normalInteraction;
  
  if( _prob < amplitude * cosPhi ){
    phi-=CLHEP::pi;
  }

  G4double tdirx = sth*cos(phi);
  G4double tdiry = sth*sin(phi);    
  G4ThreeVector tnewDirection(tdirx,tdiry,cth);
  tnewDirection.rotateUz(org);

  rotateSpinToLocal(org,tnewDirection,pol,finPol);
  return tnewDirection;
}

void printVector(G4ThreeVector vec){
  cout<<"\tX: "
      <<vec.getX()<<"\tY: "
      <<vec.getY()<<"\tZ: "
      <<vec.getZ()<<"\tR: "
      <<vec.getR()<<"\tTh: "
      <<vec.getTheta()/CLHEP::pi*180<<"\tPh: "
      <<vec.getPhi()/CLHEP::pi*180<<endl;
}



void rotateSpinToLocal(G4ThreeVector ki, G4ThreeVector kf, G4ThreeVector pi, G4ThreeVector &pf){
  G4StokesVector beamPol = pi;
  if(debugPrint){
    cout<<"l: initial beamPol "<<endl;
    printVector(beamPol);
  }
  
  G4ThreeVector  nInteractionFrame = G4PolarizationHelper::GetFrame(ki,kf);
  if(debugPrint){
    cout<<"l: normal to interaction frame"<<endl;
    printVector(nInteractionFrame);
    cout<<"l: P*n "<<nInteractionFrame*beamPol<<endl;
  }
  
  // transform polarization from ki-local into Stokes
  beamPol.RotateAz(nInteractionFrame,ki);
  if(debugPrint){
    cout<<"l: in stokes beamPol "<<endl;
    printVector(beamPol);
  }

  //polarization transfer
  G4double interactionTheta = ki*kf;
  if(debugPrint)
    cout<<"l: rotation by "<<acos(interactionTheta)*180/CLHEP::pi<<endl;
  //beamPol.rotateY( - acos(interactionTheta) + alpha); //where alpha=atan(U/T)
  if(debugPrint){
    cout<<"l: after transport beamPol "<<endl;
    printVector(beamPol);
  }

  //rotate from Stokes frame into kf-local
  beamPol.InvRotateAz(nInteractionFrame,kf);
  if(debugPrint){
    cout<<"l: back to local beamPol "<<endl;
    printVector(beamPol);
  }
  
  pf = (G4ThreeVector)beamPol;
}

G4ThreeVector inverseRotateUz(G4ThreeVector v,G4ThreeVector dir){
  G4double nx(0),ny(0),nz(0);
  G4double ux= dir.getX();
  G4double uy= dir.getY();
  G4double uz= dir.getZ();
  G4double up= ux*ux + uy*uy;

  if (up > 0){
    up = sqrt(up);
    nx =  v.getX() * ux * uz / up - v.getY() * uy * uz / up - v.getZ() * up;
    ny = -v.getX() * uy / up      + v.getY() * ux / up                     ;
    nx =  v.getX() * uz           + v.getY() * uy           + v.getZ() * uz;
  }else if ( uz <0 ){
    nx = - v.getX();
    ny =   v.getY();
    nz = - v.getZ();
  }

  G4ThreeVector finalV(nx,ny,nz);
  return finalV;
}
