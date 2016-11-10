#include <string>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <math.h>
#include <algorithm>

#include "TH1D.h"
#include "TH2D.h"
#include "TGraphErrors.h"
#include "TFile.h"
#include "TTree.h"

using namespace std;

void processTree(TTree *t, TH1D *dE,TH1D *zE[20]);
void analyze();
void findInt(std::vector<int> &inter,std::vector<int> &val,
	     int trackID,int parent, int &hasPar, int &nInt);

const int nbin=20;
TH1D *pAsym,*dAsym,*dPang,*dMang;
TH1D *zA[nbin],*zP[nbin],*zM[nbin];

int main(int argc, char** argv)
{

  if( argc <2 ) {
    cout<<" usage: build/anaWght [path to V infile with tree]  [path to mV infile with tree]"<<endl;
    return 1;
  }

  string fileV(argv[1]);  
  string fileM(argv[2]);  
  
  cout<<"Analyzing "<<fileV.c_str()<<" "<<fileM.c_str()<<endl;

  TFile *fout=new TFile("o_anaTrk.root","RECREATE");

  dAsym = new TH1D("dAsym","(P - M)/(P + M) at detector",100,-0.7,0.7);
  dPang = new TH1D("dPang","entries at detector  V;ang along X [deg]",200,-100,100);
  dMang = new TH1D("dMang","entries at detector mV;ang along X [deg]",200,-100,100);

  pAsym = new TH1D("pAsym",";z[mm];(P - M)/(P + M)",nbin,0,5);  
  for(int i=0;i<nbin;i++){
    double zmin=pAsym->GetBinCenter(i+1) - pAsym->GetBinWidth(i+1)/2;
    double zmax=pAsym->GetBinCenter(i+1) + pAsym->GetBinWidth(i+1)/2;
    zA[i]=new TH1D(Form("zAsym_%d",i),Form("(P - M)/(P + M) at %4.2f<=z<%4.2f",zmin,zmax),100,-0.7,0.7);
    zP[i]=new TH1D(Form("zPent_%d",i),Form("entries at %4.2f<=z<%4.2f; ang along x [deg]",zmin,zmax),200,-100,100);
    zM[i]=new TH1D(Form("zMent_%d",i),Form("entries at %4.2f<=z<%4.2f; ang along x [deg]",zmin,zmax),200,-100,100);
  }

  
  TFile *finV=TFile::Open(fileV.c_str(),"READ");
  TTree *tV=(TTree*)finV->Get("t");    
  TFile *finM=TFile::Open(fileM.c_str(),"READ");
  TTree *tM=(TTree*)finM->Get("t");    

  processTree(tV,dPang,zP);
  processTree(tM,dMang,zM);
  analyze();
  
  fout->cd();
  dPang->Write();
  dMang->Write();
  dAsym->Write();
  pAsym->Write();
  for(int i=0;i<nbin;i++){
    zA[i]->Write();
    zP[i]->Write();
    zM[i]->Write();
  }
  finV->Close();
  finM->Close();
  fout->Close();

  return 0;
}

void analyze(){

  int dbin=dPang->GetXaxis()->GetNbins();
  for(int i=1;i<=dbin;i++){
    double sgn = abs(dPang->GetXaxis()->GetBinCenter(i))/dPang->GetXaxis()->GetBinCenter(i); 
    double vp=dPang->GetBinContent(i);
    double vm=dMang->GetBinContent(i);
    if(vp+vm > 0)
      dAsym->Fill( sgn * (vp - vm)/(vp+vm) );

    for(int j=0;j<nbin;j++){
      vp = zP[j]->GetBinContent(i);
      vm = zM[j]->GetBinContent(i);
      if( vp + vm > 0 )
	zA[j]->Fill ( sgn * (vp - vm)/(vp+vm) );
    }
  }
  
  for(int i=0;i<nbin;i++){    
    pAsym->SetBinContent(i+1,zA[i]->GetMean());
    pAsym->SetBinError(i+1,zA[i]->GetMeanError());
  }

}

