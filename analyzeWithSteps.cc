#include <string>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <math.h>
#include <algorithm>

#include "TH3D.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TGraphErrors.h"
#include "TFile.h"
#include "TTree.h"

using namespace std;

void calcAsym(TH1 *hpp, TH1 *hmm, TH1 *haa);

int main(int argc, char** argv)
{

  if( argc < 4 ) {
    cout<<" usage: build/anaTree [path to infile with tree] [0: weight, 1:modify] [nr of Steps] [0: angle; 1: position]"<<endl;
    return 1;
  }

  string file(argv[1]);  
  int mode=atoi(argv[2]);
  int nStp=atoi(argv[3]);
  int anaVar=atoi(argv[4]);
  if(anaVar!=0 && anaVar!=1) {cout<<"not sure what anaVar " <<anaVar<<" means"<<endl; return 3;}
  cout<<"Analyzing "<<file.c_str()<<" mode :"<<mode<<" nStpes "<<nStp<<endl;
  if(nStp>=30) return 2;

  string varNm[2]={"postAngX","postPosX"};
  string varTit[2]={"angle along xAxis [deg]","position along xAxis [cm]"};

  TFile *fout=new TFile(Form("o_anaWithSteps_%s.root",varNm[anaVar].c_str()),"RECREATE");
  TH1D *hp[30],*hm[30],*ha[30];
  TH1D *det_hp[30],*det_hm[30],*det_ha[30];
  TH1D *pba_hp[30],*pba_hm[30],*pba_ha[30];
  int nb=200;
  for(int i=0;i<nStp;i++){
    hp[i]=new TH1D(Form("hp_%d",i),Form("P*N stp %d;%s",i,varTit[anaVar].c_str()),nb,-100,100);
    hm[i]=new TH1D(Form("hm_%d",i),Form("M*N stp %d;%s",i,varTit[anaVar].c_str()),nb,-100,100);
    ha[i]=new TH1D(Form("ha_%d",i),Form("A   stp %d;%s",i,varTit[anaVar].c_str()),nb,-100,100);

    det_hp[i]=new TH1D(Form("det_hp_%d",i),Form("P*N at Det stp %d;%s",i,varTit[anaVar].c_str()),nb,-100,100);
    det_hm[i]=new TH1D(Form("det_hm_%d",i),Form("M*N at Det stp %d;%s",i,varTit[anaVar].c_str()),nb,-100,100);
    det_ha[i]=new TH1D(Form("det_ha_%d",i),Form("A   at Det stp %d;%s",i,varTit[anaVar].c_str()),nb,-100,100);
    
    pba_hp[i]=new TH1D(Form("pba_hp_%d",i),Form("P*N in Pb  stp %d;%s",i,varTit[anaVar].c_str()),nb,-100,100);
    pba_hm[i]=new TH1D(Form("pba_hm_%d",i),Form("M*N in Pb  stp %d;%s",i,varTit[anaVar].c_str()),nb,-100,100);
    pba_ha[i]=new TH1D(Form("pba_ha_%d",i),Form("A   in Pb  stp %d;%s",i,varTit[anaVar].c_str()),nb,-100,100);
  }
  
  TFile *fin=TFile::Open(file.c_str(),"READ");
  int eNr,sNr,tID,pID,pType;
  int mat; //PBA:0 ; detectorMat:1
  double aP,aM,postPosX,postPosY,postPosZ,postMomX,postMomY,postMomZ,postAngX,postAngY;
  TTree *t=(TTree*)fin->Get("t");    
  t->SetBranchAddress("evNr",&eNr);
  t->SetBranchAddress("stpNr",&sNr);
  t->SetBranchAddress("pType",&pType);
  t->SetBranchAddress("trackID",&tID);
  t->SetBranchAddress("parentID",&pID);
  t->SetBranchAddress("material",&mat);
  t->SetBranchAddress("asymInfoPP", &aP);
  t->SetBranchAddress("asymInfoPM", &aM);
  t->SetBranchAddress("postPosX", &postPosX);
  t->SetBranchAddress("postPosY", &postPosY);
  t->SetBranchAddress("postPosZ", &postPosZ);
  t->SetBranchAddress("postMomX", &postMomX);
  t->SetBranchAddress("postMomY", &postMomY);
  t->SetBranchAddress("postMomZ", &postMomZ);  
  t->SetBranchAddress("postAngX",&postAngX);
  t->SetBranchAddress("postAngY",&postAngY);    

  int currentProc(-1);
  int nEntries=t->GetEntries();
  for(int i=0;i<nEntries;i++){
    t->GetEntry(i);
    if( int(i/double(nEntries)*100)>currentProc){
      cout<<"processed \t"<<int(i/double(nEntries)*100)<<"%"<<endl;
      currentProc+=10;
    }
    
    if(tID!=1) continue;
    if(pID!=0) continue;
    if(abs(pType)!=11) continue;
    if(fabs(postPosX)>1000) continue;
    if(sNr-1>=nStp || sNr==0) continue;

    double wP(1),wM(1);
    if(mode==0){
      wP=aP;
      wM=aM;
    }
    double varVal = anaVar == 0 ? postAngX : postPosX/10;

    if(mat==1){
      det_hp[sNr-1]->Fill(varVal,wP);
      det_hm[sNr-1]->Fill(varVal,wM);
    }else if(mat==0){
      pba_hp[sNr-1]->Fill(varVal,wP);
      pba_hm[sNr-1]->Fill(varVal,wM);
    }else continue;

    hp[sNr-1]->Fill(varVal,wP);
    hm[sNr-1]->Fill(varVal,wM);

  }
  fin->Close();
  
  for(int i=0;i<nStp;i++){
    calcAsym(hp[i],hm[i],ha[i]);
    calcAsym(det_hp[i],det_hm[i],det_ha[i]);
    calcAsym(pba_hp[i],pba_hm[i],pba_ha[i]);
    fout->cd();
    hp    [i]->Write();
    hm    [i]->Write();
    ha    [i]->Write();
    det_hp[i]->Write();
    det_hm[i]->Write();
    det_ha[i]->Write();
    pba_hp[i]->Write();
    pba_hm[i]->Write();
    pba_ha[i]->Write();
  }
  fout->Close();
  return 0;
  
}

void calcAsym(TH1 *hpp, TH1 *hmm, TH1 *haa){
  int nbin=hpp->GetXaxis()->GetNbins();
  for(int i=1;i<=nbin;i++){
    double a=hpp->GetBinContent(i);
    double b=hmm->GetBinContent(i);
    if(a+b>0)
      haa->SetBinContent(i,(a-b)/(a+b));
  }
}
