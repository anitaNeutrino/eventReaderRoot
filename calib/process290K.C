
void process290K() {
  ifstream PedVals("290Kofs.txt");
  char crapLine[180];
  PedVals.getline(crapLine,179);
  Double_t polVals[40][2];
  Int_t ant;
  Double_t val1,val2;
  while(PedVals >> ant >> val1 >> val2) {
    polVals[ant-1][1]=val1;
    polVals[ant-1][0]=val2 ;          
  }

  cout << "Double_t rfPowPedestals[40][2]={";
  for(int ant=0;ant<40;ant++) {
    cout << "{" << polVals[ant][0] << "," << polVals[ant][1]
	 << "}";
    if(ant<39) 
      cout << ",";
  }
  cout << "};\n";

  


}
