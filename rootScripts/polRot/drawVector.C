void drawAxes(){
  TCanvas *c1=new TCanvas("c1","c1");

  TPolyLine3D *xAxis = new TPolyLine3D(2);
  xAxis->SetPoint(0,  0,  0,  0);
  xAxis->SetPoint(1,  2,  0,  0);
  
  TPolyLine3D *yAxis = new TPolyLine3D(2);
  yAxis->SetPoint(0,  0,  0,  0);
  yAxis->SetPoint(1,  0,  2,  0);
  
  TPolyLine3D *zAxis = new TPolyLine3D(2);
  zAxis->SetPoint(0,  0,  0,  0);
  zAxis->SetPoint(1,  0,  0,  2);

  xAxis->SetLineWidth(3);
  xAxis->SetLineColor(1);
  yAxis->SetLineWidth(3);
  yAxis->SetLineColor(1);
  zAxis->SetLineWidth(3);
  zAxis->SetLineColor(1);

  TPolyLine3D *xAxisDot = new TPolyLine3D(2);
  xAxisDot->SetPoint(0,  0,  0,  0);
  xAxisDot->SetPoint(1, -2,  0,  0);
  
  TPolyLine3D *yAxisDot = new TPolyLine3D(2);
  yAxisDot->SetPoint(0,  0,  0,  0);
  yAxisDot->SetPoint(1,  0, -2,  0);
  
  TPolyLine3D *zAxisDot = new TPolyLine3D(2);
  zAxisDot->SetPoint(0,  0,  0,  0);
  zAxisDot->SetPoint(1,  0,  0, -2);

  xAxisDot->SetLineWidth(3);
  xAxisDot->SetLineColor(1);
  xAxisDot->SetLineStyle(2);
  yAxisDot->SetLineWidth(3);
  yAxisDot->SetLineColor(1);
  yAxisDot->SetLineStyle(2);
  zAxisDot->SetLineWidth(3);
  zAxisDot->SetLineColor(1);
  zAxisDot->SetLineStyle(2);

  xAxis->Draw();
  yAxis->Draw();
  zAxis->Draw();
  xAxisDot->Draw();
  yAxisDot->Draw();
  zAxisDot->Draw();    
}

TPolyLine3D *createVector(double xi, double yi, double zi, double xf, double yf, double zf,
			  int color, double thickness, int styleVal, string nm){
  TPolyLine3D *aVector = new TPolyLine3D(2);
  //aVector->SetName(nm.c_str());
  aVector->SetPoint(0, xi, yi, zi);
  aVector->SetPoint(1, xi+xf, yi+yf, zi+zf);
  aVector->SetLineWidth(thickness);
  aVector->SetLineColor(color);
  aVector->SetLineStyle(styleVal);

  return aVector;
}
