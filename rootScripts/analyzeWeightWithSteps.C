
void analyzeWeightWithSteps(){
  TFile *fin=TFile::Open("../o_msc_3D_1e6_noMod_Mott_bigDetPb.root","READ");
  TTree *t=(TTree*)fin->Get("t");

  string primary="trackID==1 && parentID==0 && pType==11 && material!=-999 && postAngX!=-999";
  string onm="../output/y_analyzeWeightWithSteps_bigDetPb.pdf";

  TCanvas *c1=new TCanvas("c1","c1",1000,600);
  c1->Print(Form("%s[",onm.c_str()),"pdf");
  t->Draw("stpNr",Form("%s && material==1",primary.c_str()));
  c1->Print(onm.c_str(),"pdf");
  t->Draw("stpNr:postPosX>>h0(100,-1000,1000,30,0,30)",Form("%s && material==1",primary.c_str()),"colz");
  c1->Print(onm.c_str(),"pdf");
  t->Draw("stpNr:postAngX>>h0(200,-100,100,30,0,30)",Form("%s && material==1",primary.c_str()),"colz");
  c1->Print(onm.c_str(),"pdf");
  t->Draw("stpNr:asymInfoPP>>h0(200,0,3,30,0,30)",Form("%s && material==1",primary.c_str()),"colz");
  c1->Print(onm.c_str(),"pdf");
  t->Draw("stpNr:asymInfoPM>>h0(200,0,3,30,0,30)",Form("%s && material==1",primary.c_str()),"colz");
  c1->Print(onm.c_str(),"pdf");

  c1->Clear();
  c1->Divide(4,2);
  int nb=200;

  TH2D *h1=new TH2D("h1","primaries;postAngX [deg];postPosX [mm]",200,-100,100,100,-100,100);
  TH2D *h5=new TH2D("h5","primaries Det;postAngX [deg];postPosX [mm]",200,-100,100,100,-100,100);

  TH2D *h2=new TH2D("h2","primaries;asymInfoPP;asymInfoPM",200,0,3,200,0,3);
  TH2D *h3=new TH2D("h3","primaries Det;asymInfoPP;asymInfoPM",200,0,3,200,0,3);
  TH2D *h4=new TH2D("h4","primaries PBA;asymInfoPP;asymInfoPM",200,0,3,200,0,3);
  TH2D *h7=new TH2D("h7","primaries Det;asymInfoPP;postPosX",200,0.7,1.3,100,-100,100);
  TH2D *h8=new TH2D("h8","primaries PBA;asymInfoPP;postPosX",200,0.7,1.3,100,-100,100);

  // TH1D *hp=new TH1D("hp","P*N ;angle at detector [deg]"      ,nb,-100,100);
  // TH1D *hm=new TH1D("hm","M*N ;angle at detector [deg]"      ,nb,-100,100);
  // TH1D *hM=new TH1D("hM","P*N - M*N ;angle at detector [deg]",nb,-100,100);
  // TH1D *hP=new TH1D("hP","P*N + M*N ;angle at detector [deg]",nb,-100,100);
  // TH1D *ha=new TH1D("ha","asymmetry ;angle at detector [deg]",nb,-100,100);

  for(int stp=1;stp<21;stp++){
    cout<<" at step "<<stp<<endl;
    c1->cd(1);
    t->Project("h1","postPosX:postAngX",Form("%s && stpNr==%d",primary.c_str(),stp),"colz");    
    h1->SetTitle(Form("primaries at step %d",stp));
    h1->GetZaxis()->SetRangeUser(0.5,1e5);
    h1->DrawCopy("colz");
    gPad->SetGridx(1);
    gPad->SetGridy(1);
    gPad->SetLogz(1);    

    c1->cd(2);
    t->Project("h5","postPosX:postAngX",Form("%s && stpNr==%d && material==1",primary.c_str(),stp),"colz");
    h5->GetZaxis()->SetRangeUser(0.5,1e5);
    h5->DrawCopy("colz");
    gPad->SetGridx(1);
    gPad->SetGridy(1);
    gPad->SetLogz(1);    

    c1->cd(3);
    t->Draw("material",Form("%s && stpNr==%d",primary.c_str(),stp));    
    gPad->SetLogy(1);

    c1->cd(4);
    t->Project("h7","postPosX:asymInfoPP",Form("%s && stpNr==%d && material==1",primary.c_str(),stp),"colz");
    h7->GetZaxis()->SetRangeUser(0.5,1e5);
    h7->DrawCopy("colz");
    gPad->SetGridx(1);
    gPad->SetGridy(1);
    gPad->SetLogz(1);    

    c1->cd(8);
    t->Project("h8","postPosX:asymInfoPP",Form("%s && stpNr==%d && material==0",primary.c_str(),stp),"colz");
    h8->GetZaxis()->SetRangeUser(0.5,1e5);
    h8->DrawCopy("colz");
    gPad->SetGridx(1);
    gPad->SetGridy(1);
    gPad->SetLogz(1);    
    

    c1->cd(7);
    t->Project("h2","asymInfoPP:asymInfoPM",Form("%s && stpNr==%d",primary.c_str(),stp),"colz");
    h2->GetZaxis()->SetRangeUser(0.5,3e5);
    h2->DrawCopy("colz");
    gPad->SetGridx(1);
    gPad->SetGridy(1);
    gPad->SetLogz(1);    

    c1->cd(6);
    t->Project("h3","asymInfoPP:asymInfoPM",Form("%s && stpNr==%d && material==1",primary.c_str(),stp),"colz");
    h3->GetZaxis()->SetRangeUser(0.5,3e5);
    h3->DrawCopy("colz");
    gPad->SetLogz(1);    
    gPad->SetGridx(1);
    gPad->SetGridy(1);

    c1->cd(5);
    t->Project("h4","asymInfoPP:asymInfoPM",Form("%s && stpNr==%d && material==0",primary.c_str(),stp),"colz");
    h4->GetZaxis()->SetRangeUser(0.5,3e5);
    h4->DrawCopy("colz");	
    gPad->SetLogz(1);    
    gPad->SetGridx(1);
    gPad->SetGridy(1);
    
    c1->Modified();
    c1->Update();
    c1->Print(onm.c_str(),"pdf");

    // c1->cd(1);
    // getAsym(hp,hm,hP,hM,ha,t,Form("(%s && stpNr==%d)",primary.c_str(),stp));
    // hp->DrawCopy();
    // hm->DrawCopy("same");
    // gPad->SetGridx(1);
    // gPad->SetGridy(1);
    // c1->cd(4);
    // ha->DrawCopy();
    // gPad->SetGridx(1);
    // gPad->SetGridy(1);

    // c1->cd(2);
    // getAsym(hp,hm,hP,hM,ha,t,Form("(%s && stpNr==%d && material==1)",primary.c_str(),stp));
    // hp->DrawCopy();
    // hm->DrawCopy("same");
    // gPad->SetGridx(1);
    // gPad->SetGridy(1);
    // c1->cd(5);
    // ha->DrawCopy();
    // gPad->SetGridx(1);
    // gPad->SetGridy(1);

    // c1->cd(3);
    // getAsym(hp,hm,hP,hM,ha,t,Form("(%s && stpNr==%d && material==0)",primary.c_str(),stp));
    // hp->DrawCopy();
    // hm->DrawCopy("same");
    // gPad->SetGridx(1);
    // gPad->SetGridy(1);
    // c1->cd(6);
    // ha->DrawCopy();
    // gPad->SetGridx(1);
    // gPad->SetGridy(1);

    // c1->Modified();
    // c1->Update();
    // c1->Print(onm.c_str(),"pdf");

  }
  fin->Close();
  c1->Print(Form("%s]",onm.c_str()),"pdf");
}

void getAsym(TH1 *hp, TH1 *hm, TH1 *hP, TH1 *hM, TH1 *ha, TTree *t,string cut){
  hp->Reset();
  hm->Reset();
  hP->Reset();
  hM->Reset();
  ha->Reset();  
  
  t->Project("hp","postAngX",Form("asymInfoPP*%s",cut.c_str()));
  t->Project("hm","postAngX",Form("asymInfoPM*%s",cut.c_str()));
  hp->SetLineColor(2);
  hp->SetTitle(cut.c_str());
  
  hM->Add(hp,hm,1,-1);
  hP->Add(hp,hm,1,1);
  ha->Divide(hM,hP);
}
