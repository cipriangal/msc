void anaAvg(){
  // anaAvg1("o_msc_3D_1e6_noMod_Mott.root");
  // anaAvg2("o_msc_3D_1e6_noMod_Mott.root");

  // anaAvg1("o_msc_3D_1e6_noMod_Mottx20.root");
  // anaAvg2("o_msc_3D_1e6_noMod_Mottx20.root");

  anaAvg1("o_msc_3D_1e6_noMod_Mott_RotatedPhi.root");
  anaAvg2("o_msc_3D_1e6_noMod_Mott_RotatedPhi.root");
}

void anaAvg1(string fnm){

  TFile *fin=TFile::Open(fnm.c_str(),"READ");
  TTree *t=(TTree*)fin->Get("t");

  int nbin=180;
  TH1D *hp=new TH1D("hp","P*N;angle [deg]",nbin,-90,90);
  TH1D *hm=new TH1D("hm","M*N;angle [deg]",nbin,-90,90);
  TH1D *ha=new TH1D("ha","P*N-M*N/+;angle [deg]",nbin,-90,90);

  t->Project("hp","postAngX","asymInfoPP*(trackID==1 && parentID==0 && material==1)");
  t->Project("hm","postAngX","asymInfoPM*(trackID==1 && parentID==0 && material==1)");

  for(int i=1;i<=nbin;i++){
    double a=hp->GetBinContent(i);
    double b=hm->GetBinContent(i);
    if(a+b>0)
      ha->SetBinContent(i,(a-b)/(a+b));
  }

  TCanvas *c1=new TCanvas("c1","c1");
  hp->SetLineColor(2);
  hp->DrawCopy();
  hm->DrawCopy("same");
  TCanvas *c3=new TCanvas("c3","c3");
  ha->DrawCopy();
  
  fin->Close();
}


void anaAvg2(string fnm){
  
  TFile *fin=TFile::Open(fnm.c_str(),"READ");
  TTree *t=(TTree*)fin->Get("t");

  int nbin=180;
  TH1D *ha=new TH1D("ha","(P-M/P+M)*N;angle [deg]",nbin,-90,90);

  t->Project("ha","postAngX","((asymInfoPP-asymInfoPM)/(asymInfoPP+asymInfoPM))*(trackID==1 && parentID==0 && material==1 && (asymInfoPP+asymInfoPM)>0)");

  TCanvas *c2=new TCanvas("c2","c2");
  ha->DrawCopy();
  
  fin->Close();
}
