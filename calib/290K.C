
void process290K() {
  ifstream PedVals("289Kofs.txt");
  char crapLine[180];
  PedVals.getline(crapLine,179);
  Double_t polVals[4][2];
  Int_t ant;
  Double_t val1,val2;
  while(PedVals >> ant >> val1 >> val2) {
    polVals[ant-1][1]=val1
    polVals[ant-1][0]=val2           
  }

  cout << "Double_t rfPowPedestals[40][20]={";
  for(int ant=0;ant<40;ant++) {
    cout << "{" << polVals[ant][0] << "," << polVals[ant][1]
	 << "}";
  }
  cout << "};\n";

  


}
