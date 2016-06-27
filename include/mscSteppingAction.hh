#ifndef mscSteppingAction_hh
#define mscSteppingAction_hh 1

#include "G4UserSteppingAction.hh"
#include "globals.hh"

#include "TFile.h"
#include "TH3I.h"
#include "TTree.h"
#include "TGraph.h"

/*
  Brem depolarization: PhysRev.114.887
  implemented only for transverse polarization
  will decrease the overall polarization by a functional form from figure 5 in the paper      
*/
static const int perpNval=128;
static const double perpXDepol[perpNval]={0.053,0.066,0.075,0.084,0.097,0.112,0.126,0.137,0.148,0.160,0.169,0.180,0.190,0.200,0.210,0.220,0.226,0.236,0.244,0.251,0.258,0.267,0.274,0.283,0.291,0.300,0.308,0.316,0.324,0.332,0.340,0.350,0.357,0.364,0.371,0.379,0.387,0.395,0.403,0.411,0.417,0.425,0.434,0.443,0.453,0.460,0.468,0.477,0.488,0.499,0.509,0.518,0.525,0.532,0.541,0.548,0.555,0.563,0.570,0.576,0.581,0.587,0.594,0.600,0.608,0.616,0.623,0.628,0.635,0.640,0.646,0.653,0.661,0.668,0.676,0.683,0.688,0.694,0.699,0.706,0.709,0.715,0.722,0.728,0.734,0.738,0.745,0.749,0.752,0.757,0.762,0.769,0.776,0.782,0.789,0.795,0.799,0.806,0.810,0.815,0.824,0.830,0.836,0.843,0.847,0.853,0.858,0.864,0.868,0.874,0.881,0.887,0.893,0.900,0.908,0.915,0.920,0.930,0.935,0.945,0.953,0.963,0.969,0.974,0.979,0.985,0.990,0.994};
static const double perpYDepol[perpNval]={0.203,0.277,0.399,0.522,0.764,0.890,1.250,1.608,1.966,2.208,2.565,2.924,3.281,3.639,4.114,4.472,4.945,5.421,5.895,6.251,6.485,6.960,7.556,8.030,8.504,9.097,9.671,10.145,10.620,11.094,11.804,12.396,12.988,13.579,14.161,14.871,15.580,16.407,17.234,17.709,18.417,19.127,20.072,21.021,21.968,22.794,23.744,24.689,25.871,27.288,28.116,29.179,30.006,30.838,31.901,32.968,34.030,34.975,35.920,36.746,37.455,38.164,38.874,39.935,40.998,41.944,43.007,43.933,44.878,45.822,46.884,47.770,48.951,50.132,51.195,52.376,53.437,54.265,55.096,56.041,56.867,57.694,58.757,59.584,60.529,61.573,62.519,63.227,63.935,64.644,65.472,66.653,67.599,68.662,69.725,70.788,71.615,72.449,73.275,74.107,75.290,76.359,77.304,78.251,78.959,79.906,80.733,81.679,82.506,83.334,84.281,85.109,86.055,87.120,88.068,89.015,89.961,91.028,91.862,92.929,94.000,95.421,96.132,96.725,97.436,98.387,98.862,99.219};


class mscSteppingAction : public G4UserSteppingAction
{
public:
  mscSteppingAction(G4int*,std::vector<double> *);
  ~mscSteppingAction();
  
  void UserSteppingAction(const G4Step*);
  void InitVar();
  void InitOutput();
  
  void SetNrUnits(G4int val){nrUnits=val;}
  void SetWriteANdata(G4int val){writeANdata=val;}
  void SetWriteTree(G4int val){writeTree=val;}
  
private:
  G4int *evNr;
  std::vector< double >* asymInfo;
  
  TFile *fout;
  TTree *tout;

  G4int currentEv;
  std::vector<G4int> savedTracks;
  std::vector<G4int> savedParents;
  G4int recordTrack(G4int trID, G4int parID);
  
  G4int nrUnits;
  G4int writeANdata;
  G4int writeTree;
  TGraph perpDepol;

  static const G4int MaxNrUnits=15;
  TH3I *hdistPe[MaxNrUnits];//pos, ang, E
  TH3I *hdistAe[MaxNrUnits];

  //tree variables
  G4int eventNr;
  G4int material; //PBA:0 ; detectorMat:1
  G4int unitNo;
  G4int pType;    
  G4int trackID;
  G4int parentID;

  G4double  preE; // total energy
  G4double  preKE;// kinetic energy

  G4double  prePosX; //position
  G4double  prePosY;
  G4double  prePosZ;
  G4double  preMomX; //momentum
  G4double  preMomY;
  G4double  preMomZ;  
  
  G4double preAngX; //angle along x (long section of the detector)
  G4double preAngY; //angle along y (short section of the detector)

  G4double  projPosX; //position at MD face (z=5 cm)
  G4double  projPosY;
  G4double  stepSize;

};

#endif
