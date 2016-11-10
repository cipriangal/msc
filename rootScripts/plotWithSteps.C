void plotWithSteps(){
  TFile *fin=TFile::Open("../o_anaWithStepsPos_3D_1e6_noMod_Mott.root","READ");
  //TFile *fin=TFile::Open("../o_anaWithSteps_3D_1e6_noMod_Mott.root","READ");

  string onm="../output/y_analyzeWeightWithStepsPosAsym.pdf";

  TCanvas *c1=new TCanvas("c1","c1",800,600);
  c1->Print(Form("%s[",onm.c_str()),"pdf");
  string partNm[3]={"","det_","pba_"};
  
  for(i=0;i<20;i++){
    c1->Clear();
    c1->Divide(3,2);
    
    for(int j=0;j<3;j++){
      TH1D *hp=(TH1D*)fin->Get(Form("%shp_%d",partNm[j].c_str(),i));
      TH1D *hm=(TH1D*)fin->Get(Form("%shm_%d",partNm[j].c_str(),i));
      TH1D *ha=(TH1D*)fin->Get(Form("%sha_%d",partNm[j].c_str(),i));

      c1->cd(1+j);
      hp->SetLineColor(2);
      hp->DrawCopy();
      hm->DrawCopy("same");
      gPad->SetGridx(1);
      gPad->SetGridy(1);
      c1->cd(4+j);
      ha->DrawCopy();
      gPad->SetGridx(1);
      gPad->SetGridy(1);
      
    }
    c1->Print(onm.c_str(),"pdf");
  }
  fin->Close();
  c1->Print(Form("%s]",onm.c_str()),"pdf");
}
