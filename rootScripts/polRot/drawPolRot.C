#include <G4StokesVector.hh>
#include <G4ThreeVector.hh>

void drawPolRot(){

  gSystem->Load("drawVector.C");

  drawAxes();

  G4ThreeVector k1(0,0,1);
  cout<<k1.getTheta()<<endl;
  
  TPolyLine3D *k1=(TPolyLine3D*)createVector(0,0,0, 0,0,1,  2,5,1,"k1");
  k1->Draw();
  TPolyLine3D *p1=(TPolyLine3D*)createVector(0,0,0, 0,1,0,  5,5,1,"k1");
  p1->Draw();
  
  TPolyLine3D *k2=(TPolyLine3D*)createVector(0,0,1, 0,1,0,  4,4,4,"k2");
  k2->Draw();
  TPolyLine3D *p2=(TPolyLine3D*)createVector(0,0,1, 0,0,-1, 5,4,4,"k2");
  p2->Draw();

}



void rotateSpinToLocal(G4ThreeVector ki, G4ThreeVector kf, G4ThreeVector pi, G4ThreeVector &pf){
  G4StokesVector beamPol = pi;
  
  //G4ThreeVector  nInteractionFrame = G4PolarizationHelper::GetFrame(ki,kf);
  G4ThreeVector  nInteractionFrame = (ki.cross(kf)).unit();
  
  // transform polarization from ki-local into Stokes
  beamPol.RotateAz(nInteractionFrame,ki);

  //polarization transfer -- DO NOTHING

  //rotate from Stokes frame into kf-local
  beamPol.InvRotateAz(nInteractionFrame,kf);
  
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
