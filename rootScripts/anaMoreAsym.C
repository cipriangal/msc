void anaMoreAsym(){
  TFile *fp=TFile::Open("../o_moreAsym_5stps_V_paul_localPandN_m20proc.root","READ");
  TFile *fm=TFile::Open("../o_moreAsym_5stps_mV_paul_localPandN_m20proc.root","READ");
  string onm="../output/y_moreAsymAna_phi_5stps_paul_localPandN_m20proc.pdf";

  TTree *tp=(TTree*)fp->Get("t");
  TTree *tm=(TTree*)fm->Get("t");

  string tit="Entries for step";
  int nbin=190;
  TH1D *hp=new TH1D("hp","",nbin,-190,190);
  TH1D *hm=new TH1D("hm","",nbin,-190,190);
  TH1D *ha=new TH1D("ha","",nbin,-190,190);

  TH1D *haxm=new TH1D("haxm",";angle along X [deg]",nbin,-190,190);
  TH1D *haym=new TH1D("haym",";angle along Y [deg]",nbin,-190,190);
  TH1D *haxp=new TH1D("haxp",";angle along X [deg]",nbin,-190,190);
  TH1D *hayp=new TH1D("hayp",";angle along Y [deg]",nbin,-190,190);
  
  TCanvas *c1=new TCanvas("c1","c1",1200,1200);
  c1->Clear();
  c1->Divide(2,2);
  c1->Print(Form("%s[",onm.c_str()),"pdf");
  double pi=acos(-1);
  TF1 *fsin=new TF1("fsin",Form("[0]*sin( (x+[1])*%f/180 )",pi));
  // fsin->SetParLimits(1,0,360);
  // fsin->SetParLimits(0,0,1);

  string plotVar=Form("phi*180/%f",pi);
  //string plotVar=Form("atan2(sin(th)*cos(phi),cos(th))*180/%f",pi);
  cout<<plotVar<<endl;

  string varAx=Form("atan2(cos(phi)*sin(th),cos(th))*180/%f",pi);
  string varAy=Form("atan2(sin(phi)*sin(th),cos(th))*180/%f",pi);

  double x[50],y[50],dx[50],dy[50];
  int nStp=5;
  for(int i=0;i<nStp;i++){
    cout<<endl<<"at step "<<i<<endl<<endl;
    if(i==1){
      tp->Project("hp",plotVar.c_str(),Form("stpNr==%d",i));
      tp->Project("hm",plotVar.c_str(),Form("stpNr==%d",i-1));
      c1->cd(1);
      hp->SetTitle(Form("%s %d;phi angle [deg]",tit.c_str(),i));
      hp->SetLineColor(2);
      hp->DrawCopy();
      hm->DrawCopy("same");
      gPad->SetGridx(1);
      gPad->SetGridy(1);

      c1->cd(2);
      tm->Project("hp",plotVar.c_str(),Form("stpNr==%d",i));
      tm->Project("hm",plotVar.c_str(),Form("stpNr==%d",i-1));
      hp->SetTitle(Form("%s %d;phi angle [deg]",tit.c_str(),i));
      hp->SetLineColor(2);
      hp->DrawCopy();
      hm->DrawCopy("same");
      gPad->SetGridx(1);
      gPad->SetGridy(1);

      c1->cd(3);
      gPad->Clear();
      
      c1->cd(4);
      gPad->Clear();
      
      c1->Print(onm.c_str(),"pdf");      
    }

    tm->Project("hm",plotVar.c_str(),Form("stpNr==%d",i));
    tp->Project("hp",plotVar.c_str(),Form("stpNr==%d",i));


    getAsym(hp,hm,ha);
    gStyle->SetOptStat(1);
    c1->cd(1);
    hp->SetTitle(Form("%s %d;phi angle [deg]",tit.c_str(),i));
    hp->SetLineColor(2);
    hm->DrawCopy();
    hp->DrawCopy("same");

    c1->cd(2);
    gStyle->SetOptStat(0);
    gStyle->SetOptFit(1);
    fsin->SetParameters(0.2,90);
    ha->Fit("fsin","","",-180,180);
    ha->DrawCopy();
    x[i]=i;
    dx[i]=0;
    dy[i]=fsin->GetParError(0);
    y[i]=fsin->GetParameter(0);
    gPad->SetGridx(1);
    gPad->SetGridy(1);

    tm->Project("haxm",varAx.c_str(),Form("stpNr==%d",i));
    tp->Project("haxp",varAx.c_str(),Form("stpNr==%d",i));
    c1->cd(3);
    haxp->SetLineColor(2);
    haxp->DrawCopy();
    haxm->DrawCopy("same");
    gPad->SetGridx(1);
    gPad->SetGridy(1);

    tm->Project("haym",varAy.c_str(),Form("stpNr==%d",i));
    tp->Project("hayp",varAy.c_str(),Form("stpNr==%d",i));
    c1->cd(4);
    hayp->SetLineColor(2);
    hayp->DrawCopy();
    haym->DrawCopy("same");
    gPad->SetGridx(1);
    gPad->SetGridy(1);

    c1->Print(onm.c_str(),"pdf");
  }

  c1->Clear();
  c1->cd(0);
  TGraphErrors *gr=new TGraphErrors(nStp,x,y,dx,dy);
  gr->SetMarkerStyle(20);
  gr->SetMarkerColor(2);
  gr->SetLineColor(2);
  gr->Draw("AP");
  c1->Print(onm.c_str(),"pdf");
      
  c1->Print(Form("%s]",onm.c_str()),"pdf");
  fp->Close();
  fm->Close();
}


void getAsym(TH1 *p,TH1 *m, TH1* a){
  int nb=p->GetXaxis()->GetNbins();
  for(int i=1;i<=nb;i++){
    double aa=p->GetBinContent(i);
    double bb=m->GetBinContent(i);
    if(aa+bb>0)
      a->SetBinContent(i,(aa-bb)/(aa+bb));
  }
}
