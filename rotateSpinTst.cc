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

void printVector(G4ThreeVector vec);
void rotateSpinToLocal(G4ThreeVector ki, G4ThreeVector kf, G4ThreeVector pi, G4ThreeVector &pf);
void polTransferG4brem(G4ThreeVector ki, G4ThreeVector kf, G4ThreeVector pi, G4ThreeVector &pf);
G4ThreeVector inverseRotateUz(G4ThreeVector v,G4ThreeVector dir);

int debugPrint(1);

int main(int argc, char** argv)
{
  G4double dphi,dthe;
  if(argc>1){
    dphi = atof(argv[1]) * CLHEP::deg;
    dthe = atof(argv[2]) * CLHEP::deg;
    cout<<"input: settting phi to "<<dphi<<" and theta to "<<dthe<<endl;
  }else{
    dphi = 135 * CLHEP::deg;
    dthe = 0.05 * CLHEP::deg;
    cout<<"no input: settting phi to "<<dphi<<" and theta to "<<dthe<<endl;
  }
  G4ThreeVector momF(cos(dphi)*sin(dthe),sin(dphi)*sin(dthe),cos(dthe));
  G4ThreeVector momI(0,0,1);
  G4ThreeVector polI(0,1,0);
  G4ThreeVector polF(0,0,0);


  cout<<endl<<"~~~~~~~~~~~~~~~~~ Test based on initial PK/CG tests:"<<endl;
  cout<<"initial mom"<<endl;printVector(momI);
  cout<<"final   mom"<<endl;printVector(momF);
  cout<<"initial pol"<<endl;printVector(polI);
  rotateSpinToLocal(momI,momF,polI,polF);
  cout<<"initial mom"<<endl;printVector(momI);
  cout<<"final   mom"<<endl;printVector(momF);
  cout<<"initial pol"<<endl;printVector(polI);
  cout<<"final   pol"<<endl;printVector(polF);
  G4ThreeVector polFv1 = inverseRotateUz(polF,momF);
  cout<<"fin pol inverse rotateUz"<<endl;printVector(polFv1);
  G4ThreeVector polFv2 = polF;
  polFv2.rotateUz(momF);
  cout<<"fin pol rotateUz"<<endl;printVector(polFv2);
  G4ThreeVector polFv3 = inverseRotateUz(polFv2,momF);
  cout<<"fin pol InvRotateUz(rotateUz)"<<endl;printVector(polFv3);
  
  // G4StokesVector polFv3 = polF;
  // polFv3.RotateAz(
  
  cout<<endl<<endl<<"~~~~~~~~~~~~~~~~~ Test based on G4 brem:"<<endl;
  cout<<"initial mom"<<endl;printVector(momI);
  cout<<"final   mom"<<endl;printVector(momF);
  cout<<"initial pol"<<endl;printVector(polI);
  polTransferG4brem(momI,momF,polI,polF);
  cout<<"initial mom"<<endl;printVector(momI);
  cout<<"final   mom"<<endl;printVector(momF);
  cout<<"initial pol"<<endl;printVector(polI);
  cout<<"final   pol"<<endl;printVector(polF);
  // polFv1 = inverseRotateUz(polF,momF);
  // cout<<"fin pol inverse rotateUz"<<endl;printVector(polFv1);
  polFv2 = polF;
  polFv2.rotateUz(momF);
  cout<<"fin pol rotateUz"<<endl;printVector(polFv2);

  return 0;
  
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

void polTransferG4brem(G4ThreeVector ki, G4ThreeVector kf, G4ThreeVector pi, G4ThreeVector &pf){
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
  beamPol.InvRotateAz(nInteractionFrame,ki);
  if(debugPrint){
    cout<<"l: in stokes beamPol "<<endl;
    printVector(beamPol);
  }

  //polarization transfer
  // G4double interactionTheta = ki*kf;
  // if(debugPrint)
  //   cout<<"l: rotation by "<<acos(interactionTheta)*180/CLHEP::pi<<endl;
  //beamPol.rotateY( - acos(interactionTheta) + alpha); //where alpha=atan(U/T)

  if(debugPrint){
    cout<<"l: after transport beamPol "<<endl;
    printVector(beamPol);
  }

  //rotate from Stokes frame into kf-local
  beamPol.RotateAz(nInteractionFrame,kf);
  if(debugPrint){
    cout<<"l: back to local beamPol "<<endl;
    printVector(beamPol);
  }
  
  pf = (G4ThreeVector)beamPol;  
}


void rotateSpinToLocal(G4ThreeVector ki, G4ThreeVector kf, G4ThreeVector pi, G4ThreeVector &pf){
  G4StokesVector beamPol = pi;
  if(debugPrint){
    cout<<"l: initial beamPol "<<endl;
    printVector(beamPol);
  }
  
  //G4ThreeVector  nInteractionFrame = G4PolarizationHelper::GetFrame(ki,kf);
  G4ThreeVector  nInteractionFrame = (ki.cross(kf)).unit();
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

  G4ThreeVector partFrameY= G4PolarizationHelper::GetParticleFrameY(kf);
  cout<<"l: particle kf yFrame: "<<endl; printVector(partFrameY);
  G4double cosphi=partFrameY*nInteractionFrame;
  cout<<"l: cosphi "<<cosphi<<endl;
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
    double px= v.getX();
    double py= v.getY();
    double pz= v.getZ();
    nx =  px * ux * uz / up + py * uy * uz / up - pz * up;
    ny = -px * uy / up      + py * ux / up               ;
    nz =  px * ux           + py * uy           + pz * uz;
  }else if ( uz < 0 ){
    nx = - v.getX();
    ny =   v.getY();
    nz = - v.getZ();
  }

  G4ThreeVector finalV(nx,ny,nz);
  return finalV;
}
