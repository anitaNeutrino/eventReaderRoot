
void makeSimonPositionFile() {

  double deltaRUpper=-0.042685;
  double deltaRLower=0.00653;
  double deltaRNadir=0.1927;
  
  double deltaTArrayMod[40]={0};
  double deltaR[40]={0};
  double deltaZ[40]={0};
  double deltaPhi[40]={0};
 deltaTArrayMod[0] = 0.292591; deltaR[0] = -0.0384839; deltaPhi[ 0] = -0.0100608; deltaZ[0] = 0;
deltaTArrayMod[1] = 0.172375; deltaR[1] = 0.00634697; deltaPhi[ 1] = -0.00313443; deltaZ[1] = 0;
deltaTArrayMod[2] = 0.342454; deltaR[2] = -0.0861167; deltaPhi[ 2] = -0.015312; deltaZ[2] = 0;
deltaTArrayMod[3] = 0.0248334; deltaR[3] = 0.0461873; deltaPhi[ 3] = 0.00206827; deltaZ[3] = 0;
deltaTArrayMod[4] = 0.0372539; deltaR[4] = 0.0153388; deltaPhi[ 4] = -0.0227948; deltaZ[4] = 0;
deltaTArrayMod[5] = 0.137506; deltaR[5] = -0.00927728; deltaPhi[ 5] = 0.00750385; deltaZ[5] = 0;
deltaTArrayMod[6] = 0.0475841; deltaR[6] = 0.0239867; deltaPhi[ 6] = 0.00388065; deltaZ[6] = 0;
deltaTArrayMod[7] = 0.123741; deltaR[7] = 0.0125282; deltaPhi[ 7] = -0.00131021; deltaZ[7] = 0;
deltaTArrayMod[8] = 0.380551; deltaR[8] = -0.0111636; deltaPhi[ 8] = -0.0299233; deltaZ[8] = 0;
deltaTArrayMod[9] = 0.445956; deltaR[9] = -0.0959452; deltaPhi[ 9] = -0.00165365; deltaZ[9] = 0;
deltaTArrayMod[10] = 0.439757; deltaR[10] = -0.0330808; deltaPhi[ 10] = -0.0107407; deltaZ[10] = 0;
deltaTArrayMod[11] = 0.244031; deltaR[11] = -0.0475617; deltaPhi[ 11] = 0.0145914; deltaZ[11] = 0;
deltaTArrayMod[12] = 0.321419; deltaR[12] = 0.0196292; deltaPhi[ 12] = -0.0150373; deltaZ[12] = 0;
deltaTArrayMod[13] = 0.118473; deltaR[13] = -0.0190837; deltaPhi[ 13] = -0.0121967; deltaZ[13] = 0;
deltaTArrayMod[14] = 0.27363; deltaR[14] = -0.00922367; deltaPhi[ 14] = -0.0038106; deltaZ[14] = 0;
deltaTArrayMod[15] = 0.187377; deltaR[15] = -0.0294811; deltaPhi[ 15] = 0.0106842; deltaZ[15] = 0;
deltaTArrayMod[16] = 0.0682454; deltaR[16] = 0.0140245; deltaPhi[ 16] = -0.0087849; deltaZ[16] = 0;
deltaTArrayMod[17] = 0.296899; deltaR[17] = -0.0621836; deltaPhi[ 17] = 0.000682206; deltaZ[17] = 0;
deltaTArrayMod[18] = 0.180588; deltaR[18] = -0.0379325; deltaPhi[ 18] = -0.00516052; deltaZ[18] = 0;
deltaTArrayMod[19] = 0.165257; deltaR[19] = -0.0108062; deltaPhi[ 19] = -0.00770935; deltaZ[19] = 0;
deltaTArrayMod[20] = 0.176423; deltaR[20] = -0.0601935; deltaPhi[ 20] = -0.00862535; deltaZ[20] = 0;
deltaTArrayMod[21] = 0.301777; deltaR[21] = -0.0968276; deltaPhi[ 21] = -0.00920648; deltaZ[21] = 0;
deltaTArrayMod[22] = 0.0969425; deltaR[22] = -0.0348523; deltaPhi[ 22] = 0.00037431; deltaZ[22] = 0;
deltaTArrayMod[23] = 0.0422886; deltaR[23] = 0.0121726; deltaPhi[ 23] = 0.00310935; deltaZ[23] = 0;
deltaTArrayMod[24] = -0.01737; deltaR[24] = 0.0405193; deltaPhi[ 24] = -0.00546085; deltaZ[24] = 0;
deltaTArrayMod[25] = 0.0412981; deltaR[25] = 0.0239992; deltaPhi[ 25] = -0.00901249; deltaZ[25] = 0;
deltaTArrayMod[26] = 0.166311; deltaR[26] = -0.0405203; deltaPhi[ 26] = -0.0145529; deltaZ[26] = 0;
deltaTArrayMod[27] = 0.139405; deltaR[27] = -0.00401756; deltaPhi[ 27] = -0.00666063; deltaZ[27] = 0;
deltaTArrayMod[28] = 0.104575; deltaR[28] = -0.0362955; deltaPhi[ 28] = -0.00372999; deltaZ[28] = 0;
deltaTArrayMod[29] = 0.0345155; deltaR[29] = -0.00587152; deltaPhi[ 29] = 0.00197442; deltaZ[29] = 0;
deltaTArrayMod[30] = 0.082859; deltaR[30] = -0.00611182; deltaPhi[ 30] = -0.000789595; deltaZ[30] = 0;
deltaTArrayMod[31] = 0.078192; deltaR[31] = -0.00321244; deltaPhi[ 31] = 0.000188257; deltaZ[31] = 0;
deltaTArrayMod[32] = 0.0143052; deltaR[32] = -0.0437687; deltaPhi[ 32] = -0.00289577; deltaZ[32] = 0;
deltaTArrayMod[33] = 0.0671887; deltaR[33] = -0.0643475; deltaPhi[ 33] = -0.0203117; deltaZ[33] = 0;
deltaTArrayMod[34] = 0.0819606; deltaR[34] = -0.0804245; deltaPhi[ 34] = -0.00503387; deltaZ[34] = 0;
deltaTArrayMod[35] = -0.0659071; deltaR[35] = -0.0112675; deltaPhi[ 35] = -0.000220575; deltaZ[35] = 0;
deltaTArrayMod[36] = -0.212624; deltaR[36] = 0.0337428; deltaPhi[ 36] = -0.00416114; deltaZ[36] = 0;
deltaTArrayMod[37] = -0.00431668; deltaR[37] = -0.0525977; deltaPhi[ 37] = -0.0223176; deltaZ[37] = 0;
deltaTArrayMod[38] = 0.0899397; deltaR[38] = -0.101587; deltaPhi[ 38] = 0.0058874; deltaZ[38] = 0;
deltaTArrayMod[39] = 0.0294537; deltaR[39] = -0.0401037; deltaPhi[ 39] = 0.00899651; deltaZ[39] = 0;
Double_t antTimeDiff[40]={-0.0268622,-0.0265958,0.0109199,0.0122393,0.00685474,0.0302896,-0.0177799,-0.0034176,-0.0370968,-0.0327585,-0.00875321,0.00560383,0.00339023,-0.00614045,-0.0314993,-0.0223937,-0.0273816,-0.0141105,-0.0167223,-0.0211678,-0.0215471,0.0246236,0.0231716,0.0384039,0.0060008,0.00974318,0.0170899,0.0180993,-0.032741,-0.00978817,-0.0056724,0.0119987,-0.0420788,-0.0429052,-0.0353696,-0.0462439,-0.0428311,-0.0391794,-0.0435896,-0.0438023};

  ofstream SimonCalib("simonsPositionAndTimingOffsets.dat");
  SimonCalib << "#Ant\tdt\t\tdr\t\tdphi\tdz\n";
  for(int ant=0;ant<40;ant++) {
    SimonCalib << ant << "\t" << (deltaTArrayMod[ant]+antTimeDiff[ant]) << "\t"
	       << deltaR[ant] << "\t" << deltaPhi[ant] << "\t"
	       << deltaZ[ant] << "\n";
  }
  SimonCalib.close();
  
}
