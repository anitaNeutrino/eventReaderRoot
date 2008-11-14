

void plotSquareWave()
{
   TF1 *sqaurey = new TF1("sqaurey",funcSquareWave,0,100,5);
   sqaurey->SetParameters(0,200,-200,30,30);
   sqaurey->Draw();
}







Double_t funcSquareWave(Double_t *x, Double_t *par)
{
   Double_t phi=par[0];
   Double_t a=par[1];
   Double_t b=par[2];
   Double_t dtLow=par[3];
   Double_t dtHigh=par[4];

   Double_t period=dtLow+dtHigh;

   Double_t t=x[0]-phi;
   
   while(t<period) {
      t+=period;
   }
   while(t>period) {
      t-=period;
   }
   if(t<dtLow)
      return b;
   return a;



}
