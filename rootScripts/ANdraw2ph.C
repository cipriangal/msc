TCanvas *c1=new TCanvas("c1","c1",800,600);

string fnm;
string inm;
string onm;

void ANdraw2ph(){
  fnm="../output/anaPowerData/o_ANd_1e3x2ph.root";
  inm="../output/anaPowerData/o_msc_ANdata_1e3x2ph.txt";
  onm="../output/anaPowerData/y_ANd_1e3x2ph_2.pdf";
  c1->Print(Form("%s[",onm.c_str()),"pdf");
  getData();
  drawData();
  c1->Print(Form("%s]",onm.c_str()),"pdf");
}


void getData(){
  TFile *fout=new TFile(fnm.c_str(),"RECREATE");

  double ene,th,pol,pow,q;  
  TTree *t=new TTree("t","Analyzing power data");
  t->Branch("energy",&ene,"energy/D");
  t->Branch("q",&q,"q/D");
  t->Branch("theta",&th,"theta/D");
  t->Branch("polR",&pol,"polR/D");
  t->Branch("AN",&pow,"AN/D");

  double deg=180./3.1415926536;
  ifstream fin(inm.c_str());
  char dm[500];
  fin.getline(dm,500);
  cout<<dm<<endl;
  while(fin>>ene>>th>>pow>>pol){
    th=acos(th)*deg;
    q=sqrt(4.*pow(ene,2)*sin(th/deg));
    //cout<<ene<<" "<<th<<" "<<pow<<" "<<pol<<endl;
    t->Fill();
  }
  fin.close();

  fout->cd();
  t->Write();
  fout->Close(); 
}

void drawData(){
  TFile *fin=TFile::Open(fnm.c_str(),"READ");
  TTree *t=(TTree*)fin->Get("t");

  t->Draw("theta : log10(q) : abs(AN)","energy>0","colz");
  htemp->SetTitle("abs(Analyzing power 1000x2ph); log10(q) [MeV]; theta [deg]");
  htemp->Draw("colz");
  gPad->SetLogz(1);
  c1->Print(onm.c_str(),"pdf");
  gPad->SetLogz(0);

  t->Draw("theta : log10(q)","energy>0","colz");
  gPad->SetLogz(1);
  c1->Print(onm.c_str(),"pdf");
  gPad->SetLogz(0);

  t->Draw("theta : log10(energy) : abs(AN)","energy>0","colz");
  htemp->SetTitle("abs(Analyzing power 1000x2ph); log10(E) [MeV]; theta [deg]");
  htemp->Draw("colz");
  gPad->SetLogz(1);
  c1->Print(onm.c_str(),"pdf");
  gPad->SetLogz(0);

  t->Draw("theta : log10(q) : abs(AN)","energy>0 && theta<30","colz");
  htemp->SetTitle("abs(Analyzing power 1000x2ph); log10(q) [MeV]; theta [deg]");
  htemp->Draw("colz");
  gPad->SetLogz(1);
  c1->Print(onm.c_str(),"pdf");
  gPad->SetLogz(0);

  t->Draw("theta : log10(q)","energy>0 && theta<30","colz");
  gPad->SetLogz(1);
  c1->Print(onm.c_str(),"pdf");
  gPad->SetLogz(0);

  t->Draw("theta : log10(energy) : abs(AN)","energy>0 && theta<30","colz");
  htemp->SetTitle("abs(Analyzing power 1000x2ph); log10(E) [MeV]; theta [deg]");
  htemp->Draw("colz");
  gPad->SetLogz(1);
  c1->Print(onm.c_str(),"pdf");
  gPad->SetLogz(0);


  fin->Close();
}

