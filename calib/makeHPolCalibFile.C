
void makeHPolCalibFile() {

   double deltaTArrayMod[40]={0};
   double deltaR[40]={0};
   double deltaZ[40]={0};
   double deltaPhi[40]={0};

   deltaTArrayMod[0] = -0.149023; deltaR[0] = 0; deltaPhi[ 0] = 0; deltaZ[0] = 0;
   deltaTArrayMod[1] = -0.0910482; deltaR[1] = 0; deltaPhi[ 1] = 0; deltaZ[1] = 0;
   deltaTArrayMod[2] = -0.124114; deltaR[2] = 0; deltaPhi[ 2] = 0; deltaZ[2] = 0;
   deltaTArrayMod[3] = -0.0137028; deltaR[3] = 0; deltaPhi[ 3] = 0; deltaZ[3] = 0;
   deltaTArrayMod[4] = -0.0603679; deltaR[4] = 0; deltaPhi[ 4] = 0; deltaZ[4] = 0;
   deltaTArrayMod[5] = -0.101831; deltaR[5] = 0; deltaPhi[ 5] = 0; deltaZ[5] = 0;
   deltaTArrayMod[6] = -0.0914806; deltaR[6] = 0; deltaPhi[ 6] = 0; deltaZ[6] = 0;
   deltaTArrayMod[7] = -0.152871; deltaR[7] = 0; deltaPhi[ 7] = 0; deltaZ[7] = 0;
   deltaTArrayMod[8] = -0.0501338; deltaR[8] = 0; deltaPhi[ 8] = 0; deltaZ[8] = 0;
   deltaTArrayMod[9] = -0.0963791; deltaR[9] = 0; deltaPhi[ 9] = 0.000158312; deltaZ[9] = 0;
   deltaTArrayMod[10] = -0.185055; deltaR[10] = 0; deltaPhi[ 10] = 0; deltaZ[10] = 0;
   deltaTArrayMod[11] = -0.146535; deltaR[11] = 0; deltaPhi[ 11] = 0; deltaZ[11] = 0;
   deltaTArrayMod[12] = -0.0564678; deltaR[12] = 0; deltaPhi[ 12] = 0; deltaZ[12] = 0;
   deltaTArrayMod[13] = -0.326708; deltaR[13] = 0; deltaPhi[ 13] = 0; deltaZ[13] = 0;
   deltaTArrayMod[14] = -0.243719; deltaR[14] = 0; deltaPhi[ 14] = 0; deltaZ[14] = 0;
   deltaTArrayMod[15] = -0.0248596; deltaR[15] = 0; deltaPhi[ 15] = 0; deltaZ[15] = 0;
   deltaTArrayMod[16] = -0.0293789; deltaR[16] = 0; deltaPhi[ 16] = 0; deltaZ[16] = 0;
   deltaTArrayMod[17] = -0.0911007; deltaR[17] = 0; deltaPhi[ 17] = 0; deltaZ[17] = 0;
   deltaTArrayMod[18] = -0.140347; deltaR[18] = 0; deltaPhi[ 18] = 0; deltaZ[18] = 0;
   deltaTArrayMod[19] = -0.161398; deltaR[19] = 0; deltaPhi[ 19] = 0; deltaZ[19] = 0;
   deltaTArrayMod[20] = -0.0837762; deltaR[20] = -0.0479999; deltaPhi[ 20] = 0; deltaZ[20] = 0;
   deltaTArrayMod[21] = -0.120312; deltaR[21] = 0; deltaPhi[ 21] = 0; deltaZ[21] = 0;
   deltaTArrayMod[22] = -0.150723; deltaR[22] = 0; deltaPhi[ 22] = 0; deltaZ[22] = 0;
   deltaTArrayMod[23] = -0.13717; deltaR[23] = 0; deltaPhi[ 23] = 0; deltaZ[23] = 0;
   deltaTArrayMod[24] = -0.0459388; deltaR[24] = -0.000824448; deltaPhi[ 24] = 0; deltaZ[24] = 0;
   deltaTArrayMod[25] = -0.127368; deltaR[25] = 0; deltaPhi[ 25] = 0; deltaZ[25] = 0;
   deltaTArrayMod[26] = -0.195026; deltaR[26] = 0; deltaPhi[ 26] = 0; deltaZ[26] = 0;
   deltaTArrayMod[27] = -0.0732175; deltaR[27] = 0; deltaPhi[ 27] = 0; deltaZ[27] = 0;
   deltaTArrayMod[28] = -0.285423; deltaR[28] = 0; deltaPhi[ 28] = 0; deltaZ[28] = 0;
   deltaTArrayMod[29] = -0.170697; deltaR[29] = 0; deltaPhi[ 29] = 0; deltaZ[29] = 0;
   deltaTArrayMod[30] = -0.102262; deltaR[30] = 0; deltaPhi[ 30] = 0; deltaZ[30] = 0;
   deltaTArrayMod[31] = -0.134849; deltaR[31] = 0; deltaPhi[ 31] = 0; deltaZ[31] = 0;
   deltaTArrayMod[32] = -1.14515; deltaR[32] = 0; deltaPhi[ 32] = 0; deltaZ[32] = 0;
   deltaTArrayMod[33] = -0.956054; deltaR[33] = 0; deltaPhi[ 33] = 0; deltaZ[33] = 0;
   deltaTArrayMod[34] = -0.998455; deltaR[34] = 0; deltaPhi[ 34] = 0; deltaZ[34] = 0;
   deltaTArrayMod[35] = -1.06894; deltaR[35] = 0; deltaPhi[ 35] = 0; deltaZ[35] = 0;
   deltaTArrayMod[36] = -0.983545; deltaR[36] = 0; deltaPhi[ 36] = 0; deltaZ[36] = 0;
   deltaTArrayMod[37] = -1.03014; deltaR[37] = 0; deltaPhi[ 37] = 0; deltaZ[37] = 0;
   deltaTArrayMod[38] = -1.03878; deltaR[38] = 0; deltaPhi[ 38] = 0; deltaZ[38] = 0;
   deltaTArrayMod[39] = -1.04706; deltaR[39] = 0; deltaPhi[ 39] = 0; deltaZ[39] = 0;

   //   Double_t antTimeDiff[40]={0};
   Double_t antTimeDiff[40]={0.0397616,0.0315506,0.0279627,0.00546607,0.00162207,-0.00794248,-0.0179397,0.0301225,0.023019,0.0162649,0.0607524,0.0218439,0.00920753,0.00270723,-0.00270522,-0.00169312,-0.000788524,-0.00452435,0.00583965,0.00372261,-0.0970163,0.0248889,0.0281113,0.0277912,0.0232382,0.0270256,0.0111319,0.0010349,-0.00403086,-0.0119509,-0.0147447,-0.0197287,0.044283,0.0228882,0.0372106,0.0496755,0.0508683,0.0370313,0.0458517,0.0481914};
   ofstream SimonCalib("simonsHPolPositionAndTimingOffsets.dat");
   SimonCalib << "#Ant\tdt\t\tdr\t\tdphi\tdz\n";
   for(int ant=0;ant<40;ant++) {
      SimonCalib << ant << "\t" << (deltaTArrayMod[ant]+antTimeDiff[ant]) << "\t"
		 << deltaR[ant] << "\t" << deltaPhi[ant] << "\t"
		 << deltaZ[ant] << "\n";
   }
   SimonCalib.close();
  
}
