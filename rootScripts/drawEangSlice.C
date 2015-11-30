TCanvas *c1=new TCanvas("c1","c1",800,600);
string onm;
void drawEangSlice(){

  string infile="../output/depth15x2mm/o_dis_V_mottx1e2_1e6_15Sections.root";
  int z=9;
  onm="../output/depth15x2mm/y_EsliceAngle.pdf";
  c1->Print(Form("%s[",onm.c_str()),"pdf");
  drawOne(infile,z);
  c1->Print(Form("%s]",onm.c_str()),"pdf");

}

void drawOne(string infile,int z){
  TFile *fin=TFile::Open(infile.c_str(),"READ");

  TH2D *h=(TH2D*)fin->Get(Form("distEangPe_z%d",z));

  const int nSlice=6;
  double Eval[nSlice]={0,2,10,30,100,1160};
  TH1D *ha[nSlice-1];
  
  
  for(int i=0;i<nSlice-1;i++){
    ha[i]=new TH1D(Form("ha%d",i),Form("%.0f<E<%.0f",Eval[i],Eval[i+1]),
		   180,-90,90);    
  }

  int nbx=h->GetXaxis()->GetNbins();
  int nby=h->GetYaxis()->GetNbins();
  for(int i=1;i<=nbx;i++){
    double e=pow(10,h->GetXaxis()->GetBinCenter(i));
    int hn=-1;
    for(int ii=0;ii<nSlice-1;ii++)      
      if(e>=Eval[ii] && e<Eval[ii+1])
	hn=ii;
    if(hn==-1) {
      cout<<e<<" "<<endl;
      continue;
    }
    for(int j=1;j<=nby;j++){
      double ang=h->GetYaxis()->GetBinCenter(j);
      double val=h->GetBinContent(i,j);
      if(abs(ang)>90)
	cout<<ang<<" "<<i<<" "<<j<<" "<<h->GetYaxis()->GetBinCenter(j)<<" "<<h->GetXaxis()->GetBinCenter(i)<<endl;
      for(int kk=0;kk<val;kk++) ha[hn]->Fill(ang);
      //ha[hn]->Fill(ang,val);
    }
  }

  for(int i=0;i<nSlice-1;i++){
    ha[i]->Draw();
    gPad->SetGridx(1);
    gPad->SetGridy(1);
    c1->Print(onm.c_str(),"pdf");
  }
  
  fin->Close();
}
