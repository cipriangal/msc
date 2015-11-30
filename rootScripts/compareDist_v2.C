void compareDist(){

  compare1("../../QweakG4DD/output/mottUpdateCH/o_dist_Center_V_100xAN_16e6.root",
	   "../output/depth15x2mm/mottUpdate/o_msc_V_15xmott1e2Update_45e6.root",
	   "distPe",
	   "hdistPe_9");

  compare1("../../QweakG4DD/output/mottUpdateCH/o_dist_Center_V_100xAN_16e6.root",
  	   "../output/depth15x2mm/mottUpdate/o_msc_V_15xmott1e2Update_45e6.root",
  	   "distAe",
  	   "hdistAe_9");
}

void compare1(string f1,string f2,string h1,string h2){
  TH1 *x1=getOnePos("x1",f1,h1);
  TH1 *x2=getOnePos("x2",f2,h2);

  TH1 *ax1=getOneAng("ax1",f1,h1);
  TH1 *ax2=getOneAng("ax2",f2,h2);

  // int n1=x1->GetXaxis()->GetNbins();
  // double s1=x1->Integral(3*n1/8,5*n1/8);
  // int n2=x2->GetXaxis()->GetNbins();
  // double s2=x2->Integral(3*n2/4,5*n2/8);

  // x2->Scale(s1/s2);
  
  TCanvas *c1=new TCanvas();
  x1->SetLineColor(2);
  // x1->DrawCopy();
  // x2->DrawCopy("same");
  x1->DrawNormalized();
  x2->DrawNormalized("same");
  gPad->SetLogy(1);
  gPad->SetGridx(1);
  gPad->SetGridy(1);

  
  TCanvas *c2=new TCanvas();
  TH1 *div=x1->Clone();
  x1->Sumw2();
  x2->Sumw2();
  div->Divide(x1,x2);
  //div->Fit("gaus");
  div->DrawCopy();
  gPad->SetGridx(1);
  gPad->SetGridy(1);

  TCanvas *c3=new TCanvas();
  ax1->SetLineColor(2);
  ax1->DrawNormalized();
  ax2->DrawNormalized("same");
  //gPad->SetLogy(1);
  gPad->SetGridx(1);
  gPad->SetGridy(1);

  
  TCanvas *c4=new TCanvas();
  TH1 *diva=ax1->Clone();
  ax1->Sumw2();
  ax2->Sumw2();
  diva->Divide(ax1,ax2);
  diva->DrawCopy();
  gPad->SetGridx(1);
  gPad->SetGridy(1);
}

TH1 *getOnePos(string nm,string fn, string hn){

  TFile *fin=TFile::Open(fn.c_str(),"READ");
  TH3 *hp=(TH3*)fin->Get(hn.c_str());
  int nbx=hp->GetXaxis()->GetNbins();
  int nby=hp->GetYaxis()->GetNbins();
  int nbz=hp->GetZaxis()->GetNbins();
  double b0=hp->GetXaxis()->GetBinLowEdge(1);
  double bn=hp->GetXaxis()->GetBinUpEdge(nbx);  
  TH1D *h=new TH1D(nm.c_str(),Form("%s E>3 MeV ; position [cm]",hn.c_str()),nbx,b0,bn);

  double entries=0;
  for(int i=1;i<=nbx;i++){
    double val=0;
    for(int j=1;j<=nby;j++)
      for(int k=1;k<=nbz;k++){
	double e=hp->GetZaxis()->GetBinCenter(k);
	if(e<3) continue;
	val+=hp->GetBinContent(i,j,k);
      }
    h->SetBinContent(i,val);
    entries+=val;
  }
  h->SetEntries(entries);
  h->SetDirectory(0);
  fin->Close();
  return h;
}

TH1 *getOneAng(string nm,string fn, string hn){

  TFile *fin=TFile::Open(fn.c_str(),"READ");
  TH3 *hp=(TH3*)fin->Get(hn.c_str());
  int nbx=hp->GetXaxis()->GetNbins();
  int nby=hp->GetYaxis()->GetNbins();
  int nbz=hp->GetZaxis()->GetNbins();
  double b0=hp->GetYaxis()->GetBinLowEdge(1);
  double bn=hp->GetYaxis()->GetBinUpEdge(nby);  
  TH1D *h=new TH1D(nm.c_str(),Form("%s E>3 MeV ; angle [deg]",hn.c_str()),nby,b0,bn);

  double entries=0;
  for(int i=1;i<=nby;i++){
    double val=0;
    for(int j=1;j<=nbx;j++)
      for(int k=1;k<=nbz;k++){
	double e=hp->GetZaxis()->GetBinCenter(k);
	if(e<3) continue;
	val+=hp->GetBinContent(j,i,k);
      }
    h->SetBinContent(i,val);
    entries+=val;
  }
  h->SetEntries(entries);
  h->SetDirectory(0);
  fin->Close();
  return h;
}
