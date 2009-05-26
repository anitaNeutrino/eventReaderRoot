
void makeSimonPositionFile() {

  double deltaRUpper=-0.042685;
  double deltaRLower=0.00653;
  double deltaRNadir=0.1927;
  
  double deltaTArrayMod[40]={0};
  double deltaR[40]={0};
  double deltaZ[40]={0};
  double deltaPhi[40]={0};
  deltaTArrayMod[0] = 0.16044; deltaR[0] = -0.0618352; deltaPhi[ 0] = -0.00280991; deltaZ[0] = 0.0287513;
deltaTArrayMod[1] = 0.290365; deltaR[1] = -0.110188; deltaPhi[ 1] = -0.00372876; deltaZ[1] = 0.0170052;
deltaTArrayMod[2] = 0.349607; deltaR[2] = -0.140583; deltaPhi[ 2] = -0.024769; deltaZ[2] = 0.0623979;
deltaTArrayMod[3] = 0.033917; deltaR[3] = -0.0260668; deltaPhi[ 3] = 0.00113633; deltaZ[3] = 0.0423352;
deltaTArrayMod[4] = -0.074643; deltaR[4] = -0.0356161; deltaPhi[ 4] = -0.0141947; deltaZ[4] = -0.00394529;
deltaTArrayMod[5] = 0.156259; deltaR[5] = -0.095648; deltaPhi[ 5] = 0.0117896; deltaZ[5] = 0.0105241;
deltaTArrayMod[6] = 0.212836; deltaR[6] = -0.0907411; deltaPhi[ 6] = -0.0122972; deltaZ[6] = 0.0516748;
deltaTArrayMod[7] = 0.541271; deltaR[7] = -0.157934; deltaPhi[ 7] = -0.00368077; deltaZ[7] = 0.130464;
deltaTArrayMod[8] = -0.128575; deltaR[8] = 0.0582477; deltaPhi[ 8] = -0.0111813; deltaZ[8] = -0.035067;
deltaTArrayMod[9] = -0.122485; deltaR[9] = -0.0119241; deltaPhi[ 9] = -0.00668961; deltaZ[9] = -0.0579302;
deltaTArrayMod[10] = -0.263916; deltaR[10] = 0.0811694; deltaPhi[ 10] = -0.00717066; deltaZ[10] = -0.0897962;
deltaTArrayMod[11] = -0.229033; deltaR[11] = 0.00897635; deltaPhi[ 11] = -5.65709e-05; deltaZ[11] = -0.0366053;
deltaTArrayMod[12] = -0.222093; deltaR[12] = 0.0800239; deltaPhi[ 12] = -0.00901442; deltaZ[12] = -0.0750748;
deltaTArrayMod[13] = -0.68459; deltaR[13] = 0.108285; deltaPhi[ 13] = -0.0148342; deltaZ[13] = -0.133195;
deltaTArrayMod[14] = -0.516002; deltaR[14] = 0.107793; deltaPhi[ 14] = -0.00133016; deltaZ[14] = -0.143198;
deltaTArrayMod[15] = -0.749822; deltaR[15] = 0.145538; deltaPhi[ 15] = -0.00448024; deltaZ[15] = -0.118927;
deltaTArrayMod[16] = 0.0961751; deltaR[16] = -0.0564912; deltaPhi[ 16] = -0.00220892; deltaZ[16] = 0.0532255;
deltaTArrayMod[17] = -0.166225; deltaR[17] = 0.00298098; deltaPhi[ 17] = 0.00128378; deltaZ[17] = -0.0263557;
deltaTArrayMod[18] = 0.178941; deltaR[18] = -0.0972098; deltaPhi[ 18] = -0.00837075; deltaZ[18] = 0.0461125;
deltaTArrayMod[19] = 0.171386; deltaR[19] = -0.0643506; deltaPhi[ 19] = -0.00831496; deltaZ[19] = 0.0592813;
deltaTArrayMod[20] = -0.0204964; deltaR[20] = -0.0803763; deltaPhi[ 20] = -0.00428327; deltaZ[20] = -0.00602899;
deltaTArrayMod[21] = 0.160323; deltaR[21] = -0.101956; deltaPhi[ 21] = -0.0151547; deltaZ[21] = 0.0514858;
deltaTArrayMod[22] = 0.053953; deltaR[22] = -0.0773129; deltaPhi[ 22] = -0.00414444; deltaZ[22] = 0.0562807;
deltaTArrayMod[23] = 0.00325878; deltaR[23] = -0.0378326; deltaPhi[ 23] = 0.000528826; deltaZ[23] = 0.0517299;
deltaTArrayMod[24] = -0.0670197; deltaR[24] = -0.0225655; deltaPhi[ 24] = -0.00391848; deltaZ[24] = 0.0263552;
deltaTArrayMod[25] = 0.0035097; deltaR[25] = -0.0427118; deltaPhi[ 25] = -0.00285503; deltaZ[25] = 0.0211308;
deltaTArrayMod[26] = -0.0337735; deltaR[26] = -0.062921; deltaPhi[ 26] = -0.00747562; deltaZ[26] = -0.0138518;
deltaTArrayMod[27] = -0.110361; deltaR[27] = -0.0144298; deltaPhi[ 27] = -0.0028118; deltaZ[27] = -0.0318333;
deltaTArrayMod[28] = -0.227861; deltaR[28] = -0.0389804; deltaPhi[ 28] = -0.0012114; deltaZ[28] = -0.0674168;
deltaTArrayMod[29] = -0.081439; deltaR[29] = -0.0449712; deltaPhi[ 29] = -0.00826954; deltaZ[29] = 0.00548985;
deltaTArrayMod[30] = 0.17098; deltaR[30] = -0.0938958; deltaPhi[ 30] = -0.00969672; deltaZ[30] = 0.0597671;
deltaTArrayMod[31] = 0.0258336; deltaR[31] = -0.052614; deltaPhi[ 31] = -0.000359792; deltaZ[31] = 0.044548;
deltaTArrayMod[32] = -0.425254; deltaR[32] = -0.0372025; deltaPhi[ 32] = -0.00536103; deltaZ[32] = -0.0978299;
deltaTArrayMod[33] = -0.440596; deltaR[33] = -0.0534945; deltaPhi[ 33] = -0.0144455; deltaZ[33] = -0.136331;
deltaTArrayMod[34] = -0.256048; deltaR[34] = -0.0884831; deltaPhi[ 34] = -0.00580407; deltaZ[34] = -0.0676089;
deltaTArrayMod[35] = -0.601107; deltaR[35] = 0.0224642; deltaPhi[ 35] = -0.00283517; deltaZ[35] = -0.115955;
deltaTArrayMod[36] = -0.472965; deltaR[36] = 0.0023781; deltaPhi[ 36] = -0.00319073; deltaZ[36] = -0.049661;
deltaTArrayMod[37] = -0.498374; deltaR[37] = -0.0241109; deltaPhi[ 37] = -0.0168551; deltaZ[37] = -0.102832;
deltaTArrayMod[38] = -0.466373; deltaR[38] = -0.0623884; deltaPhi[ 38] = 0.00665517; deltaZ[38] = -0.131102;
deltaTArrayMod[39] = -0.382128; deltaR[39] = -0.040161; deltaPhi[ 39] = 0.00508923; deltaZ[39] = -0.100047;
  Double_t antTimeDiff[40]={-0.0386453,-0.0153132,0.00122104,0.0159814,0.0270482,0.0376653,-0.0187465,-0.0277069,-0.0335177,-0.0244345,0.00564512,0.0127849,0.029559,0.0182265,0.00704821,-0.0128154,-0.0333643,-0.0270623,-0.0302241,-0.0445596,-0.0140304,0.00545488,0.0109931,0.0268804,0.0221953,0.0270194,0.0356925,0.0344474,0.00566617,-0.00202704,-0.0114466,-0.00563483,-0.00687291,0.00588648,-0.00329303,-0.00602704,-0.00988036,0.00524623,-0.00493698,-0.0121224};

 

  ofstream SimonCalib("simonsPositionAndTimingOffsets.dat");
  SimonCalib << "#Ant\tdt\t\tdr\t\tdphi\tdz\n";
  for(int ant=0;ant<40;ant++) {
    SimonCalib << ant << "\t" << (deltaTArrayMod[ant]+antTimeDiff[ant]) << "\t"
	       << deltaR[ant] << "\t" << deltaPhi[ant] << "\t"
	       << deltaZ[ant] << "\n";
  }
  SimonCalib.close();
  
}
