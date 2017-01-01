void compareDist_v2(){

  compare1("../../QweakG4DD/output/mottUpdateCH/o_dist_Center_V_100xAN_16e6.root",
	   "../../QweakG4DD/output/mottUpdateCH/o_dist_L_Wouter_SingleBar_1e6.root",
	   //"../../QweakG4DD/output/mottUpdateCH/o_dist_Center_V_1e2_stdG4step_myMac.root",
	   //"../output/o_msc_Simple_V_500k_stdG4step.root",
	   //"../output/o_msc_Qweak_V_2M_stdG4step.root",
	   //"../o_msc.root",
	   //"../output/o_msc_Simple_V_2M_eOnly_stdG4step.root",
	   "distPe",
	   "distPe");
  
  compare1(//"../../QweakG4DD/output/mottUpdateCH/o_dist_Center_V_100xAN_16e6.root",
	   "../../QweakG4DD/output/mottUpdateCH/o_dist_L_Wouter_SingleBar_1e6.root",
	   "../../QweakG4DD/output/mottUpdateCH/o_dist_Center_V_1e2_stdG4step_myMac.root",
	   //"../output/o_msc_Simple_V_500k_stdG4step.root",
	   //"../output/o_msc_Qweak_V_2M_stdG4step.root",
	   //"../o_msc.root",
	   //"../output/o_msc_Simple_V_2M_eOnly_stdG4step.root",
	   "distAe",
	   "distAe");

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
  
  TCanvas *c1=new TCanvas(Form("%s_pos",h1.c_str()),Form("%s_pos",h1.c_str()),1200,600);
  c1->Divide(2);
  c1->cd(1);    
  x1->SetLineColor(2);
  // x1->DrawCopy();
  // x2->DrawCopy("same");
  x1->DrawNormalized();
  x2->DrawNormalized("same");
  gPad->SetLogy(1);
  gPad->SetGridx(1);
  gPad->SetGridy(1);

  c1->cd(2);
  TH1 *div=x1->Clone();
  x1->Sumw2();
  x2->Sumw2();
  div->Divide(x1,x2);
  //div->Fit("gaus");
  div->DrawCopy();
  gPad->SetGridx(1);
  gPad->SetGridy(1);

  TCanvas *c3=new TCanvas(Form("%s_ang",h1.c_str()),Form("%s_ang",h1.c_str()),1200,600);
  c3->Divide(2);
  c3->cd(1);
  ax1->SetLineColor(2);
  ax1->DrawNormalized();
  ax2->DrawNormalized("same");
  //gPad->SetLogy(1);
  gPad->SetGridx(1);
  gPad->SetGridy(1);

  
  c3->cd(2);
  TH1 *diva=ax1->Clone();
  ax1->Sumw2();
  ax2->Sumw2();
  diva->Divide(ax1,ax2);
  diva->DrawCopy();
  gPad->SetGridx(1);
  gPad->SetGridy(1);

  TFile *fin1=TFile::Open(f1.c_str(),"READ");
  TFile *fin2=TFile::Open(f2.c_str(),"READ");
  TH3 *full1=(TH3*)fin1->Get(h1.c_str());
  TH3 *full2=(TH3*)fin2->Get(h2.c_str());
  TH1 *e1=full1->Project3D("z");
  e1->SetName(Form("f1_%s",h1.c_str()));
  TH1 *e2=full2->Project3D("z");
  e2->SetName(Form("f2_%s",h2.c_str()));
  TCanvas *c4=new TCanvas(Form("%s_E",h1.c_str()),Form("%s_E",h1.c_str()),1200,600);
  c4->Divide(2);
  c4->cd(1);  
  e1->SetLineColor(2);
  double in1=e1->Integral(1,290);
  double in2=e2->Integral(1,290);
  // e2->Scale(in1/in2);
  // e1->DrawCopy();
  // e2->DrawCopy("same");
  e1->DrawNormalized();
  e2->DrawNormalized("same");
  gPad->SetGridx(1);
  gPad->SetGridy(1);
  c4->cd(2);
  TH1 *dive=e1->Clone();
  e1->Sumw2();
  e2->Sumw2();
  dive->Divide(e1,e2);
  dive->DrawCopy();
  gPad->SetGridx(1);
  gPad->SetGridy(1);
 
  // TCanvas *c5=new TCanvas();
  // e2->DrawNormalized();
  fin2->Close();
  fin1->Close();
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
