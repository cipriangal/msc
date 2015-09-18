TCanvas *c1=new TCanvas("c1","c1",1200,900);
string onm;
TGraph *singleScat;

int printAll;

void mscPDGcomparisonAndPbXsec(){

  onm="y_MSCcomparisonAndPbXsec.pdf";
  printAll=1;
  
  c1->Print(Form("%s[",onm.c_str()),"pdf");

  gStyle->SetOptFit(1);
  
  double length[12]={0.01,0.02,0.03,0.05,0.10,0.20,0.3,0.4,0.5,0.7,0.9,1.};
  double ene[12]={10,20,100,150,500,0,0,0,0,0,0,0,0};

  // doDep(length,4,100,0);
  // doDep(ene,4,0.02,1);
  doDep(ene,5,0.001,1);
  
  c1->Print(Form("%s]",onm.c_str()),"pdf");
}

void doDep(double dep[12], int nStep, double other, int energyToggle){
  
  TGraphErrors *PDG=new TGraphErrors();
  TGraphErrors *MSC=new TGraphErrors();
  
  for(int i=0;i<nStep;i++){
    double x=dep[i];
    double y(0),dy(0),prediction(0);
    if(energyToggle){
      doOne(Form("../output/o_mscSteppingAction_L%4.2f_%dMeV.root",other,(int)dep[i]),y,dy,other,(int)dep[i]);
      prediction=meanAng(dep[i],other);
    }else{
      doOne(Form("../output/o_mscSteppingAction_L%4.2f_%dMeV.root",dep[i],(int)other),y,dy,dep[i],(int)other);
      prediction=meanAng(other,dep[i]);      
    }
    cout<<y<<" "<<dy<<" <> "<<prediction<<endl;
    PDG->SetPoint(i,x,prediction);
    MSC->SetPoint(i,x,y);
    PDG->SetPointError(i,0,0.1*prediction);
    MSC->SetPointError(i,0,dy);    
  }

  PDG->SetMarkerStyle(20);
  PDG->SetMarkerColor(2);
  PDG->SetLineColor(2);
  MSC->SetMarkerStyle(24);
  MSC->SetMarkerColor(4);
  PDG->GetYaxis()->SetTitle("Mean angle deviation for MSc [rad]");
  MSC->GetYaxis()->SetTitle("Mean angle deviation for MSc [rad]");
  if(energyToggle){
    MSC->SetTitle(Form("Fixed thickness %4.2f",other));
    PDG->GetXaxis()->SetTitle("Initial electron momentum [MeV]");
    MSC->GetXaxis()->SetTitle("Initial electron momentum [MeV]");
  }else{
    MSC->SetTitle(Form("Fixed momentum %d",(int)other));
    PDG->GetXaxis()->SetTitle("Radiator length [X_0]");
    MSC->GetXaxis()->SetTitle("Radiator length [X_0]");
  }

  MSC->Draw("AP");
  PDG->Draw("P");
  
  if(energyToggle)
    gPad->SetLogy(1);
  else
    gPad->SetLogy(0);
    
  TLegend *lg1;
  if(energyToggle)
    lg1=new TLegend(0.5,0.7,0.8,0.9);
  else
    lg1=new TLegend(0.15,0.7,0.45,0.9);

  lg1->AddEntry(PDG,"MSc Prediction with 10\% uncert","lep");
  lg1->AddEntry(MSC,"Sim mean with uncer","lep");
  lg1->Draw();
  c1->Print(onm.c_str(),"pdf");
  
}

void doOne(string fnm,double &mean,double &dmean, double len, int ener){
  cout<<fnm.c_str()<<endl;

  getXsect(ener);

  double pi=3.14159265;
  
  TFile *fin=TFile::Open(fnm.c_str(),"READ");
  TTree *t=(TTree*)fin->Get("t");
  t->Draw("postTheta","postTheta!=-999 && trackID==1 && parentID==0 && material==1");
  double bh=htemp->GetBinCenter(htemp->FindLastBinAbove(0));
  int bnmax=(int)(bh*100.);
  TH1D *h=new TH1D("h","Angle distribution primary coming out of the Pb",bnmax*10,0,bnmax/100.);
  cout<<" Bin width "<<h->GetBinWidth(2)<<endl;
  t->Project("h","postTheta","postTheta!=-999 && trackID==1 && parentID==0 && material==1");

  //maybe i should fit this FIXME
  TF1 *gs=new TF1("gs","gaus");
  TF1 *sg=new TF1("sg",fitSingle,0.05,1,1);

  TF1 *fit=new TF1("fit",fitAll,0,1,4);
  fit->SetParNames("Single Scat Norm","Constant","Mean","Sigma");
  fit->SetParameters(h->GetBinContent(h->GetXaxis()->FindBin(0.1))/singleScat->Eval(0.1),h->GetMaximum(),meanAng(ener,len),h->GetRMS());

  fit->FixParameter(0,0.115*len*100.);
  //fit->SetParLimits(0,0,2);
  //h->Fit("fit","Q&&0","",0,0.8);
  h->Fit("gs","Q","",0,0.05);
  
  //cout<<h->GetBinContent(h->GetXaxis()->FindBin(0.05))/singleScat->Eval(0.05)<<
  sg->SetParameter(0,0.115*len*100);
  sg->SetLineColor(3);
  //gs->SetParameters(fit->GetParameter(1),fit->GetParameter(2),fit->GetParameter(3));
  //gs->SetLineColor(2);

  h->DrawCopy();  
  sg->Draw("same");
  
  TLegend *legh=new TLegend(0.3,0.7,0.55,0.9);
  legh->AddEntry(h,Form("Radiator Length: %4.2f*X_0",len),"l");
  legh->AddEntry(h,Form("Electron mom: %d MeV",ener),"l");
  legh->Draw();
  gPad->SetLogy(1);
  mean=gs->GetParameter(1);
  dmean=gs->GetParError(1);
  if(printAll)
    c1->Print(onm.c_str(),"pdf");
  
  fin->Close();
}

void getXsect(int ener){
  ifstream fin(Form("./PbXsection/Pb_xsec_%d_MeV.dat",ener));
  singleScat=new TGraph();
  double th(0),xsec(0),dm(0);
  int n=0;
  while(fin>>th>>dm>>xsec){
    //cout<<th<<" "<<xsec<<endl;
    if(th>0.05){
      singleScat->SetPoint(n,th,xsec);
      n++;
    }
    
  }
  fin.close();
  singleScat->SetName(Form("PbSingleScat_%d",ener));

  singleScat->SetMarkerColor(1);
  singleScat->SetMarkerStyle(20);
  singleScat->SetLineColor(1);  
}

double meanAng(double p,double radLen){
  double eMass=0.510998910;//MeV
  double beta=sqrt(p*p/(p*p+eMass*eMass));
  return 13.6/(beta*p)*1.*sqrt(radLen)*(1.+0.0038*log(radLen));
}

double fitSingle(double *x, double *par){
  //cout<<x[0]<<" "<<singleScat->Eval(x[0])<<" "<<par[0]<<endl;
  return singleScat->Eval(x[0])*par[0];
}

double fitAll(double *x, double *par){
  double single=singleScat->Eval(x[0])*par[0];
  double gaus=par[1]*exp(- pow(x[0]-par[2],2) / (2*par[3]*par[3]));

  if(x<0.05) return gaus;
  //if(x>0.1) return single;
  return single+gaus;
}
