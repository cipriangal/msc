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
#include "TApplication.h"
#include "TPolyLine3D.h"
#include "TCanvas.h"

using namespace std;

void printVector(G4ThreeVector vec);
void rotateSpinToLocal(G4ThreeVector ki, G4ThreeVector kf, G4ThreeVector pi, G4ThreeVector &pf);
void polTransferG4brem(G4ThreeVector ki, G4ThreeVector kf, G4ThreeVector pi, G4ThreeVector &pf);
G4ThreeVector inverseRotateUz(G4ThreeVector v,G4ThreeVector dir);

void drawAxes(double xi, double yi, double zi, double xx, double xy, double xz, double yx, double yy, double yz,double zx,double zy, double zz);
TPolyLine3D *createVector(double xi, double yi, double zi, double xf, double yf, double zf,
			  int color, double thickness, int styleVal);

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
  // G4ThreeVector polFv3 = inverseRotateUz(polFv2,momF);
  // cout<<"fin pol InvRotateUz(rotateUz)"<<endl;printVector(polFv3); // verify that rotation(inverse) does nothing
  

  TApplication theApp("drawVectors",&argc,argv);
  TCanvas *c1=new TCanvas("c1","c1");

  drawAxes(0,0,0,
	   3,0,0,
	   0,3,0,
	   0,0,3);
  TPolyLine3D *k1=createVector(0,0,0, momI.getX(),momI.getY(),momI.getZ(), 2,5,1);
  TPolyLine3D *p1=createVector(0,0,0, polI.getX(),polI.getY(),polI.getZ(), 5,5,1);
  
  TPolyLine3D *k2=createVector(momI.getX(),momI.getY(),momI.getZ(), momF.getX(),momF.getY(),momF.getZ(), 2,4,2);
  TPolyLine3D *p2=createVector(momI.getX(),momI.getY(),momI.getZ(), polF.getX(),polF.getY(),polF.getZ(), 3,4,4); //local - blue
  TPolyLine3D *p2r=createVector(momI.getX(),momI.getY(),momI.getZ(), polFv2.getX(),polFv2.getY(),polFv2.getZ(), 6,4,2); //global - magenta ---> the only one correct?!?
  TPolyLine3D *p2i=createVector(momI.getX(),momI.getY(),momI.getZ(), polFv1.getX(),polFv1.getY(),polFv1.getZ(), 4,3,3); //inverseRotateUz momF - green

  G4ThreeVector xAx(2,0,0);
  G4ThreeVector yAx(0,2,0);
  G4ThreeVector zAx(0,0,2);

  xAx.rotateUz(momF);
  yAx.rotateUz(momF);
  zAx.rotateUz(momF);
  drawAxes(momI.getX(),momI.getY(),momI.getZ(),
  	   xAx.getX(),xAx.getY(),xAx.getZ(),
  	   yAx.getX(),yAx.getY(),yAx.getZ(),
  	   zAx.getX(),zAx.getY(),zAx.getZ());

  G4ThreeVector momF2(cos(dphi)*sin(dthe),sin(dphi)*sin(dthe),cos(dthe));
  momF2.rotateUz(momF); //global coordinates

  G4ThreeVector polI2(0,1,0);
  polI2.rotateUz(momF); //global coordinates
  
  G4ThreeVector polF2(0,0,1);
  
  cout<<endl<<endl<<endl<<"initial mom"<<endl;printVector(momI);
  cout<<"final   mom"<<endl;printVector(momF);
  cout<<"initial pol"<<endl;printVector(polF);
  rotateSpinToLocal(momI,momF,polF,polF2);
  cout<<"initial mom"<<endl;printVector(momI);
  cout<<"final   mom"<<endl;printVector(momF);
  cout<<"initial pol"<<endl;printVector(polF);
  cout<<"final   pol"<<endl;printVector(polF2);
  //G4ThreeVector polF2v1 = inverseRotateUz(polF,momF);
  // cout<<"fin pol inverse rotateUz"<<endl;printVector(polFv1);
  G4ThreeVector polF2v2 = polF2;
  G4ThreeVector momFG2=momF; momFG2.rotateUz(momF);
  polF2v2.rotateUz(momFG2);
  cout<<"fin pol rotateUz"<<endl;printVector(polF2v2);
  
  TPolyLine3D *k3=createVector(momF.getX()+momI.getX(),momF.getY()+momI.getY(),momF.getZ()+momI.getZ(), momF2.getX(),momF2.getY(),momF2.getZ(), 2,3,4);
  //TPolyLine3D *p3=createVector(momI.getX(),momI.getY(),momI.getZ(), polF.getX(),polF.getY(),polF.getZ(), 3,4,4); //local - light blue
  TPolyLine3D *p3=createVector(momF.getX()+momI.getX(),momF.getY()+momI.getY(),momF.getZ()+momI.getZ(), polF2.getX(),polF2.getY(),polF2.getZ(), 3,3,4); //
  TPolyLine3D *p3r=createVector(momF.getX()+momI.getX(),momF.getY()+momI.getY(),momF.getZ()+momI.getZ(), polF2v2.getX(),polF2v2.getY(),polF2v2.getZ(), 6,3,3); //global - magenta ---> the only one correct?!?
  //  TPolyLine3D *p2i=createVector(momI.getX(),momI.getY(),momI.getZ(), polFv1.getX(),polFv1.getY(),polFv1.getZ(), 4,3,3); //inverseRotateUz momF - green

  G4ThreeVector xAx3(1.5,0,0);
  G4ThreeVector yAx3(0,1.5,0);
  G4ThreeVector zAx3(0,0,1.5);

  xAx3.rotateUz(momFG2);
  yAx3.rotateUz(momFG2);
  zAx3.rotateUz(momFG2);
  drawAxes(momI.getX()+momF.getX(),momI.getY()+momF.getY(),momI.getZ()+momF.getZ(),
  	   xAx3.getX(),xAx3.getY(),xAx3.getZ(),
  	   yAx3.getX(),yAx3.getY(),yAx3.getZ(),
  	   zAx3.getX(),zAx3.getY(),zAx3.getZ());

  
  // G4ThreeVector xAr=inverseRotateUz(xAx,momF);
  // G4ThreeVector yAr=inverseRotateUz(yAx,momF);
  // G4ThreeVector zAr=inverseRotateUz(zAx,momF);
  // drawAxes(momI.getX(),momI.getY(),momI.getZ(),
  // 	   xAr.getX(),xAr.getY(),xAr.getZ(),
  // 	   yAr.getX(),yAr.getY(),yAr.getZ(),
  // 	   zAr.getX(),zAr.getY(),zAr.getZ());

  k1->Draw();
  p1->Draw();
  k2->Draw();
  //p2->Draw();
  k3->Draw();
  //p3->Draw();
  p3r->Draw();
  p2r->Draw();
  //p2i->Draw();
  
  theApp.Run();
  
  // cout<<endl<<endl<<"~~~~~~~~~~~~~~~~~ Test based on G4 brem:"<<endl;
  // cout<<"initial mom"<<endl;printVector(momI);
  // cout<<"final   mom"<<endl;printVector(momF);
  // cout<<"initial pol"<<endl;printVector(polI);
  // polTransferG4brem(momI,momF,polI,polF);
  // cout<<"initial mom"<<endl;printVector(momI);
  // cout<<"final   mom"<<endl;printVector(momF);
  // cout<<"initial pol"<<endl;printVector(polI);
  // cout<<"final   pol"<<endl;printVector(polF);
  // // polFv1 = inverseRotateUz(polF,momF);
  // // cout<<"fin pol inverse rotateUz"<<endl;printVector(polFv1);
  // polFv2 = polF;
  // polFv2.rotateUz(momF);
  // cout<<"fin pol rotateUz"<<endl;printVector(polFv2);

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

