
void makeSimonPositionFile() {

  double deltaRUpper=-0.042685;
  double deltaRLower=0.00653;
  double deltaRNadir=0.1927;
  
  double deltaTArrayMod[40]={0};
  double deltaR[40]={0};
  double deltaZ[40]={0};
  double deltaPhi[40]={0};
  
deltaTArrayMod[0] = 0.0966272; deltaR[0] = -0.057775; deltaPhi[0] = -0.00219158; deltaZ[0] = 0.000263412;
deltaTArrayMod[1] = 0.0876092; deltaR[1] = -0.0545754; deltaPhi[1] = -0.0011866; deltaZ[1] = -0.00790786;
deltaTArrayMod[2] = -0.0500581; deltaR[2] = -0.0472341; deltaPhi[2] = -0.0133014; deltaZ[2] = -0.0326872;
deltaTArrayMod[3] = -0.116461; deltaR[3] = 0.0166277; deltaPhi[3] = -0.00412326; deltaZ[3] = 0.0163752;
deltaTArrayMod[4] = -0.200318; deltaR[4] = 0.00349228; deltaPhi[4] = -0.0129494; deltaZ[4] = -0.0160669;
deltaTArrayMod[5] = -0.154772; deltaR[5] = 0.0117742; deltaPhi[5] = 0.0102184; deltaZ[5] = -0.00294579;
deltaTArrayMod[6] = -0.128339; deltaR[6] = 0.00601329; deltaPhi[6] = -0.00475167; deltaZ[6] = 0.0123858;
deltaTArrayMod[7] = 0.0391449; deltaR[7] = -0.0296225; deltaPhi[7] = -0.000472942; deltaZ[7] = 0.0357695;
deltaTArrayMod[8] = 0.131671; deltaR[8] = -0.0148234; deltaPhi[8] = -0.0213506; deltaZ[8] = -0.000821416;
deltaTArrayMod[9] = -0.0116696; deltaR[9] = -0.0491182; deltaPhi[9] = -0.0013534; deltaZ[9] = -0.0500962;
deltaTArrayMod[10] = 0.186637; deltaR[10] = -0.0451328; deltaPhi[10] = 0.00187113; deltaZ[10] = -0.0313106;
deltaTArrayMod[11] = -0.18339; deltaR[11] = -0.00212003; deltaPhi[11] = 0.00157555; deltaZ[11] = -0.037919;
deltaTArrayMod[12] = 0.0594779; deltaR[12] = 0.00839198; deltaPhi[12] = -0.00947194; deltaZ[12] = -0.0254805;
deltaTArrayMod[13] = -0.295476; deltaR[13] = 0.0242404; deltaPhi[13] = -0.00412887; deltaZ[13] = -0.0375236;
deltaTArrayMod[14] = -0.0147839; deltaR[14] = -0.000364146; deltaPhi[14] = -0.00786151; deltaZ[14] = -0.0163511;
deltaTArrayMod[15] = -0.17456; deltaR[15] = -0.0090811; deltaPhi[15] = 0.00373828; deltaZ[15] = -0.0300118;
deltaTArrayMod[16] = -4.33681e-19; deltaR[16] = -0.0371989; deltaPhi[16] = -0.00606947; deltaZ[16] = 0.0261158;
deltaTArrayMod[17] = 0.0360281; deltaR[17] = -0.062061; deltaPhi[17] = 0.00181476; deltaZ[17] = -0.00874937;
deltaTArrayMod[18] = 0.0522174; deltaR[18] = -0.0797212; deltaPhi[18] = -0.00488843; deltaZ[18] = 0.000748227;
deltaTArrayMod[19] = 0.126481; deltaR[19] = -0.06581; deltaPhi[19] = -0.00782968; deltaZ[19] = 0.0287152;
deltaTArrayMod[20] = 0.0753953; deltaR[20] = -0.110449; deltaPhi[20] = -0.00374284; deltaZ[20] = -0.00172909;
deltaTArrayMod[21] = -0.0795108; deltaR[21] = -0.0534425; deltaPhi[21] = -0.0102019; deltaZ[21] = -0.0192991;
deltaTArrayMod[22] = -0.047079; deltaR[22] = -0.0512932; deltaPhi[22] = -0.00481952; deltaZ[22] = 0.0308291;
deltaTArrayMod[23] = 0.00705287; deltaR[23] = -0.0373865; deltaPhi[23] = -0.00184126; deltaZ[23] = 0.0491608;
deltaTArrayMod[24] = 0.0317818; deltaR[24] = -0.0466269; deltaPhi[24] = -0.00335195; deltaZ[24] = 0.0417712;
deltaTArrayMod[25] = -0.0253495; deltaR[25] = -0.0239962; deltaPhi[25] = -0.0016429; deltaZ[25] = 0.0284582;
deltaTArrayMod[26] = -0.0794882; deltaR[26] = -0.0339915; deltaPhi[26] = -0.0116964; deltaZ[26] = 0.00286242;
deltaTArrayMod[27] = -0.071585; deltaR[27] = 0.000317079; deltaPhi[27] = -0.00583844; deltaZ[27] = 0.0165072;
deltaTArrayMod[28] = -0.142463; deltaR[28] = -0.041775; deltaPhi[28] = -0.00384949; deltaZ[28] = -0.0143281;
deltaTArrayMod[29] = -0.19288; deltaR[29] = -0.0131172; deltaPhi[29] = -0.00376459; deltaZ[29] = -0.00181494;
deltaTArrayMod[30] = -0.112084; deltaR[30] = -0.0301828; deltaPhi[30] = -0.00486714; deltaZ[30] = -0.00387768;
deltaTArrayMod[31] = -0.0804114; deltaR[31] = -0.0275454; deltaPhi[31] = 0.000204602; deltaZ[31] = 0.017993;
deltaTArrayMod[32] = -0.448416; deltaR[32] = -0.0391346; deltaPhi[32] = -0.00552888; deltaZ[32] = -0.111188;
deltaTArrayMod[33] = -0.432299; deltaR[33] = -0.0662314; deltaPhi[33] = -0.00876767; deltaZ[33] = -0.154263;
deltaTArrayMod[34] = -0.386577; deltaR[34] = -0.0550183; deltaPhi[34] = -0.00663737; deltaZ[34] = -0.102624;
deltaTArrayMod[35] = -0.506231; deltaR[35] = -0.00388577; deltaPhi[35] = -0.00532373; deltaZ[35] = -0.103655;
deltaTArrayMod[36] = -0.456243; deltaR[36] = 0.00195356; deltaPhi[36] = -0.000592967; deltaZ[36] = -0.0436351;
deltaTArrayMod[37] = -0.421369; deltaR[37] = -0.027404; deltaPhi[37] = -0.017051; deltaZ[37] = -0.0695648;
deltaTArrayMod[38] = -0.478056; deltaR[38] = -0.0409523; deltaPhi[38] = 4.7049e-05; deltaZ[38] = -0.104336;
deltaTArrayMod[39] = -0.413654; deltaR[39] = -0.0363768; deltaPhi[39] = 0.00564773; deltaZ[39] = -0.109414;
Double_t antTimeDiff[40]={-0.0323396,-0.0217482,0.0134463,0.0139473,0.0171591,0.0243049,-0.0188381,-0.0231837,-0.0436763,-0.0294017,-0.00186964,0.0110517,0.0193566,0.000908365,-0.0194097,-0.0217073,-0.0278912,-0.0212246,-0.013629,-0.0348219,-0.0119918,0.0170441,0.0122237,0.0212869,0.0152573,0.0155666,0.0246164,0.0153134,-0.00561224,-0.00348799,0.000683768,-0.00333314,-0.00247814,0.0191795,0.00541418,-0.00541361,-0.0167872,-0.00506498,-0.0233656,-0.0114842};

 

  ofstream SimonCalib("simonsPositionAndTimingOffsets.dat");
  SimonCalib << "#Ant\tdt\t\tdr\t\tdphi\tdz\n";
  for(int ant=0;ant<40;ant++) {
    SimonCalib << ant << "\t" << (deltaTArrayMod[ant]+antTimeDiff[ant]) << "\t"
	       << deltaR[ant] << "\t" << deltaPhi[ant] << "\t"
	       << deltaZ[ant] << "\n";
  }
  SimonCalib.close();
  
}
