TCanvas *c1=new TCanvas("c1","c1",1400,600);
string onm;
const int units=15;

void depth3DHistAna(){

  string infile="../output/depth15x2mm/o_msc_V_15xmott1e2_5e7.root";
  onm="../output/depth15x2mm/y_dis_V_15xmottx1e2_5e7_Pe_10.pdf";
  // string infile="../o_msc.root";
  // onm="../y_dis.pdf";
  c1->Print(Form("%s[",onm.c_str()),"pdf");
  calcLightAsym(infile);
  c1->Print(Form("%s]",onm.c_str()),"pdf");
  
}


void calcLightAsym(string infile){

  gStyle->SetOptStat(1211);
  gStyle->SetOptFit(1);

  TFile *fin=TFile::Open(infile.c_str(),"READ");
  
  TGraphErrors *g;
  TH1D *asym[units];
  
  g=new TGraphErrors();
  g->SetName("g_Pe");
  g->SetTitle("Primary e;z position [mm]; light imbalance");

  TH3D *h=(TH3D*)fin->Get("hPosAngUnit_Pe");
  int nbx=h->GetXaxis()->GetNbins();
  // double bx1=h->GetXaxis()->GetBinCenter(1) - h->GetXaxis()->GetBinWidth(1)/2.;
  // double bxn=h->GetXaxis()->GetBinCenter(nbx) - h->GetXaxis()->GetBinWidth(nbx)/2.;
  double bx1=h->GetXaxis()->GetXmin();
  double bxn=h->GetXaxis()->GetXmax();
  cout<<nbx<<" "<<bx1<<" "<<bxn<<endl;
  int nby=h->GetYaxis()->GetNbins();
  double by1=h->GetYaxis()->GetXmin();
  double byn=h->GetYaxis()->GetXmax();
  // double by1=h->GetYaxis()->GetBinCenter(1) - h->GetYaxis()->GetBinWidth(1)/2.;
  // double byn=h->GetYaxis()->GetBinCenter(nby) - h->GetYaxis()->GetBinWidth(nby)/2.;
  cout<<nby<<" "<<by1<<" "<<byn<<endl;

  for(int j=0;j<units;j++){
    asym[j]=new TH1D(Form("asym_Pe_%d",j),"light imbalance",
		     201,-1.2,1.2);

    TH2D *distY=new TH2D("distY","2D dist;pos [cm]; angle [deg]",nbx,bx1,bxn,nby,by1,byn);

    for(int ii=1;ii<=nbx;ii++)
      for(int jj=1;jj<=nby;jj++){
	//int bx=distY->GetXaxis()->FindBin(h->GetXaxis()->GetBinCenter(ii)/10.);
	distY->SetBinContent(ii,jj,h->GetBinContent(ii,jj,j+1));
      }
    
    calcMean(distY,asym[j]);
      
    double zpos=2.+2.1*j;
    g->SetPoint(j,zpos,asym[j]->GetMean());
    g->SetPointError(j,0,asym[j]->GetMeanError());
	
    c1->Clear();
    c1->cd(0);
    c1->Divide(2);
    c1->cd(1);  
    distY->GetXaxis()->SetRangeUser(-15,15);
    // distY->GetYaxis()->SetRangeUser(-35,35);
    //distY->GetZaxis()->SetRangeUser(1,200000);
    //distY->SetStats(0);
    distY->DrawCopy("colz");
    gPad->SetLogz(1);
    gPad->SetGridx(1);
    gPad->SetGridy(1);
    c1->cd(2);
    asym[j]->DrawCopy();
    gPad->SetLogy(1);
    c1->Print(onm.c_str(),"pdf");

    delete distY;
  }

  c1->Clear();
  g->SetMarkerStyle(20);
  //g[i]->Fit("pol0");
  g->SetMinimum(0.);//Pe
  g->SetMaximum(0.023);//Pe
  // g->SetMinimum(-0.0002);//Ae
  // g->SetMaximum(0.003);//Ae
  g->Draw("APL");
  gPad->SetGridy(1);
  c1->Print(onm.c_str(),"pdf");
  
  fin->Close();
  
}

void calcMean(TH2D *a,TH1D *dd){

  int binx=a->GetXaxis()->GetNbins();
  int biny=a->GetYaxis()->GetNbins();

  for(int i=1;i<=binx;i++)
    for(int j=1;j<=biny;j++){
      double nentries=a->GetBinContent(i,j);
      double xpos=a->GetXaxis()->GetBinCenter(i);
      double xang=a->GetYaxis()->GetBinCenter(j);
      
      //if(fabs(xpos)>=15 || fabs(xang)>=35) continue;//default
      if(fabs(xpos)>=15 || fabs(xang)<0 || fabs(xang)>=10) continue;
      
      if(nentries>0){      
	double asym=getAsym(xpos,xang);
	for(int kk=0;kk<nentries;kk++) dd->Fill(asym);
      }
    }    
}

double getAsym(double pos,double ang){
  //double posA=(posAsym->Eval(pos,0,"S") - posAsym->Eval(-pos,0,"S"))/2;
  //double angA=(angAsym->Eval(ang,0,"S") - angAsym->Eval(-ang,0,"S"))/2;
  
  double posA=0.006351*pos;
  double angA(0);
  if(fabs(ang)<10)
    angA=ang*0.065;
  else if(fabs(ang)<20)
    angA=ang*0.016+0.49*ang/fabs(ang);
  else
    angA=0.81*ang/fabs(ang);
  //cout<<pos<<" ==> "<<posA<<" ~~~ "<<ang<<" ==> "<<angA<<endl;
  return posA+angA;
}
