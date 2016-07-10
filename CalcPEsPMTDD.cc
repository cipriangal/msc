#include <string>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <math.h>
#include <algorithm>

#include "TH3D.h"
#include "TGraphErrors.h"
#include "TGraph.h"
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

int main(int argc, char** argv)
{

  if( argc !=3 ) {
    cout<<" usage: build/calcPEsPMTDD [#slices] [path to infile with distributions]"<<endl;
    return 1;
  }

  readPEs();

  const int nDet=atoi(argv[1]);  
  string file(argv[2]);
  cout<<"Analyzing "<<file.c_str()<<" for "<<nDet<<endl;
  TFile *fin=TFile::Open(file.c_str(),"READ");

  string part[2]={"Pe","Ae"};
  string partTit[2]={"Primary e-","All e"};

  TFile *fout=new TFile("o_calcPMTDD.root","RECREATE");
  TGraphErrors *g[2];
  TGraph *conv[2];
  for(int i=0;i<2;i++){
    g[i]=new TGraphErrors();
    g[i]->SetName(Form("g%s",part[i].c_str()));
    conv[i]=new TGraph();
    conv[i]->SetName(Form("conv_%s",part[i].c_str()));
  }
  
  TH1D *p_p_angPE=new TH1D("p_p_angPE","Primary +;shower angle [deg]",180,-90,90);
  TH1D *p_m_angPE=new TH1D("p_m_angPE","Primary -;shower angle [deg]",180,-90,90);
  TH1D *a_p_angPE=new TH1D("a_p_angPE","All     +;shower angle [deg]",180,-90,90);
  TH1D *a_m_angPE=new TH1D("a_m_angPE","All     -;shower angle [deg]",180,-90,90);

  double plTotPE(0),prTotPE(0);
  double alTotPE(0),arTotPE(0);
  int nConv[2]={0,0};
  
  for(int i=0;i<2;i++)
    for(int j=0;j<nDet;j++){
      TH3D *dist;
      if( nDet == 1 )
	dist=(TH3D*)fin->Get(Form("dist%s",part[i].c_str()));
      else
	dist=(TH3D*)fin->Get(Form("hdist%s_%d",part[i].c_str(),j));

      cout<<"looking at "<<dist->GetTitle()<<endl;
    
      double lTotPE(0),rTotPE(0);
      int counter=0;
      int printStep=500000;
    
      for(int xx=1;xx<=dist->GetXaxis()->GetNbins();xx++)//position
	for(int yy=1;yy<=dist->GetYaxis()->GetNbins();yy++)//angle
	  for(int zz=1;zz<=dist->GetZaxis()->GetNbins();zz++){//energy
	    double entries=dist->GetBinContent(xx,yy,zz);
	    if(entries<=0) continue;
	  
	    std::vector<double> pt1(dimension-2,0);//correct point
	    std::vector<double> pt2(dimension-2,0);//mirror point
	    std::vector<double> pts1[dimension];
	    std::vector<double> pts2[dimension];
	    
	    pt1[0]=dist->GetXaxis()->GetBinCenter(xx);
	    pt1[2]=dist->GetYaxis()->GetBinCenter(yy);
	    pt1[1]=dist->GetZaxis()->GetBinCenter(zz);
	    if(pt1[1]>100) pt1[1]=100.;
	    
	    pt2[0]=-pt1[0];
	    pt2[2]=-pt1[2];
	    pt2[1]=pt1[1];
	    
	    if(fabs(pt1[0])>90) continue;
	    if(fabs(pt1[2])>80) continue;
	    if(pt1[1]>100 || pt1[1]<3) continue;
	    
	    if(debugPrint || counter%printStep==1)
	      cout<<endl<<counter<<" !! Calc for pos, ang, E: "<<pt1[0]<<" "<<pt1[2]<<" "<<pt1[1]<<endl;
	    
	    double rpe(-1),lpe(-1);
	    double rp1(-1),rp2(-1);
	    double lp1(-1),lp2(-1);
	    getCorners(0,scanPoints[0].size(),0,pt1,pts1);
	    getPEs(pts1,pt1,lp1,rp1);
	    
	    getCorners(0,scanPoints[0].size(),0,pt2,pts2);
	    getPEs(pts2,pt2,lp2,rp2);

	    //cout<<lp1<<" "<<rp1<<" "<<lp2<<" "<<rp2<<" "<<lpe<<" "<<rpe<<endl;

	    if(lp1!=-1 && rp1!=-1 && lp2!=-1 && rp2!=-1){
	      lpe=(lp1+rp2)/2;
	      rpe=(rp1+lp2)/2;	    
	    }else{
	      cout<<"Problem with interpolator!"<<lpe<<" "<<rpe<<" "<<pt1[0]<<" "<<pt1[1]<<" "<<pt1[2]<<endl;
	      exit(1);
	    }
	    
	    if(debugPrint || counter%printStep==1){
	      cout<<"    ~~~~ lpe rpe TL TR "<<lpe<<" "<<rpe<<" "<<lTotPE<<" "<<rTotPE<<endl;
	      if(debugPrint) cin.ignore();
	    }

	    lTotPE+=lpe*entries;
	    rTotPE+=rpe*entries;
	    if(lTotPE+rTotPE>0){
	      conv[i]->SetPoint(nConv[i],nConv[i],(lTotPE-rTotPE)/(lTotPE+rTotPE));
	      nConv[i]++;
	    }
	      
	    if(i==0){
	      p_p_angPE->Fill(pt1[2],rpe*entries);
	      p_m_angPE->Fill(pt1[2],rpe*entries);
	    }else{
	      a_p_angPE->Fill(pt1[2],rpe*entries);
	      a_m_angPE->Fill(pt1[2],rpe*entries);
	    }
	    counter++;
	    if(isnan(lpe) || isnan(rpe)) exit(2);
	  }
      double as=(lTotPE-rTotPE)/(lTotPE+rTotPE);
      double das=2.*lTotPE*rTotPE/(pow(lTotPE+rTotPE,2))*sqrt((1./lTotPE)+(1./rTotPE));
      cout<<partTit[i]<<" : L R (L-R)/(L+R) "
	  <<setprecision(12)<<lTotPE<<" "<<rTotPE<<" "
	  <<(lTotPE-rTotPE)/(lTotPE+rTotPE)<<" pm "<<das<<endl;
      if(i==0){
	plTotPE=lTotPE;
	prTotPE=rTotPE;
      }else{
	alTotPE=lTotPE;
	arTotPE=rTotPE;
      }
      double zpos=2.+2.1*j;
      g[i]->SetPoint(j,zpos,as);
      g[i]->SetPointError(j,0,das);
    }    
  
  fin->Close();

  TH1D *p_dd_ang=(TH1D*)p_p_angPE->Clone();
  p_dd_ang->SetName("p_dd_ang");
  p_dd_ang->SetTitle("phys asym primary");
  TH1D *a_dd_ang=(TH1D*)a_p_angPE->Clone();
  a_dd_ang->SetName("a_dd_ang");
  a_dd_ang->SetTitle("phys asym all primary");

  int nb=p_p_angPE->GetXaxis()->GetNbins();
  for(int i=1;i<=nb;i++){
    double vp=p_p_angPE->GetBinContent(i);
    double vm=p_m_angPE->GetBinContent(nb-i+1);
    //if(vp+vm>0)
    p_dd_ang->SetBinContent(i,(vp-vm)/(plTotPE+prTotPE));
    vp=a_p_angPE->GetBinContent(i);
    vm=a_m_angPE->GetBinContent(nb-i+1);
    //if(vp+vm>0)
    a_dd_ang->SetBinContent(i,(vp-vm)/(alTotPE+arTotPE));
  }
  
  fout->cd();
  for(int i=0;i<2;i++){
    g[i]->Write();
    conv[i]->Write();
  }

  p_p_angPE->Write();
  p_m_angPE->Write();
  a_p_angPE->Write();
  a_m_angPE->Write();
  p_dd_ang->Write();
  a_dd_ang->Write();
  return 0;
  
}

void readPEs(){
  ifstream fin("macros/yl_md3_angle_scan.txt");
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
