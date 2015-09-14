
void getDistributions(){

  string infile="../output/o_msc_L_500k.root";
  string oufile="../output/o_msc_L_500k_dist.root";
  
  string type[3]={"Pe","Ae","Ph"};
  string partType[3]={"trackID==1 && parentID==0 && pType==11","pType==11","pType==22"};
  TFile *fout=new TFile(oufile.c_str(),"RECREATE");
  TH1D *distXpos[3];
  TH1D *distXang[3];

  TFile *fin=TFile::Open(infile.c_str(),"READ");
  TTree *t=(TTree*)fin->Get("t");

  for(int i=0;i<3;i++){
    fout->cd();
    distXpos[i]=new TH1D(Form("distXpos%s",type[i].c_str()),"X position distribution primary e-;x pos [cm]",200,-100,100);    
    distXang[i]=new TH1D(Form("distXang%s",type[i].c_str()),"Angle along X distribution primary e-;ang X [deg]",180,-90,90);

    t->Project(distXpos[i]->GetName(),"prePosX/10",
	       Form("%s && (preE)>=2 && material==1 && prePosZ>30",partType[i].c_str()));
    int nev=t->Project(distXang[i]->GetName(),"preAngX"   ,
	       Form("%s && (preE)>=2 && material==1 && prePosZ>30",partType[i].c_str()));
    cout<<nev<<endl;
  }
  fin->Close();
  //cout<<"asdfa "<<distXpos[0]->GetEntries()<<" "<<endl<<distXpos[0]->GetDirectory()->GetPath()<<endl;
  fout->cd();
  for(int i=0;i<3;i++){
    //cout<<i<< distXpos[i]->GetName()<<" "<< distXpos[i]->GetDirectory()->GetPath()<<endl;
    distXpos[i]->Write();
    distXang[i]->Write();    
  }
  fout->Close();
  
}