void drawAxes(double xi,double yi, double zi,
	      double xx, double xy, double xz,
	      double yx, double yy, double yz,
	      double zx, double zy, double zz){
  TPolyLine3D *xAxis = new TPolyLine3D(2);
  xAxis->SetPoint(0, xi, yi, zi);
  xAxis->SetPoint(1, xi+xx, yi+xy, zi+xz);
  
  TPolyLine3D *yAxis = new TPolyLine3D(2);
  yAxis->SetPoint(0, xi, yi, zi);
  yAxis->SetPoint(1, xi+yx, yi+yy, zi+yz);
  
  TPolyLine3D *zAxis = new TPolyLine3D(2);
  zAxis->SetPoint(0, xi, yi, zi);
  zAxis->SetPoint(1, xi+zx, yi+zy, zi+zz);

  TPolyLine3D *xAxisDot = new TPolyLine3D(2);
  xAxisDot->SetPoint(0, xi, yi, zi);
  xAxisDot->SetPoint(1, xi-xx, yi-xy, zi-xz);
  
  TPolyLine3D *yAxisDot = new TPolyLine3D(2);
  yAxisDot->SetPoint(0, xi, yi, zi);
  yAxisDot->SetPoint(1, xi-yx, yi-yy, zi-yz);
  
  TPolyLine3D *zAxisDot = new TPolyLine3D(2);
  zAxisDot->SetPoint(0, xi, yi, zi);
  zAxisDot->SetPoint(1, xi-zx, yi-zy, zi-zz);

  xAxis->SetLineWidth(3);
  xAxis->SetLineColor(41);
  yAxis->SetLineWidth(3);
  yAxis->SetLineColor(15);
  zAxis->SetLineWidth(3);
  zAxis->SetLineColor(46);

  xAxisDot->SetLineWidth(3);
  xAxisDot->SetLineColor(41);
  xAxisDot->SetLineStyle(5);
  yAxisDot->SetLineWidth(3);
  yAxisDot->SetLineColor(15);
  yAxisDot->SetLineStyle(5);
  zAxisDot->SetLineWidth(3);
  zAxisDot->SetLineColor(46);
  zAxisDot->SetLineStyle(5);

  xAxis->Draw();
  yAxis->Draw();
  zAxis->Draw();
  xAxisDot->Draw();
  yAxisDot->Draw();
  zAxisDot->Draw();    
}

TPolyLine3D *createVector(double xi, double yi, double zi, double xf, double yf, double zf,
			  int color, double thickness, int styleVal){
  TPolyLine3D *aVector = new TPolyLine3D(2);
  //aVector->SetName(nm.c_str());
  aVector->SetPoint(0, xi, yi, zi);
  aVector->SetPoint(1, xi+xf, yi+yf, zi+zf);
  aVector->SetLineWidth(thickness);
  aVector->SetLineColor(color);
  aVector->SetLineStyle(styleVal);

  return aVector;
}
