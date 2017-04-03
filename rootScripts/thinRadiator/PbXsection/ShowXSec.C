void ShowXSec(Double_t Eb){
  // X0 6.37 g/cm2, rho = 11.35 g/cm3

  Double_t X0 = 6.37;

  // L = Nbeam * g/cm2 * N_A / A;
  Double_t PbA = 207.2;
  Double_t N_A = 6.02e23;

  Double_t Nbeam = 1e4;
  //  1e-24 cm^2 = 1 barn
  Double_t LumPerNrad = Nbeam * X0 * N_A / PbA * 1e-24;  // inverse barn

  // xsec in barn/str. 
  TString instr = Form("Pb_xsec_%.0f_MeV.dat",Eb);
  ifstream inF(instr.Data());

  Double_t th;
  Double_t q2;
  Double_t xsec;
  const Int_t ndat = 100;
  Double_t vth[ndat];
  Double_t vxs[ndat];
  Int_t id = 0;

  while(inF && id<ndat) {
    inF >> th >> q2 >> xsec;
    if (th>0.05) {
      vth[id] = th;
      vxs[id] = xsec;
      id++;
    }
  }

  TGraph* g1 = new TGraph(id,vth,vxs);
  g1->Draw("APL");
  c1->SetLogy();

}
