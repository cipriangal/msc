TCanvas *c1=new TCanvas("c1","c1",800,600);

string fnm;
string inm;
string onm;

void ANdataAna(){
  fnm="../output/anaPowerData/o_ANd_1e2xMott.root";
  inm="../output/anaPowerData/o_msc_ANdata_1e2xMott.txt";
  onm="../output/anaPowerData/y_ANd_1e2xMott.pdf";
  c1->Print(Form("%s[",onm.c_str()),"pdf");
  getData();
  drawData();
  c1->Print(Form("%s]",onm.c_str()),"pdf");

  // fnm="../output/anaPowerData/o_ANd_1e3x2ph.root";
  // inm="../output/anaPowerData/o_msc_ANdata_1e3x2ph.txt";
  // onm="../output/anaPowerData/y_ANd_1e3x2ph.pdf";
  // c1->Print(Form("%s[",onm.c_str()),"pdf");
  // getData();
  // drawData();
  // c1->Print(Form("%s]",onm.c_str()),"pdf");

  
}

void getData(){
  TFile *fout=new TFile(fnm.c_str(),"RECREATE");

  double ene,th,pol,pow;  
  TTree *t=new TTree("t","Analyzing power data");
  t->Branch("energy",&ene,"energy/D");
  t->Branch("theta",&th,"theta/D");
  t->Branch("polR",&pol,"polR/D");
  t->Branch("AN",&pow,"AN/D");

  ifstream fin(inm.c_str());
  char dm[500];
  fin.getline(dm,500);
  cout<<dm<<endl;
  while(fin>>ene>>th>>pow>>pol){
    th=acos(th)*180./3.1415926536;
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

  TH2D *eThDist=new TH2D("eThDist",";theta [deg]; log(energy) [MeV]",180,0,180,100,-1,3.5);
  //TH2D *anaPow =new TH2D("anaPow","Analyzing power;theta [deg]; log(energy) [MeV]",180,0,180,100,-5,3.5);

  t->Project("eThDist","log10(energy):acos(theta)*180./3.1415926536","energy>0");
  c1->Clear();
  c1->cd();
  
  t->Draw("theta : log10(energy) : AN","energy>0","colz");
  htemp->SetTitle("Analyzing power 100xMott; log10(energy) [MeV]; theta [deg]");
  htemp->Draw("colz");
  c1->Print(onm.c_str(),"pdf");
  
  t->Draw("theta : log10(energy) : AN","energy>0");
  c1->Print(onm.c_str(),"pdf");

  eThDist->Draw("colz");
  gPad->SetLogz(1);
  c1->Print(onm.c_str(),"pdf");
  gPad->SetLogz(0);
  
  // double ene,th,pol,pow;
  // int n=t->GetEntries();
  // t->SetBranchAddress("energy",&ene);
  // t->SetBranchAddress("theta",&th);
  // t->SetBranchAddress("polR",&pol);
  // t->SetBranchAddress("AN",&pow);

  // for(int i=0;i<n;i++){
  // }
  
  fin->Close();
}

