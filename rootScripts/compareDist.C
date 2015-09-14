#include <string>

TCanvas *c1=new TCanvas("c1","c1",1400,600);
string onm="y_distCompare.pdf";

void compareDist(){

  c1->Print(Form("%s[",onm.c_str()),"pdf");

  compareOne("distXposPe");
  compareOne("distXposAe");
  compareOne("distXposPh");
  compareOne("distXangPe");
  compareOne("distXangAe");
  compareOne("distXangPh");
  
  c1->Print(Form("%s]",onm.c_str()),"pdf");

}

void compareOne(string distnm){
  c1->Clear();
  double scale = 6.;
  TFile *f1=TFile::Open("../output/o_msc_L_500k_dist.root","READ");
  TFile *f2=TFile::Open("/Users/ciprian/qweak/QweakG4DD/output/depol/o_dist_asym_depol_L_299.root","READ");

  TH1D *h1=(TH1D*)f1->Get(Form("%s",distnm.c_str()));
  TH1D *h2=(TH1D*)f2->Get(Form("%s_2",distnm.c_str()));

  
  c1->cd(0);
  c1->Divide(2);
  c1->cd(1);
  h1->Scale(scale);
  h1->SetLineColor(4);
  h2->SetLineColor(2);
  h2->SetTitle("Red: Qweak | Blue: MSc");
  h2->DrawCopy();
  h1->DrawCopy("same");
  if(distnm.find("pos")==5)
    gPad->SetLogy(1);
  c1->cd(2);
  TH1D *div=h1->Clone(Form("div_%s",distnm.c_str()));
  h1->Sumw2();
  h2->Sumw2();
  div->Divide(h1,h2);
  div->DrawCopy();

  c1->Print(onm.c_str(),"pdf");
			  
  f1->Close();
  f2->Close();    
}
