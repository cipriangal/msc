TCanvas *c1=new TCanvas("c1","c1",1400,600);
string onm;
const int units=15;

void depthAna(){

  string infile="../output/depth15x2mm/o_msc_V_mottx1e2_1e6_15Sections.root";
  string oufile="../output/depth15x2mm/o_dis_V_mottx1e2_1e6_15Sections.root";
  onm="../output/depth15x2mm/y_dis_V_mottx1e2_1e6_15Sections.pdf";
  c1->Print(Form("%s[",onm.c_str()),"pdf");
  getDist(infile,oufile);
  calcLightAsym(oufile);
  c1->Print(Form("%s]",onm.c_str()),"pdf");
  
  // string infile="../output/depth11x2mm/o_msc_L_5e5_11Sections.root";
  // string oufile="../output/depth11x2mm/o_dis_L_5e5_11Sections.root";
  // onm="../output/depth11x2mm/y_dis_L_5e5_11Sections.pdf";
  // c1->Print(Form("%s[",onm.c_str()),"pdf");
  // //getDist(infile,oufile);
  // calcLightAsym(oufile);
  // c1->Print(Form("%s]",onm.c_str()),"pdf");

  // string infile="../output/depth11x2mm/o_msc_V_mottx1e2_1e6_11Sections.root";
  // string oufile="../output/depth11x2mm/o_dis_V_mottx1e2_1e6_11Sections.root";
  // onm="../output/depth11x2mm/y_dis_V_mottx1e2_1e6_11Sections.pdf";
  // c1->Print(Form("%s[",onm.c_str()),"pdf");
  // getDist(infile,oufile);
  // calcLightAsym(oufile);
  // c1->Print(Form("%s]",onm.c_str()),"pdf");
  
  // string infile="../output/depth11x2mm/o_msc_V_2phx1e4_1e5_11Sections.root";
  // string oufile="../output/depth11x2mm/o_dis_V_2phx1e4_1e5_11Sections.root";
  // onm="../output/depth11x2mm/y_dis_V_2phx1e4_1e5_11Sections.pdf";
  // c1->Print(Form("%s[",onm.c_str()),"pdf");
  // getDist(infile,oufile);
  // calcLightAsym(oufile);
  // c1->Print(Form("%s]",onm.c_str()),"pdf");

  
}


void getDist(string infile, string oufile){
  
  const string type[3]={"Pe","Ae","Ph"};
  const string tit[3]={"Primary e-","All e","Photons"};
  const string partType[3]={"trackID==1 && parentID==0 && pType==11","pType==11","pType==22"};

  TFile *fout=new TFile(oufile.c_str(),"RECREATE");
  TH2D *distY[3][units];
  
  TFile *fin=TFile::Open(infile.c_str(),"READ");
  TTree *t=(TTree*)fin->Get("t");

  for(int j=0;j<units;j++){
    double zpos=2.+2.1*j;

    for(int i=0;i<3;i++){
      fout->cd();
      distY[i][j]=new TH2D(Form("distY%s_z%d",type[i].c_str(),j),
			   Form("%s E>2MeV;y pos [cm];y angle [deg]",tit[i].c_str()),
			   200,-15,15,
			   180,-90,90);    
      cout<<distY[i][j]->GetName()<<endl;
      t->Project(distY[i][j]->GetName(),"preAngX:projPosX/10",
		 Form("%s && preE>=2 && material==1 && unitNo==%d && abs(prePosZ-%f)<0.05",
		      partType[i].c_str(),j,zpos));
    }

  }

  fin->Close();
  fout->cd();

  for(int j=0;j<units;j++)
    for(int i=0;i<3;i++){
      distY[i][j]->Write();
    }

  
  fout->Close();  
}

void calcLightAsym(string infile){

  gStyle->SetOptStat(1211);
  gStyle->SetOptFit(1);

  TFile *fin=TFile::Open(infile.c_str(),"READ");
  
  TGraphErrors *g[3];
  TH1D *asym[3][units];
  const string type[3]={"Pe","Ae","Ph"};
  const string tit[3]={"Primary e-","All e","Photons"};
  
  for(int i=0;i<3;i++){
    g[i]=new TGraphErrors();
    g[i]->SetName(Form("g_%s",type[i].c_str()));
    g[i]->SetTitle(Form("%s;z position [mm]; light imbalance",tit[i].c_str()));
    
    for(int j=0;j<units;j++){
      asym[i][j]=new TH1D(Form("asym_%s_%d",type[i].c_str(),j),"light imbalance",
			  201,-1.2,1.2);

      TH2D *distY=(TH2D*)fin->Get(Form("distY%s_z%d",type[i].c_str(),j));
      calcMean(distY,asym[i][j]);
      
      double zpos=2.+2.1*j;
      g[i]->SetPoint(j,zpos,asym[i][j]->GetMean());
      g[i]->SetPointError(j,0,asym[i][j]->GetMeanError());
	
      c1->Clear();
      c1->cd(0);
      c1->Divide(2);
      c1->cd(1);  
      distY->GetXaxis()->SetRangeUser(-20,20);
      distY->GetYaxis()->SetRangeUser(-35,35);
      distY->GetZaxis()->SetRangeUser(1,200000);
      distY->SetStats(0);
      distY->DrawCopy("colz");
      gPad->SetLogz(1);
      gPad->SetGridx(1);
      gPad->SetGridy(1);
      c1->cd(2);
      asym[i][j]->DrawCopy();
      gPad->SetLogy(1);
      c1->Print(onm.c_str(),"pdf");
    }

    c1->Clear();
    g[i]->SetMarkerStyle(20);
    g[i]->Fit("pol0");
    g[i]->Draw("APL");
    gPad->SetGridy(1);
    c1->Print(onm.c_str(),"pdf");
    
  }

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
      
      if(fabs(xpos)>=15 || fabs(xang)>=35) continue;
      
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
