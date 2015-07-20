TCanvas *c1=new TCanvas("c1","c1",1200,900);
string onm;

void mscPDGcomparison(){

  onm="y_MSCcomparison.pdf";
  c1->Print(Form("%s[",onm.c_str()),"pdf");

  gStyle->SetOptFit(1);
  
  double length[12]={0.01,0.02,0.03,0.05,0.10,0.20,0.3,0.4,0.5,0.7,0.9,1.};
  int energy[12]={10,15,20,30,40,50,70,100,150,200,300,500};

  TGraphErrors *lenPDG=new TGraphErrors();
  TGraphErrors *lenMSC=new TGraphErrors();
  TGraphErrors *enePDG=new TGraphErrors();
  TGraphErrors *eneMSC=new TGraphErrors();

  int nStep=12;

  for(int i=0;i<nStep;i++){
    double x=length[i];
    double y,dy;
    doOne(Form("../output/o_mscSteppingAction_L%4.2f_20MeV.root",length[i]),i*3+12,y,dy,length[i],20);
    double prediction=meanAng(20,length[i]);
    lenPDG->SetPoint(i,x,prediction);
    lenMSC->SetPoint(i,x,y);
    lenPDG->SetPointError(i,0,0.1*prediction);
    lenMSC->SetPointError(i,0,dy);

    x=energy[i];
    doOne(Form("../output/o_mscSteppingAction_L0.02_%dMeV.root",energy[i]),i+1,y,dy,0.02,energy[i]);
    prediction=meanAng(energy[i],0.02);
    enePDG->SetPoint(i,x,prediction);
    eneMSC->SetPoint(i,x,y);
    enePDG->SetPointError(i,0,0.1*prediction);
    eneMSC->SetPointError(i,0,dy);
  }

  lenPDG->SetMarkerStyle(20);
  lenPDG->SetMarkerColor(2);
  lenPDG->SetLineColor(2);
  lenMSC->SetMarkerStyle(24);
  lenMSC->SetMarkerColor(4);
  lenPDG->GetYaxis()->SetTitle("Mean angle deviation for MSc [rad]");
  lenPDG->GetXaxis()->SetTitle("Radiator length [X_0]");
  lenPDG->Draw("AP");
  lenMSC->Draw("P");
  gPad->SetLogy(0);
  TLegend *lg1=new TLegend(0.5,0.2,0.8,0.4);
  lg1->AddEntry(lenPDG,"MSc Prediction with 10\% uncert","lep");
  lg1->AddEntry(lenMSC,"Sim mean with uncer","lep");
  lg1->Draw();
  c1->Print(onm.c_str(),"pdf");

  enePDG->SetMarkerStyle(20);
  enePDG->SetMarkerColor(2);
  enePDG->SetLineColor(2);
  eneMSC->SetMarkerStyle(24);
  eneMSC->SetMarkerColor(4);
  enePDG->GetYaxis()->SetTitle("Mean angle deviation for MSc [rad]");
  enePDG->GetXaxis()->SetTitle("Electron energy [MeV]");
  enePDG->Draw("AP");
  eneMSC->Draw("P"); 
  gPad->SetLogy(1);
  TLegend *lg2=new TLegend(0.5,0.7,0.8,0.9);
  lg2->AddEntry(enePDG,"MSc Prediction with 10\% uncert","lep");
  lg2->AddEntry(eneMSC,"Sim mean with uncer","lep");
  lg2->Draw();

  c1->Print(onm.c_str(),"pdf");

  
  c1->Print(Form("%s]",onm.c_str()),"pdf");
}

void doOne(string fnm,int nr,double &mean,double dmean&, double len, int ener){
  cout<<fnm.c_str()<<endl;
  
  double pi=3.14159265;
  
  TFile *fin=TFile::Open(fnm.c_str(),"READ");
  TTree *t=(TTree*)fin->Get("t");
  t->Draw("postTheta","postTheta!=-999 && trackID==1 && parentID==0 && material==1");
  double bh=htemp->GetBinCenter(htemp->FindLastBinAbove(0));
  TH1D *h=new TH1D(Form("h%d",nr),"Angle distribution primary coming out of the Pb",300,0,bh+0.01);
  t->Project(Form("h%d",nr),"postTheta","postTheta!=-999 && trackID==1 && parentID==0 && material==1");
  
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
  c1->Print(onm.c_str(),"pdf");
  fin->Close();
}

double meanAng(double p,double radLen){
  double eMass=0.510998910;//MeV
  double beta=sqrt(p*p/(p*p+eMass*eMass));
  return 13.6/(beta*p)*1.*sqrt(radLen)*(1.+0.0038*log(radLen));
}
