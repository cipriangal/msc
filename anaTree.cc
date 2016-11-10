#include <string>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <math.h>
#include <algorithm>

#include "TH3D.h"
#include "TGraphErrors.h"
#include "TFile.h"
#include "TTree.h"

using namespace std;

const int dimension=5;//3 DoF + 2 PE values
vector<double> scanPoints[dimension];
const int debugPrint=0;

void readPEs();
void getCorners(int lowerIndex, int upperIndex, int depth, std::vector<double> point,
		std::vector<double> points[dimension]);
void getPEs(std::vector<double> in[dimension], std::vector<double> pt,
	    double &outL, double &outR);

void findInt(std::vector<int> &inter,std::vector<int> &val,
	     int trackID,int parent, int &hasPar, int &nInt);

int main(int argc, char** argv)
{

  if( argc <2 ) {
    cout<<" usage: build/anaTree [path to infile with tree] [optional use PE? 1 for true; default false]"<<endl;
    return 1;
  }

  readPEs();

  string file(argv[1]);  
  int usePEs(0);
  if(argc==3) usePEs=atoi(argv[2]);
  
  cout<<"Analyzing "<<file.c_str()<<" using PEs?: "<<usePEs<<endl;

  TFile *fout=new TFile("o_anaTree.root","RECREATE");
  TGraphErrors *g[3];
  g[0]=new TGraphErrors();
  g[1]=new TGraphErrors();
  g[2]=new TGraphErrors();
  g[0]->SetName("gPe");
  g[1]->SetName("gAe");
  g[2]->SetName("gPeCalc");

  TH1D *p_m_asPE =new TH1D("p_m_asPE" ,"- asymmetry for Primary",200,0,200);
  TH1D *n_m_asPE =new TH1D("n_m_asPE" ,"- asymmetry for Non Primary",200,0,200);
  TH1D *p_m_angPE=new TH1D("p_m_angPE","- P PEs;angle in shower [deg]",180,-90,90);
  TH1D *n_m_angPE=new TH1D("n_m_angPE","- N PEs;angle in shower [deg]",180,-90,90);

  TH1D *p_p_asPE =new TH1D("p_p_asPE" ,"+ asymmetry for Primary",200,0,200);
  TH1D *n_p_asPE =new TH1D("n_p_asPE" ,"+ asymmetry for Non Primary",200,0,200);
  TH1D *p_p_angPE=new TH1D("p_p_angPE","+ P PEs;angle in shower [deg]",180,-90,90);
  TH1D *n_p_angPE=new TH1D("n_p_angPE","+ N PEs;angle in shower [deg]",180,-90,90);

  TH1D *calc_L_asPE=new TH1D("calc_L_asPE","L calculated asymmetry",200,-1.1,1.1);
  TH1D *calc_R_asPE=new TH1D("calc_R_asPE","R calculated asymmetry",200,-1.1,1.1);

  TH1D *calc_P_angPE=new TH1D("calc_P_angPE","P calc;angle in shower [deg]",360,-90,90);
  TH1D *calc_M_angPE=new TH1D("calc_M_angPE","M calc;angle in shower [deg]",360,-90,90);

  TH1D *bins_L_angPE=new TH1D("bins_R_angPE","R calculated asymmetry;angle in shower [deg]",360,-90,90);
  
  TFile *fin=TFile::Open(file.c_str(),"READ");

  int eventNr,unitNo,trackID,parentID,pType;
  int material; //PBA:0 ; detectorMat:1
  double polX,polY,polZ,asymInfoPP,asymInfoPM,postE,postKE,
    postPosX,postPosY,postPosZ,postMomX,postMomY,postMomZ,
    postAngX,postAngY, projPosX,projPosY, stepSize;
  TTree *t=(TTree*)fin->Get("t");    
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
  t->SetBranchAddress("asymInfoPP", &asymInfoPP);
  t->SetBranchAddress("asymInfoPM", &asymInfoPM);
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

  // int nPrim(0),nNon(0);
  // double avAsP,avAsN;
  
  int thisEvNr=-1;
  std::vector<int> interaction,tID;
  double lTotPE(0),rTotPE(0);
  double plTotPE(0),prTotPE(0);
  double nlTotPE(0),nrTotPE(0);
  int currentProc=1;
  int nEntries=t->GetEntries();
  for(int i=0;i<nEntries;i++){
    t->GetEntry(i);
    if( int(i/double(nEntries)*100)>currentProc){
      cout<<"processed \t"<<int(i/double(nEntries)*100)<<"%"<<endl;
      currentProc+=10;
    }

    if(material!=1) continue;
    if(abs(pType)!=11) continue;
    if(abs(postPosX)>1000) continue;
    if(abs(postAngX)>89) continue;
    if(postKE<3) continue;
    
    if(thisEvNr!=eventNr){
      thisEvNr=eventNr;
      interaction.clear();
      tID.clear();
    }

    int hasParent(-1),nInt(-1);
    findInt(interaction,tID,trackID,parentID,hasParent,nInt);
    if(nInt!=1 || hasParent==1) continue;

    std::vector<double> pt1(dimension-2,0);//correct point
    std::vector<double> pt2(dimension-2,0);//mirror point
    std::vector<double> pts1[dimension];
    std::vector<double> pts2[dimension];
    pt1[0]=postPosX/10.;
    pt1[1]= ( (postKE>100) ? 100 : postKE );
    pt1[2]=postAngX;

    pt2[0]=-pt1[0];
    pt2[2]=-pt1[2];
    pt2[1]=pt1[1];

	    
    double rpe(-1),lpe(-1);
    double rp1(-1),rp2(-1);
    double lp1(-1),lp2(-1);
    getCorners(0,scanPoints[0].size(),0,pt1,pts1);
    getPEs(pts1,pt1,lp1,rp1);
    
    getCorners(0,scanPoints[0].size(),0,pt2,pts2);
    getPEs(pts2,pt2,lp2,rp2);
    
    if(lp1<0 || rp1<0 || lp2<0 || rp2<0 ||
       isnan(lp1) || isnan(rp1) || isnan(lp2) || isnan(rp2) ||
       isinf(lp1) || isinf(rp1) || isinf(lp2) || isinf(rp2)){
      cout<<"Problem with interpolator! "<<endl;
      cout<<" x,E,ang: "<<pt1[0]<<" "<<pt1[1]<<" "<<pt1[2]<<endl;
      cout<<" "<<lp1<<" "<<rp1<<" "<<lp2<<" "<<rp2<<endl;
      //exit(1);
      continue;
    }
    lpe=(lp1+rp2)/2;
    rpe=(rp1+lp2)/2;
    
    if( lpe+rpe <= 0 ) continue;

    if(!usePEs){
      lpe=1;
      rpe=1;
    }
    
    lTotPE+=lpe;
    rTotPE+=rpe;
    
    if(trackID==1 && parentID==0){
      p_p_asPE->Fill(rpe);
      p_p_angPE->Fill(postAngX,rpe);
      p_m_asPE->Fill(lpe);
      p_m_angPE->Fill(postAngX,lpe);
      plTotPE+=lpe;
      prTotPE+=rpe;
    }else{
      n_p_asPE->Fill(rpe);
      n_p_angPE->Fill(postAngX,rpe);
      n_m_asPE->Fill(lpe);
      n_m_angPE->Fill(postAngX,lpe);
      nlTotPE+=lpe;
      nrTotPE+=rpe;
    }

    if(asymInfoPM>-2 && asymInfoPP>-2 && asymInfoPM+asymInfoPP!=0){
      double cas=(asymInfoPP-asymInfoPM)/(asymInfoPP+asymInfoPM);
      calc_L_asPE->Fill(lpe*cas);
      calc_R_asPE->Fill(rpe*cas);

      bins_L_angPE->Fill(postAngX);
      calc_P_angPE->Fill(postAngX,lpe*asymInfoPP);
      calc_M_angPE->Fill(postAngX,rpe*asymInfoPM);
    }
  }
  
  //cout<<(lTotPE-rTotPE)/(lTotPE+rTotPE)<<endl;
    
  fin->Close();

  TH1D *calc_A_bin=(TH1D*)calc_P_angPE->Clone();
  calc_A_bin->SetName("calc_A_bin");
  for(int i=1;i<=calc_A_bin->GetXaxis()->GetNbins();i++){
    double a=calc_P_angPE->GetBinContent(i);
    double b=calc_M_angPE->GetBinContent(i);
    if(a+b>0)
      calc_A_bin->SetBinContent(i,(a-b)/(a+b));
  }
  
  // TH1D *calc_dd_angPE=(TH1D*)calc_L_angPE->Clone();
  // calc_dd_angPE->SetName("calc_dd_angPE");
  // calc_dd_angPE->SetTitle("DD calc (Rpe*as - Lpe*as)");
  // TH1D *calc_Ab_angPE=(TH1D*)calc_L_angPE->Clone();
  // calc_Ab_angPE->SetName("calc_Ab_angPE");
  // calc_Ab_angPE->SetTitle("Abias calc (Rpe*as + Lpe*as)");
  // int nb=calc_L_angPE->GetXaxis()->GetNbins();
  // TH1D *calc_Ab_angPE_fold=new TH1D("calc_Ab_angPE_fold","folded Abias calculation",
  // 				    nb/2,0,90);

  // for(int i=1;i<=nb;i++){
  //   double vr=calc_R_angPE->GetBinContent(i);
  //   double vl=calc_L_angPE->GetBinContent(i);
  //   calc_dd_angPE->SetBinContent(i,(vr-vl)/(plTotPE+prTotPE));
  //   calc_Ab_angPE->SetBinContent(i,(vr+vl)/(plTotPE+prTotPE));
  // }
  // for(int i=1;i<=nb/2;i++){
  //   double vl=calc_Ab_angPE->GetBinContent(i);
  //   double vr=calc_Ab_angPE->GetBinContent(nb-i+1);
  //   calc_Ab_angPE_fold->SetBinContent(nb/2-i+1,vl+vr);
  // }
  
  TH1D *p_dd_ang=(TH1D*)p_p_angPE->Clone();
  p_dd_ang->SetName("p_dd_ang");
  p_dd_ang->SetTitle("phys asym primary");
  TH1D *n_dd_ang=(TH1D*)n_p_angPE->Clone();
  n_dd_ang->SetName("n_dd_ang");
  n_dd_ang->SetTitle("phys asym non primary");

  TH1D *p_dd_bin=(TH1D*)p_p_angPE->Clone();
  p_dd_bin->SetName("p_dd_bin");
  p_dd_bin->SetTitle("phys asym primary");
  TH1D *n_dd_bin=(TH1D*)n_p_angPE->Clone();
  n_dd_bin->SetName("n_dd_bin");
  n_dd_bin->SetTitle("phys asym non primary");

  int nb=p_p_angPE->GetXaxis()->GetNbins();  
  for(int i=1;i<=nb;i++){
    double vm=p_p_angPE->GetBinContent(i);
    double vp=p_m_angPE->GetBinContent(nb-i+1);
    p_dd_ang->SetBinContent(i,(vp-vm)/(plTotPE+prTotPE));
    if(vm+vp>0)
      p_dd_bin->SetBinContent(i,(vp-vm)/(vp+vm));
    vp=n_p_angPE->GetBinContent(i);
    vm=n_m_angPE->GetBinContent(nb-i+1);
    n_dd_ang->SetBinContent(i,(vp-vm)/(nlTotPE+nrTotPE));
    if(vm+vp>0)
      n_dd_bin->SetBinContent(i,(vp-vm)/(vp+vm));
  }

  // calc_L_angPE->Scale(1./plTotPE);
  // calc_R_angPE->Scale(1./prTotPE);

  fout->cd();
  p_p_asPE->Write();
  n_p_asPE->Write();
  p_p_angPE->Write();
  n_p_angPE->Write();
  p_m_asPE->Write();
  n_m_asPE->Write();
  p_m_angPE->Write();
  n_m_angPE->Write();
  p_dd_ang->Write();
  n_dd_ang->Write();
  p_dd_bin->Write();
  n_dd_bin->Write();

  calc_L_asPE->Write();
  calc_R_asPE->Write();

  calc_P_angPE->Write();
  calc_M_angPE->Write();
  calc_A_bin->Write();
  
  // calc_dd_angPE->Write();
  // calc_Ab_angPE->Write();
  // calc_Ab_angPE_fold->Write();
  return 0;
  
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


void readPEs(){
  ifstream fin("input/idealBar_alongDir_acrossAng23_lightPara.txt");
  //ifstream fin("input/idealBar_alongDir_acrossAng0_lightPara.txt");
  //ifstream fin("input/md8Config16_alongDir_acrossAng23_lightPara.txt");
  double x1,x2,x3,x4,x5,x6,x7,x8,x9;  
  string data;
  getline(fin,data);

  for(int i=0;i<dimension;i++)
    scanPoints[i].clear();
  
  while(fin>>x1>>x2>>x3>>x4>>x5>>x6>>x7>>x8>>x9){
    scanPoints[0].push_back(x1);//position
    scanPoints[1].push_back(x2);//energy
    scanPoints[2].push_back(x3);//angle
    scanPoints[3].push_back(x6);//LPEs
    scanPoints[4].push_back(x8);//RPEs
    if(debugPrint)
      cout<<x1<<" "<<x2<<" "<<x3<<" "<<x6<<" "<<x8<<endl;
  }
  
  fin.close();
}

void getCorners(int lowerIndex, int upperIndex, int depth, std::vector<double> point,
		std::vector<double> points[dimension]){

  if(lowerIndex==-1 || upperIndex==-1 || lowerIndex>upperIndex){
    cout<<"Problem with index: "<<lowerIndex<<" "<<upperIndex<<endl;    
    exit(0);
  }
  if(lowerIndex==upperIndex) return;
  
  int lI(-1),hI(-1);
  double valSmaller(999),valLarger(999);
  int nextDepth=depth+1;
  
  std::vector<double>::iterator begin=scanPoints[depth].begin();
  std::vector<double>::iterator start=begin+lowerIndex;
  std::vector<double>::iterator stop =begin+upperIndex;

  if(debugPrint)
    cout<<"start upper : "<<point[depth]<<" "<<*start<<" "<<*(stop-1)<<" "
	<<int(start-begin)<<" "<<int(stop-begin)<<endl;

  if( point[depth]== *start)
    lI=lowerIndex;
  else if( point[depth] == *(stop-1) ){
    lI = int( lower_bound(start,stop,point[depth]) - begin );
  }else{    
    valSmaller = *( lower_bound(start,stop,point[depth]) - 1 );
    lI = int( lower_bound(start,stop,valSmaller) - begin );
  }
  
  hI = int( upper_bound(start,stop,point[depth]) - begin );

  if(debugPrint){
    cout<<depth<<" "<<lowerIndex<<" "<<upperIndex<<" "<<lI<<" "<<hI<<endl;
    cout<<" "<<valSmaller<<" "<<point[depth]<<" "<<valLarger<<" "
	<<scanPoints[depth][lI]<<" "<<scanPoints[depth][hI-1]<<endl;
  }

  if(depth==dimension-3){

    for(int i=0;i<dimension;i++) {
      points[i].push_back(scanPoints[i][lI]);
      points[i].push_back(scanPoints[i][hI]);
    }
    
    if(debugPrint){
      cout<<endl<<endl<<"End lower: "<<endl;
      for(int i=0;i<dimension;i++) cout<<scanPoints[i][lI]<<" ";
      cout<<endl;
      for(int i=0;i<dimension;i++) cout<<scanPoints[i][hI]<<" ";
      cout<<endl<<endl;
    }
    return;
  }else{
    getCorners(lI,hI,nextDepth,point,points);
  }

  if( point[depth] == *(stop-1) ) return;

  if(debugPrint)
    cout<<"start upper : "<<depth<<" "<<point[depth]<<" "<<*start<<" "<<*(stop-1)<<" "
	<<int(start-begin)<<" "<<int(stop-begin)<<endl;
  
  lI = int( upper_bound(start,stop,point[depth]) - begin );
  if( point[depth] == *(stop-1) )
    hI = upperIndex;
  else{
    valLarger=*(lower_bound(start,stop,point[depth]));
    hI = int( upper_bound(start,stop,valLarger) - begin );
  }
  if(debugPrint){
    cout<<depth<<" "<<lowerIndex<<" "<<upperIndex<<" "<<lI<<" "<<hI<<endl;
    cout<<" "<<valSmaller<<" "<<point[depth]<<" "<<valLarger<<" "
	<<scanPoints[depth][lI]<<" "<<scanPoints[depth][hI-1]<<endl;
  }
  
  if( point[depth]!= *start )
    getCorners(lI,hI,nextDepth,point,points);
}

void getPEs(std::vector<double> in[dimension], std::vector<double> pt,
	    double &outL, double &outR){

  std::vector<double> dm[dimension];

  if(debugPrint){
    cout<<"start "<<in[0].size()<<endl;
    for(unsigned long i=0;i<in[0].size();i++){
      for(int j=0;j<dimension;j++)
	cout<<in[j][i]<<" ";
      cout<<endl;
    }
  }

  
  if(in[0].size()==1){
    outL=in[dimension-2].front();
    outR=in[dimension-1].front();
    return;
  }

  int depth(-1);
  for(int j=0;j<dimension-2;j++)
    if(in[j][0]!=in[j][1])
      depth=j;
  
  for(int i=0;i<dimension;i++){
    dm[i].resize(in[i].size());
    for(unsigned long j=0;j<in[i].size();j++)
      dm[i][j]=in[i][j];
  }

  for(int i=0;i<dimension;i++)
    in[i].resize(dm[i].size()/2);

  for(unsigned long i=0;i<dm[0].size();i+=2){

    double l1=dm[dimension-2][i];
    double r1=dm[dimension-1][i];
    double l2=dm[dimension-2][i+1];
    double r2=dm[dimension-1][i+1];
    double fl=l1+(l2-l1)*(pt[depth]-dm[depth][i])/(dm[depth][i+1]-dm[depth][i]);
    double fr=r1+(r2-r1)*(pt[depth]-dm[depth][i])/(dm[depth][i+1]-dm[depth][i]);
      
    for(int j=0;j<dimension-2;j++)
      if(j!=depth)
	in[j][i/2]=dm[j][i];
      else
	in[j][i/2]=pt[depth];
    in[dimension-2][i/2]=fl;
    in[dimension-1][i/2]=fr;      
  }

  if(debugPrint){
    cout<<endl<<depth<<endl;
    for(unsigned long i=0;i<in[0].size();i++){
      for(int j=0;j<dimension;j++)
	cout<<in[j][i]<<" ";
      cout<<endl;
    }
  }
  getPEs(in,pt,outL,outR);
}
