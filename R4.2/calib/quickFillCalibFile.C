

void quickFillCalibFile() 
{
  ofstream Calib("justBinByBin.dat");
  Calib << "SURF\tChip\tRCO\tSample\tDeltaT\n";
  for(int surf=0;surf<10;surf++) {
    for(int chip=0;chip<4;chip++) {
      for(int rco=0;rco<2;rco++) {
	 for(int samp=0;samp<260;samp++) {
	    Calib << surf << "\t" << chip << "\t" << rco << "\t" << samp << "\t" << 1./2.6 << endl;
      }
    }
  }
  }
  Calib.close();
}
