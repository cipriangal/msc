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

G4ThreeVector scatter(G4ThreeVector org, int pol,int stp);

void printVector(G4ThreeVector vec);
void rotateSpinToLocal(G4ThreeVector ki, G4ThreeVector kf, G4ThreeVector pi, G4ThreeVector &pf);
G4ThreeVector inverseRotateUz(G4ThreeVector v,G4ThreeVector dir);

int main(int argc, char** argv)
{
 
  G4double pi = acos(-1);

  G4double tstTh = 135./180. * pi;
  G4double tstPh = 315./180. * pi;

  G4ThreeVector iniMom(0,0,1);
  cout<<"Initial mom:"<<endl; printVector(iniMom);
  G4ThreeVector iniPol(0,1,0);
  cout<<"Initial pol:"<<endl; printVector(iniPol);
  G4ThreeVector finMom(sin(tstTh)*cos(tstPh),sin(tstTh)*sin(tstPh),cos(tstTh));
  cout<<"Final mom:"<<endl; printVector(finMom);

  G4ThreeVector finPol(0,1,0);
  rotateSpinToLocal(iniMom,finMom,iniPol,finPol);
  //rotateSpinToGlobal(iniMom,finMom,iniPol,finPol);
  //rotatePaul(iniMom,finMom,iniPol,finPol);
  cout<<"Final pol:"<<endl; printVector(finPol);

  //G4ThreeVector globalFinPol=inverseRotateUz(finPol,finMom);
  finPol.rotateUz(finMom);
  cout<<"global :"<<endl;
  printVector(finPol);

  
  cout<<"rotateUz :"<<endl;
  iniPol.rotateUz(finMom);
  printVector(iniPol);


  return 0;
  
  if( argc < 4 ) {
    cout<<" usage: build/moreAsym [# events] [#of asymmetry steps] [1 for pos pol: -1 for neg pol]"<<endl;
    cout<<"\t e.g. : build/moreAsym 100000 3 1"<<endl;
    return 1;
  }

  int nEvt=atoi(argv[1]);
  int nStp=atoi(argv[2]);
  int pol=atoi(argv[3]);
  
  TFile *fout=new TFile("o_moreAsym.root","RECREATE");
  TTree *t=new TTree("t","Asym toy MC output");
  double phi,th;
  int evNr,stpNr;
  t->Branch("evNr",&evNr,"evNr/I");
  t->Branch("stpNr",&stpNr,"stpNr/I");
  t->Branch("phi",&phi,"phi/D");
  t->Branch("th",&th,"th/D");

  vector<G4ThreeVector> prevStp,currStp;
  for(int iEv=0;iEv<nEvt;iEv++)
    currStp.push_back(G4ThreeVector(0,0,1));
  
  for(int iStp=0;iStp<nStp;iStp++){
    cout<<"at step "<<iStp<<endl;
    prevStp=currStp;

    for(int iEv=0;iEv<nEvt;iEv++){
      currStp[iEv]=scatter(prevStp[iEv],pol,iStp);
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

G4ThreeVector scatter(G4ThreeVector org, int pol, int stp){

  //cos(20/180*pi)
  double cth = 9.39692620785908428e-01;
  double sth = 3.42020143325668657e-01;

  TRandom3 a(0);
  double phi = a.Uniform() * CLHEP::twopi;
  double phiPol = phi - CLHEP::twopi/4;

  // G4double amplitude(0);
  // if(stp==0) amplitude = 0.4*pol;
  G4double amplitude = 0.4*pol;
  
  double _prob = a.Uniform();
  if( _prob < amplitude * sin(phiPol) ){
    phi-=CLHEP::pi;
  }

  double tdirx = sth*cos(phi);
  double tdiry = sth*sin(phi);    
  G4ThreeVector tnewDirection(tdirx,tdiry,cth);
  tnewDirection.rotateUz(org);
  return tnewDirection;
}

void printVector(G4ThreeVector vec){
  G4double pi=acos(-1);
  cout<<"\tX: "
      <<vec.getX()<<"\tY: "
      <<vec.getY()<<"\tZ: "
      <<vec.getZ()<<"\tR: "
      <<vec.getR()<<"\tTh: "
      <<vec.getTheta()/pi*180<<"\tPh: "
      <<vec.getPhi()/pi*180<<endl;
}



void rotateSpinToLocal(G4ThreeVector ki, G4ThreeVector kf, G4ThreeVector pi, G4ThreeVector &pf){
  G4StokesVector beamPol = pi;
  cout<<"l: initial beamPol "<<endl;
  printVector(beamPol);
  
  G4ThreeVector  nInteractionFrame = G4PolarizationHelper::GetFrame(ki,kf);
  cout<<"l: normal to interaction frame"<<endl;
  printVector(nInteractionFrame);
  cout<<"l: P*n "<<nInteractionFrame*beamPol<<endl;
  
  // transform polarization from ki-local into Stokes
  beamPol.RotateAz(nInteractionFrame,ki);
  cout<<"l: in stokes beamPol "<<endl;
  printVector(beamPol);

  //polarization transfer
  //G4double interactionTheta = ki*kf;
  //cout<<"l: rotation by "<<acos(interactionTheta)*180/3.1415<<endl;
  //beamPol.rotateY(acos(interactionTheta)); //this should be alpha=atan(U/T)
  //beamPol.rotateY(acos( - interactionTheta) + alpha); //this should be alpha=atan(U/T)
  //this should produce the same result <-- i think this is wrong because we are in another frame 
  //beamPol.rotate(acos(interactionTheta),nInteractionFrame);
  // cout<<"l: after transport beamPol "<<endl;
  // printVector(beamPol);

  //rotate from Stokes frame into kf-local
  beamPol.InvRotateAz(nInteractionFrame,kf);
  cout<<"l: back to local beamPol "<<endl;
  printVector(beamPol);

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
