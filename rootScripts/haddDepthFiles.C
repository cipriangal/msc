void haddDepthFiles(){
  int start=200;
  int stop=300;
  string fout="../output/o_msc_V_15xmott1e2_5e7.root";

  string hadd="hadd "+fout;
  for(int i=start;i<stop;i++)
    hadd+=Form(" /lustre/expphy/volatile/hallc/qweak/ciprian/farmoutput/msc/mott/1e2/V_015_%03d/o_msc.root",i);

  cout<<hadd<<" "<<hadd.length()<<endl;
  gSystem->Exec(hadd.c_str());
}
