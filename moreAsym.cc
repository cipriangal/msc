#include <string>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <math.h>
#include <algorithm>

#include "G4ThreeVector.hh"
#include "CLHEP/Units/PhysicalConstants.h"
#include "TFile.h"
#include "TTree.h"
#include "TRandom3.h"

using namespace std;

G4ThreeVector scatter(G4ThreeVector org, int pol);

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
      currStp[iEv]=scatter(prevStp[iEv],pol);
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

G4ThreeVector scatter(G4ThreeVector org, int pol){

  //cos(20/180*pi)
  double cth = 9.39692620785908428e-01;
  double sth = 3.42020143325668657e-01;

  TRandom3 a(0);
  double phi = a.Uniform() * CLHEP::twopi;
  double phiPol = phi - CLHEP::twopi/4;
  
  G4double amplitude = 0.1*pol;
  
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
