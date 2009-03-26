
void makeSimonPositionFile() {

  double deltaRUpper=-0.042685;
  double deltaRLower=0.00653;
  double deltaRNadir=0.1927;
  
  double deltaTArrayMod[40]={0};
  double deltaR[40]={0};
  double deltaZ[40]={0};
  double deltaPhi[40]={0};
  
  deltaTArrayMod[9] = 0.00852165;
  deltaTArrayMod[1] = 0.0070049;
  deltaTArrayMod[10] = 0.235863;
  deltaTArrayMod[2] = -0.000183876;
  deltaTArrayMod[11] = 0.0993989;
  deltaTArrayMod[3] = 0.146733;
  deltaTArrayMod[12] = 0.422042;
  deltaTArrayMod[4] = 0.116907;
  deltaTArrayMod[13] = 0.108364;
  deltaTArrayMod[5] = 0.148237;
  deltaTArrayMod[14] = 0.186751;
  deltaTArrayMod[6] = 0.035624;
  deltaTArrayMod[15] = -0.0644544;
  deltaTArrayMod[7] = -0.0156711;
  deltaTArrayMod[8] = 0.129338;
  deltaTArrayMod[0] = -0.0279045;
  
  deltaTArrayMod[17] = 0.0418554;
  deltaTArrayMod[18] = 0.0529965;
  deltaTArrayMod[19] = 0.19453;
  deltaTArrayMod[20] = 0.103009;
  deltaTArrayMod[21] = 0.265183;
  deltaTArrayMod[22] = 0.335643;
  deltaTArrayMod[23] = 0.475472;
  deltaTArrayMod[24] = 0.525096;
  deltaTArrayMod[25] = 0.542914;
  deltaTArrayMod[26] = 0.449728;
  deltaTArrayMod[27] = 0.506337;
  deltaTArrayMod[28] = 0.236351;
  deltaTArrayMod[29] = 0.185779;
  deltaTArrayMod[30] = 0.167559;
  deltaTArrayMod[31] = 0.11412;
  deltaTArrayMod[16] = 0.0595093;
  

  deltaTArrayMod[0+32] = -0.33006;
  deltaTArrayMod[1+32] = -0.31323;
  deltaTArrayMod[2+32] = -0.0845225;
  deltaTArrayMod[3+32] = 0.234214;
  deltaTArrayMod[4+32] = 0.382551;
  deltaTArrayMod[5+32] = 0.232483;
  deltaTArrayMod[6+32] = -0.0292506;
  deltaTArrayMod[7+32] = -0.122308;
 
  
  //r, phi, deltaT
  deltaR[0+32] = 0.192096;
  deltaR[1+32] = 0.185281;
  deltaR[2+32] = 0.172963;
  deltaR[3+32] = 0.136439;
  deltaR[4+32] = 0.126339;
  deltaR[5+32] = 0.143903;
  deltaR[6+32] = 0.144807;
  deltaR[7+32] = 0.171424;
  
  deltaPhi[0+32] = -0.010578;
  deltaPhi[1+32] = 0.0020203;
  deltaPhi[2+32] = 0.0283709;
  deltaPhi[3+32] = 0.0171348;
  deltaPhi[4+32] = 0.00764585;
  deltaPhi[5+32] = -0.0151912;
  deltaPhi[6+32] = 0.00774264;
  deltaPhi[7+32] = 0.0128789;
  
  deltaZ[0+32] =0.4;
  deltaZ[1+32] =0.4;
  deltaZ[2+32] =0.4;
  deltaZ[3+32] =0.4;
  deltaZ[4+32] =0.4;
  deltaZ[5+32] =0.4;
  deltaZ[6+32] =0.4;
  deltaZ[7+32] =0.4;

  for(int ant=0;ant<16;ant++) {
    deltaR[ant]=deltaRUpper;
  }
  for(int ant=16;ant<32;ant++) {
    deltaR[ant]=deltaRLower;
  }

  ofstream SimonCalib("simonsPositionAndTimingOffsets.dat");
  SimonCalib << "#Ant\tdt\t\tdr\t\tdphi\tdz\n";
  for(int ant=0;ant<40;ant++) {
    SimonCalib << ant << "\t" << deltaTArrayMod[ant] << "\t"
	       << deltaR[ant] << "\t" << deltaPhi[ant] << "\t"
	       << deltaZ[ant] << "\n";
  }
  SimonCalib.close();
  
}