void processTree(TTree *t, TH1D *dE, TH1D *zE[20]){

  int eventNr,unitNo,trackID,parentID,pType;
  int material; //PBA:0 ; detectorMat:1
  double polX,polY,polZ,ap,am,postE,postKE,
    postPosX,postPosY,postPosZ,postMomX,postMomY,postMomZ,
    postAngX,postAngY, projPosX,projPosY, stepSize;
  t->SetBranchAddress("evNr",&eventNr);
  t->SetBranchAddress("material",&material);
  t->SetBranchAddress("unitNo",&unitNo);  
  t->SetBranchAddress("stepSize", &stepSize);  
  t->SetBranchAddress("pType",&pType);
  t->SetBranchAddress("trackID",&trackID);
  t->SetBranchAddress("parentID",&parentID);
  t->SetBranchAddress("polX", &polX);
  t->SetBranchAddress("polY", &polY);
  t->SetBranchAddress("polZ", &polZ);  
  t->SetBranchAddress("asymInfoPP", &ap);
  t->SetBranchAddress("asymInfoPM", &am);
  t->SetBranchAddress("postE",&postE);
  t->SetBranchAddress("postKE",&postKE);  
  t->SetBranchAddress("postPosX", &postPosX);
  t->SetBranchAddress("postPosY", &postPosY);
  t->SetBranchAddress("postPosZ", &postPosZ);
  t->SetBranchAddress("postMomX", &postMomX);
  t->SetBranchAddress("postMomY", &postMomY);
  t->SetBranchAddress("postMomZ", &postMomZ);  
  t->SetBranchAddress("postAngX",&postAngX);
  t->SetBranchAddress("postAngY",&postAngY);    
  t->SetBranchAddress("projPosX",&projPosX);
  t->SetBranchAddress("projPosY",&projPosY);  

  int thisEvNr=-1;
  std::vector< std::vector<int> > inter(20);
  std::vector< std::vector<int> > trkID(20);
  //  cout<<"sizeof "<<sizeof(inter)<<endl;
  
  int currentProc=1;
  int nEntries=t->GetEntries();
  cout<<nEntries<<endl;
  for(int i=0;i<nEntries;i++){
    t->GetEntry(i);
    //cout<<i<<" "<<eventNr<<" "<<thisEvNr<<" "<<postPosZ<<endl;

    if( int(i/double(nEntries)*100)>currentProc){
      cout<<"processed \t"<<int(i/double(nEntries)*100)<<"%"<<endl;
      currentProc+=10;
    }
    //if(currentProc > 10) break;

    if(thisEvNr!=eventNr){
      thisEvNr=eventNr;
      std::vector<int> dmInt;
      for(int j=0;j<20;j++)
	if(thisEvNr==-1){
	  inter.push_back(dmInt);
	  trkID.push_back(dmInt);
	}else{
	  inter[j].clear();
	  trkID[j].clear();
	}
    }

    if(abs(pType)!=11) continue;
    if(abs(postPosX)>1000) continue;
    if(abs(postAngX)>90) continue;
    if(postKE<3) continue;
    if(postPosZ<0) continue;

    if( material == 1 )
      dE->Fill(postAngX);

    if( material != 0 ) continue;
    
    int zbin=pAsym->GetXaxis()->FindBin(postPosZ) - 1;
    if(zbin>=20) continue;

    int hasParent(-1),nInt(-1);
    findInt(inter[zbin],trkID[zbin],trackID,parentID,hasParent,nInt);
    if(nInt!=1 || hasParent==1) continue;
    
    zE[zbin]->Fill( postAngX );

  }
}

void findInt(std::vector<int> &inter,std::vector<int> &val, int trackID,int parent, int &hasPar, int &nInt){
  int found=0;
  nInt=-2;
  int findParent=0;
  for(unsigned int i=0;i<val.size();i++){
    if(trackID==val[i]){
      inter[i]++;
      nInt=inter[i];
      found++;
    }
    if(parent==val[i])
      findParent++;
  }
  
  if(findParent) hasPar=1;
  else hasPar=0;
  
  if(!found){
    val.push_back(trackID);
    inter.push_back(1);
    nInt=1;
  }
  if(found>1){
    cout<<"multiple entries for track "<<trackID<<endl;
  }
}
