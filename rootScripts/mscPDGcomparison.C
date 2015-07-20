TCanvas *c1=new TCanvas("c1","c1",1200,900);
string onm;
int printAll;

void mscPDGcomparison(){

  //onm="y_MSCcomparison.pdf";
  //printAll=0;
  onm="y_MSCcomparisonAll.pdf";
  printAll=1;
  
  c1->Print(Form("%s[",onm.c_str()),"pdf");

  gStyle->SetOptFit(1);
  
  double length[12]={0.01,0.02,0.03,0.05,0.10,0.20,0.3,0.4,0.5,0.7,0.9,1.};
  double energy[12]={10,15,20,30,40,50,70,100,150,200,300,500};

  doDep(length,12,20,0);
  //doDep(length, 5,20,0);

  doDep(length,12,100,0);
  //doDep(length, 5,100,0);

  doDep(length,12,150,0);
  //doDep(length, 5,150,0);

  doDep(energy,12,0.02,1);
    
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
  
  double pi=3.14159265;
  
  TFile *fin=TFile::Open(fnm.c_str(),"READ");
  TTree *t=(TTree*)fin->Get("t");
  t->Draw("postTheta","postTheta!=-999 && trackID==1 && parentID==0 && material==1");
  double bh=htemp->GetBinCenter(htemp->FindLastBinAbove(0));
  TH1D *h=new TH1D("h","Angle distribution primary coming out of the Pb",300,0,bh+0.01);
  t->Project("h","postTheta","postTheta!=-999 && trackID==1 && parentID==0 && material==1");
  
  TF1 *gs=new TF1("gs","gaus");
  gs->SetParameters(h->GetMaximum(),h->GetMean(),h->GetRMS());

  h->Fit("gs","Q","",h->GetMean()-2*h->GetRMS(),h->GetMean()+2*h->GetRMS());
  h->DrawCopy();
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

double meanAng(double p,double radLen){
  double eMass=0.510998910;//MeV
  double beta=sqrt(p*p/(p*p+eMass*eMass));
  return 13.6/(beta*p)*1.*sqrt(radLen)*(1.+0.0038*log(radLen));
}
