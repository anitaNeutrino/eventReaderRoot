#include "CalibratedHk.h" 
#include "CalibratedSSHk.h" 

void dumpHKToAscii(int run, const char * out_dir = ".", bool dump_mag = true, bool dump_accel = true, bool dump_ss = true, bool dump_gps = true) 
{


  TString path; 
  path.Form("%s/run%d/hkFile%d.root",getenv("ANITA_ROOT_DATA"), run, run); 

  TFile hkFile(path.Data()); 

  TTree * hkTree = (TTree*) hkFile.Get("hkTree"); 


  path.Form("%s/run%d/sshkFile%d.root",getenv("ANITA_ROOT_DATA"), run, run); 
  TFile SShkFile(path.Data()); 
  TTree * SShkTree = (TTree*) SShkFile.Get("hkTree"); 


  CalibratedHk * hk = new CalibratedHk; 
  CalibratedSSHk * SShk = new CalibratedSSHk; 

  hkTree->SetBranchAddress("hk",&hk); 
  SShkTree->SetBranchAddress("hk",&SShk); 

  path.Form("%s/run%d/gpsFile%d.root",getenv("ANITA_ROOT_DATA"), run, run); 
  TFile gpsFile(path.Data()); 

  TTree * adu5PatTree = (TTree*) gpsFile.Get("adu5PatTree"); 

  Adu5Pat * patA = new Adu5Pat; adu5PatTree->SetBranchAddress("pat",&patA); 

  TTree * adu5SatTree = (TTree*) gpsFile.Get("adu5SatTree"); 
  Adu5Sat * satA = new Adu5Sat;  adu5SatTree->SetBranchAddress("sat",&satA); 
  TTree * adu5bPatTree = (TTree*) gpsFile.Get("adu5bPatTree"); 
  Adu5Pat * patB = new Adu5Pat; adu5bPatTree->SetBranchAddress("pat",&patB); 
  TTree * adu5bSatTree = (TTree*) gpsFile.Get("adu5bSatTree"); 
  Adu5Sat * satB = new Adu5Sat; adu5bSatTree->SetBranchAddress("sat",&satB); 



  if (dump_mag) 
  {
    path.Form("%s/mag%d.dat",out_dir, run); 
    FILE * f = fopen(path.Data(),"w"); 

    fprintf(f,"Time\tMag-X\tMag-Y\tMag-Z\n");

    for (int i = 0; i < hkTree->GetEntries(); i++) 
    {
      hkTree->GetEntry(i); 
      fprintf(f,"%f\t%f\t%f\t%f\n", hk->payloadTime + 1e-6 * hk->payloadTimeUs, hk->magX, hk->magY, hk->magZ); 

    }
    fclose(f); 
    path = TString("gzip -f ") + path; 
    system(path.Data()); 
  }

  if (dump_accel) 
  {
    path.Form("%s/acc%d.dat",out_dir, run); 
    FILE * f = fopen(path.Data(),"w"); 

    fprintf(f,"Time\tAcc1-X\tAcc1-Y\tAcc1-Z\tAcc1-T\tAcc2-X\tAcc2-Y\tAcc2-Z\tAcc2-T\n");

    for (int i = 0; i < hkTree->GetEntries(); i++) 
    {
      hkTree->GetEntry(i); 
      fprintf(f,"%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\n", hk->payloadTime + 1e-6 * hk->payloadTimeUs, 
          hk->getAccelerometer(0,0), 
          hk->getAccelerometer(0,1), 
          hk->getAccelerometer(0,2), 
          hk->getAccelerometer(0,3), 
          hk->getAccelerometer(1,0), 
          hk->getAccelerometer(1,1), 
          hk->getAccelerometer(1,2), 
          hk->getAccelerometer(1,3)); 
    }

    fclose(f); 
    path = TString("gzip -f ") + path; 
    system(path.Data()); 
    
  }


  if (dump_ss)
  {

    path.Form("%s/ssodd%d.dat",out_dir, run); 
    FILE * f = fopen(path.Data(),"w"); 
    fprintf(f,"Time\t"
              "SS1-x1\tSS1-x2\tSS1-y1\tSS1-y2\tSS1-T\t" 
              "SS3-x1\tSS3-x2\tSS3-y1\tSS3-y2\tSS3-T\t" 
              "SS5-x1\tSS5-x2\tSS5-y1\tSS5-y2\tSS5-T\t" 
              "SS7-x1\tSS7-x2\tSS7-y1\tSS7-y2\tSS7-T\n" ); 

    for (int i = 0; i < hkTree->GetEntries(); i++) 
    {
          hkTree->GetEntry(i);  
          fprintf(f,"%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\n", hk->payloadTime + 1e-6 * hk->payloadTimeUs, 
          hk->getRawSunsensor(0,0), hk->getRawSunsensor(0,1), hk->getRawSunsensor(0,2), hk->getRawSunsensor(0,3), hk->getRawSunsensor(0,4), 
          hk->getRawSunsensor(1,0), hk->getRawSunsensor(1,1), hk->getRawSunsensor(1,2), hk->getRawSunsensor(1,3), hk->getRawSunsensor(1,4), 
          hk->getRawSunsensor(2,0), hk->getRawSunsensor(2,1), hk->getRawSunsensor(2,2), hk->getRawSunsensor(2,3), hk->getRawSunsensor(2,4), 
          hk->getRawSunsensor(3,0), hk->getRawSunsensor(3,1), hk->getRawSunsensor(3,2), hk->getRawSunsensor(3,3), hk->getRawSunsensor(3,4)); 
    }
    fclose(f); 
    path = TString("gzip -f ") + path; 
    system(path.Data()); 

    path.Form("%s/sseven%d.dat",out_dir, run); 
    f = fopen(path.Data(),"w"); 
    fprintf(f,"Time\t"
              "SS2-x1\tSS2-x2\tSS2-y1\tSS2-y2\tSS2-T\t" 
              "SS4-x1\tSS4-x2\tSS4-y1\tSS4-y2\tSS4-T\t" 
              "SS6-x1\tSS6-x2\tSS6-y1\tSS6-y2\tSS6-T\t" 
              "SS8-x1\tSS8-x2\tSS8-y1\tSS8-y2\tSS8-T\n" ); 

    for (int i = 0; i < SShkTree->GetEntries(); i++) 
    {
          SShkTree->GetEntry(i);  
          fprintf(f,"%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\n", hk->payloadTime + 1e-6 * hk->payloadTimeUs, 
          SShk->getRawSunsensor(0,0), SShk->getRawSunsensor(0,1), SShk->getRawSunsensor(0,2), SShk->getRawSunsensor(0,3), SShk->getRawSunsensor(0,4), 
          SShk->getRawSunsensor(1,0), SShk->getRawSunsensor(1,1), SShk->getRawSunsensor(1,2), SShk->getRawSunsensor(1,3), SShk->getRawSunsensor(1,4), 
          SShk->getRawSunsensor(2,0), SShk->getRawSunsensor(2,1), SShk->getRawSunsensor(2,2), SShk->getRawSunsensor(2,3), SShk->getRawSunsensor(2,4), 
          SShk->getRawSunsensor(3,0), SShk->getRawSunsensor(3,1), SShk->getRawSunsensor(3,2), SShk->getRawSunsensor(3,3), SShk->getRawSunsensor(3,4)); 
    }
    fclose(f); 
    path = TString("gzip -f ") + path; 
    system(path.Data()); 

  }

  if (dump_gps) 
  {
    //dump ADU5APat
    path.Form("%s/adu5Apat%d.dat",out_dir, run); 
    FILE * f = fopen(path.Data(),"w"); 

    fprintf(f,"payloadTime\t" 
              "timeOfDay\t" 
              "latitude\t"
              "longitude\t"
              "altitude\t"
              "heading\t"
              "pitch\t"
              "roll\t"
              "mrms\t"
              "brms\t"
              "attFlag\n"); 


    for (int i = 0; i < adu5PatTree->GetEntries(); i++) 
    {
      adu5PatTree->GetEntry(i); 
      fprintf(f,"%f\t%d\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%d\n",
          patA->payloadTime + patA->payloadTimeUs*1e-6, patA->timeOfDay, patA->latitude, patA->longitude,
          patA->altitude,patA->heading,patA->pitch,patA->roll,patA->mrms,patA->brms,patA->attFlag); 
    }

    fclose(f); 
    path = TString("gzip -f ") + path; 
    system(path.Data()); 


    //dump ADU5BPat
    path.Form("%s/adu5Bpat%d.dat",out_dir, run); 
    f = fopen(path.Data(),"w"); 

    fprintf(f,"payloadTime\t" 
              "timeOfDay\t" 
              "latitude\t"
              "longitude\t"
              "altitude\t"
              "heading\t"
              "pitch\t"
              "roll\t"
              "mrms\t"
              "brms\t"
              "attFlag\n"); 


    for (int i = 0; i < adu5bPatTree->GetEntries(); i++) 
    {
      adu5bPatTree->GetEntry(i); 
      fprintf(f,"%f\t%d\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%d\n",
          patB->payloadTime + patB->payloadTimeUs*1e-6, patB->timeOfDay, patB->latitude, patB->longitude,
          patB->altitude,patB->heading,patB->pitch,patB->roll,patB->mrms,patB->brms,patB->attFlag); 
    }

    fclose(f); 
    path = TString("gzip -f ") + path; 
    system(path.Data()); 


    //dump ADU5ASat
    path.Form("%s/adu5Asat%d.dat",out_dir, run); 
    f = fopen(path.Data(),"w"); 
    fprintf(f,"payloadTime"); 

    for (int i = 0; i < 4; i++) {  fprintf(f,"\tnumSats%d",i); } 

    const char * fields[] = {"prn","elevation","snr","flag","azimuth",0}; 

    for (const char ** field = &fields[0]; *field; field++)
    {
      for (int i = 0; i < 4; i++)
      {
        for (int j = 0; j < 12; j++) 
        {
          fprintf(f,"\t%s%d_%d",*field,i,j); 
        }
      }
    }
    fprintf(f,"\n"); 

    for (int e = 0; e < adu5SatTree->GetEntries(); e++)
    {
      adu5SatTree->GetEntry(e); 
      fprintf(f,"%d\t",satA->payloadTime); 

      for (int i = 0; i < 4; i++) fprintf(f,"\t%d",satA->numSats[i]); 
      for (int i = 0; i < 4; i++) { for (int j = 0; j < 12; j++) { fprintf(f,"\t%d", satA->prn[i][j]);}}
      for (int i = 0; i < 4; i++) { for (int j = 0; j < 12; j++) { fprintf(f,"\t%d", satA->elevation[i][j]);}}
      for (int i = 0; i < 4; i++) { for (int j = 0; j < 12; j++) { fprintf(f,"\t%d", satA->snr[i][j]);}}
      for (int i = 0; i < 4; i++) { for (int j = 0; j < 12; j++) { fprintf(f,"\t%d", satA->flag[i][j]);}}
      for (int i = 0; i < 4; i++) { for (int j = 0; j < 12; j++) { fprintf(f,"\t%d", satA->azimuth[i][j]);}}
      fprintf(f,"\n"); 
    }
    fclose(f); 
    path = TString("gzip -f ") + path; 
    system(path.Data()); 
              

    //dump ADU5BSat
    path.Form("%s/adu5Bsat%d.dat",out_dir, run); 
    f = fopen(path.Data(),"w"); 
    fprintf(f,"payloadTime"); 

    for (int i = 0; i < 4; i++) {  fprintf(f,"\tnumSats%d",i); } 

    for (const char ** field = &fields[0]; *field; field++)
    {
      for (int i = 0; i < 4; i++)
      {
        for (int j = 0; j < 12; j++) 
        {
          fprintf(f,"\t%s%d_%d",*field,i,j); 
        }
      }
    }
    fprintf(f,"\n"); 

    for (int e = 0; e < adu5bSatTree->GetEntries(); e++)
    {
      adu5bSatTree->GetEntry(e); 
      fprintf(f,"%d\t",satB->payloadTime); 

      for (int i = 0; i < 4; i++) fprintf(f,"\t%d",satB->numSats[i]); 
      for (int i = 0; i < 4; i++) { for (int j = 0; j < 12; j++) { fprintf(f,"\t%d", satB->prn[i][j]);}}
      for (int i = 0; i < 4; i++) { for (int j = 0; j < 12; j++) { fprintf(f,"\t%d", satB->elevation[i][j]);}}
      for (int i = 0; i < 4; i++) { for (int j = 0; j < 12; j++) { fprintf(f,"\t%d", satB->snr[i][j]);}}
      for (int i = 0; i < 4; i++) { for (int j = 0; j < 12; j++) { fprintf(f,"\t%d", satB->flag[i][j]);}}
      for (int i = 0; i < 4; i++) { for (int j = 0; j < 12; j++) { fprintf(f,"\t%d", satB->azimuth[i][j]);}}
      fprintf(f,"\n"); 
    }
    fclose(f); 
    path = TString("gzip -f ") + path; 
    system(path.Data()); 
              

  }

}
