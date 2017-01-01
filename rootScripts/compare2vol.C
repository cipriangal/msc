void compare2vol(){
  TFile *f1=TFile::Open("o_msc_simple_wTree_trk_1e4_002mm_V.root","READ");
  TFile *f2=TFile::Open("o_msc_2vol_wTree_trk_1e4_002mm_V.root","READ");

  TTree *t1=(TTree*)f1->Get("t");
  TTree *t2=(TTree*)f2->Get("t");

  // TCanvas *c1=new TCanvas("c1","c1");
  // t1->Draw("postPosX>>hp1","abs(postPosX)<1000 && abs(postAngX)<90 && postPosZ>15.5 && postPosZ<16");
  // hp1->SetLineColor(2);
  // hp1->DrawCopy();
  // t2->Draw("postPosX","abs(postPosX)<1000 && abs(postAngX)<90 && postPosZ>15.5 && postPosZ<16","same");
  
  // TCanvas *c2=new TCanvas("c2","c2");
  // t1->Draw("postAngX>>ha1","abs(postPosX)<1000 && abs(postAngX)<90 && postPosZ>15.5 && postPosZ<16");
  // ha1->SetLineColor(2);
  // ha1->DrawCopy();
  // t2->Draw("postAngX","abs(postPosX)<1000 && abs(postAngX)<90 && postPosZ>15.5 && postPosZ<16","same");

  // TCanvas *c3=new TCanvas("c3","c3");
  // t1->Draw("postPosX>>hp12","abs(postPosX)<1000 && abs(postAngX)<90 && postPosZ>14.5 && postPosZ<15");
  // hp12->SetLineColor(2);
  // t2->Draw("postPosX>>hp22","abs(postPosX)<1000 && abs(postAngX)<90 && postPosZ>14.5 && postPosZ<15","same");
  // hp12->DrawNormalized();
  // hp22->DrawNormalized("same");
  
  TCanvas *c4=new TCanvas("c4","c4");
  t1->Draw("postAngX>>ha12","abs(postPosX)<1000 && abs(postAngX)<90 && postPosZ>14.5 && postPosZ<15");
  ha12->SetLineColor(2);
  t2->Draw("postAngX>>ha22","abs(postPosX)<1000 && abs(postAngX)<90 && postPosZ>14.5 && postPosZ<15","same");
  ha12->DrawNormalized();
  ha22->DrawNormalized("same");
}
