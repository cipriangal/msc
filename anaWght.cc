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

void processTree(TTree *t, TH1D *pA,TH1D *zA[20]);
void findInt(std::vector<int> &inter,std::vector<int> &val,
	     int trackID,int parent, int &hasPar, int &nInt);


int main(int argc, char** argv)
{

  if( argc <2 ) {
    cout<<" usage: build/anaWght [path to infile with tree]"<<endl;
    return 1;
  }

  string file(argv[1]);  
  
  cout<<"Analyzing "<<file.c_str()<<endl;

  TFile *fout=new TFile("o_anaWght.root","RECREATE");

  const int nbin=20;
  TH1D *pAsym = new TH1D("pAsym",";z[mm];(PP - PM)/(PP + PM)",nbin,0,5);  
  TH1D *zA[nbin];
  for(int i=0;i<nbin;i++){
    double zm=pAsym->GetBinCenter(i+1) - pAsym->GetBinWidth(i+1)/2;
    double zM=pAsym->GetBinCenter(i+1) + pAsym->GetBinWidth(i+1)/2;
    zA[i]=new TH1D(Form("zAsym_%d",i),Form("(PP - PM)/(PP + PM) at %4.2f<=z<%4.2f",zm,zM),100,-0.5,0.5);
  }

  
  TFile *fin=TFile::Open(file.c_str(),"READ");
  TTree *t=(TTree*)fin->Get("t");    
  TH1D *dAsym = new TH1D("dAsym","(PP - PM)/(PP + PM) at detector",100,-0.3,0.3);
  TH2D *dPPvsPM = new TH2D("dPPvsPM",";PM;PP",100,0.4,0.65,100,0.35,0.6);
  string pp="asymInfoPP/(asymInfoPP+asymInfoPM)";
  string pm="asymInfoPM/(asymInfoPP+asymInfoPM)";
  string primary="trackID==1 && parentID==0 && pType==11";
  int nent=t->Project("dAsym",Form("postAngX/abs(postAngX)*(%s - %s)/(%s + %s)",pp.c_str(),pm.c_str(),pp.c_str(),pm.c_str()),
		      Form("(material==1 && %s)",primary.c_str()));
  cout<<"dAsym "<<nent<<" "<<dAsym->GetEntries()<<endl;
  nent=t->Project("dPPvsPM",Form("%s : %s",pp.c_str(),pm.c_str()),
  	     Form("material==1 && %s",primary.c_str()));
  cout<<"dPPvsPM "<<nent<<endl;

  processTree(t,pAsym,zA);

  fout->cd();
  dAsym->Write();
  dPPvsPM->Write();
  pAsym->Write();
  for(int i=0;i<nbin;i++)
    zA[i]->Write();
  fin->Close();
  fout->Close();

  return 0;
}

void processTree(TTree *t, TH1D *pA,TH1D *zA[20]){

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

    if(material!=0) continue;
    if(abs(pType)!=11) continue;
    if(abs(postPosX)>1000) continue;
    if(abs(postAngX)>90) continue;
    if(postKE<3) continue;
    if(postPosZ<0) continue;

    int zbin=pA->GetXaxis()->FindBin(postPosZ) - 1;
    if(zbin>=20) {
      //cout<<"zbin > 20 ??? "<<postPosZ<<" "<<material<<" "<<zbin<<endl;
      continue;
    }
    int hasParent(-1),nInt(-1);
    findInt(inter[zbin],trkID[zbin],trackID,parentID,hasParent,nInt);
    if(nInt!=1 || hasParent==1) continue;
    
    double pp=ap/(am+ap);
    double pm=am/(am+ap);
    double sgnAng = postAngX/abs(postAngX);
    zA[zbin]->Fill( sgnAng*(pp-pm)/(pp+pm) );

  }

  for(int i=0;i<20;i++){
    pA->SetBinContent(i+1,zA[i]->GetMean());
    pA->SetBinError(i+1,zA[i]->GetMeanError());
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
