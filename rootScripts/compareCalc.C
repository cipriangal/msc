void compareCalc(){
  gStyle->SetOptStat(0);

  TCanvas *c1=new TCanvas("c1","c1",800,600);
  compareCalc1("calc_L_asPE");

  TCanvas *c2=new TCanvas("c2","c2",800,600);
  compareCalc1("calc_L_angPE");

  TCanvas *c3=new TCanvas("c3","c3",800,600);
  compareCalc1("calc_dd_angPE");

}

void compareCalc1(string hnm){
  const int nFiles=4;
  string fnm[nFiles]={
    "../o_anaTree_std_1e5.root",    
    "../o_anaTree_stp02mm_1e5.root",
    "../o_anaTree_stp01mm_1e5.root",
    "../o_anaTree_stp005mm_1e5.root"};

  string stpNm[nFiles]={"std","0.2 mm","0.1 mm","0.05 mm"};
  int colors[nFiles]={1,2,4,6};

  TLegend *leg = new TLegend(0.5,0.7,0.88,0.9);

  TFile *fin[nFiles];
  TH1D *h[nFiles];
  for(int i=0;i<nFiles;i++){
    fin[i]=TFile::Open(fnm[i].c_str(),"READ");
    h[i]=(TH1D*)fin[i]->Get(hnm.c_str());
    h[i]->Rebin(4);
    h[i]->SetLineColor(colors[i]);
    leg->AddEntry(h[i],stpNm[i].c_str(),"l");
    if(i==0)
      h[i]->DrawCopy();
    else
      h[i]->DrawCopy("same");
  }
  leg->Draw();
  
}
