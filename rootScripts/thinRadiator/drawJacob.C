void drawJacob(string fnm="../../output/thinRadiator/o_msc_L001um_03MeV_1e5ev.root"){
  TFile *fin=TFile::Open(fnm.c_str(),"READ");
  TTree *t=(TTree*)fin->Get("t");
  int material, pType, trackID, parentID;
  double pX, pY, pZ;
  
  t->SetBranchAddress("pType", &pType);
  t->SetBranchAddress("material", &material);
  t->SetBranchAddress("trackID", &trackID);
  t->SetBranchAddress("parentID", &parentID);
  t->SetBranchAddress("postMomX", &pX);
  t->SetBranchAddress("postMomY", &pY);
  t->SetBranchAddress("postMomZ", &pZ);

  int nev = t->GetEntries();
  TH1D *scatAng = new TH1D("scatAng","Rate/sin(theta); scattering angle [deg]", 400,0,100);
  double pi = acos(-1);
  cout<<"pi "<<pi<<endl;
  for(int i=0;i<nev;i++){
    t->GetEntry(i);

    if(pType!=11) continue;
    if(material != 1) continue;
    if(trackID != 1 || parentID!=0) continue;

    double r = sqrt( pX*pX + pY*pY + pZ*pZ);
    if(r==0) continue;

    double thetaRad = acos(pZ/r);
    double theta = thetaRad * 180/pi;
    if(theta<0.01) continue;
    
    scatAng->Fill(theta, 1./sin(thetaRad));
    
  }
  gStyle->SetOptStat("eMRou");
  scatAng->DrawCopy();
  fin->Close();
}
