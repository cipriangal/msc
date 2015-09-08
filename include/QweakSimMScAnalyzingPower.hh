#ifndef QweaSimAnalyzingPower_h
#define QweaSimAnalyzingPower_h

#include "TGraph.h"
#include "math.h"

#include "MScMottIn.hh" //this is where all the mott data is 

extern const double motX3[606];
extern const double motY3[606];
extern const double motX5[606];
extern const double motY5[606];
extern const double motX6[606];
extern const double motY6[606];
extern const double motX8[606];
extern const double motY8[606];


inline G4double Mott(G4double energy, G4double theta);
inline G4double AnalyzingPower(G4double energy, G4double cth){

  G4double power(0);
  G4double theta = acos(cth);
  
  G4double ahat = -22e-6 * 207./82.;
  G4double twoPhoton = ahat * sqrt(4.*pow(energy/1000.,2) * sin(theta/2.) );
  twoPhoton *= 1000.;
  if( fabs(twoPhoton) > 1 ) twoPhoton = 1. * twoPhoton/fabs(twoPhoton);

  G4double mott = Mott(energy,theta/CLHEP::pi *180.) * 100.;
  if( fabs(mott) > 1 ) mott = 1. * mott/fabs(mott);

  // power = twoPhoton;
  // power = mott + twoPhoton;
  power = mott;
  
  return power;
}

inline G4double Mott(G4double energy, G4double theta){

  //we should not extrapolate at the cross over point
  if(fabs(theta-30)<1 && energy>8)
    return 0;
  
  TGraph *c[4];
  for(int i=0;i<4;i++) c[i]=new TGraph();
  
  const int nEntries2=606;

  for(int i=0;i<nEntries2;i++){
    c[0]->SetPoint(i,motX3[i],motY3[i]);
    c[1]->SetPoint(i,motX5[i],motY5[i]);
    c[2]->SetPoint(i,motX6[i],motY6[i]);
    c[3]->SetPoint(i,motX8[i],motY8[i]);
  }
  
  TGraph *a=new TGraph();
  const G4double oneE[4]={3,5,6,8};

  G4double sgn(0);
  for(int i=0;i<4;i++){
    G4double eval=c[i]->Eval(theta,0,"");
    sgn+=eval;
  }
  if(fabs(sgn)>0)
    sgn/=fabs(sgn);
  
  for(int i=0;i<4;i++){
    a->SetPoint(i,oneE[i],c[i]->Eval(theta,0,""));
  }

  G4double mott = a->Eval(energy,0,"");
  
  if( mott*sgn <= 0 )
    mott=0.;

  for(int i=0;i<4;i++) delete c[i];
  delete a;
  
  return mott;
}


#endif
