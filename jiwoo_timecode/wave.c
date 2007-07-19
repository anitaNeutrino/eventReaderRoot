int goodpointsArray[N_SURF][N_CHN];
float TimeArray[N_SURF][N_SCA] ; 
int WrappedHitbus[N_SURF][N_CHN]; 
int rawUnwrappedArray[N_SURF][N_CHN][N_SCA] ;
float unwrappedArray[N_SURF][N_CHN][N_SCA] ; 

Float_t tcal_tbin[N_SURF][N_CHIP][2][N_SCA];
Float_t tcal_epsilon[N_SURF][N_CHIP][2];
Float_t tcal_rco_delay_bin[N_SURF][N_CHIP][2];

int scaArray[N_SURF][N_CHN][N_SCA] ;
int chipnumber[N_SURF][N_CHN] ;
int rcobit[N_SURF][N_CHN][N_SCA] ;
int first_hitbus[N_SURF] ;
int last_hitbus[N_SURF] ;


void read_tcal() 
{
  
  FILE *fp = fopen("anita_surf_time_constant_epsilon.txt","r");
  Int_t ncols;
  Int_t nlines = 0;

  while (1) {
    int surf, chip, rco;
    float dt_global, tbin0, tbin_slop, epsilon;

    ncols = fscanf(fp,"%d %d %d %g",&surf, &chip, &rco, &epsilon);
    if (ncols < 0) break;        
    int tmp_rco=1-rco;
    tcal_epsilon[surf][chip][tmp_rco]=epsilon;
    nlines++;  
  }
  fclose(fp);    

  
  FILE *fp_diff = fopen("anita_surf_time_constant_differeniial.txt","r");

  nlines = 0;
  while (1) {
    int surf, chip, rco;
    float epsilon, tbin;
    ncols = fscanf(fp,"%d %d %d",&surf, &chip, &rco);
    if (ncols < 0) break;        
    for (int ibin=0 ; ibin<260; ibin++){      
      ncols = fscanf(fp,"%g",&tbin);
      tcal_tbin[surf][chip][rco][ibin]=tbin;
      if (ncols < 0) break;        
    }
    nlines++;  
  }
  fclose(fp_diff);    

  
  FILE *fp_rco_delay = fopen("anita_surf_time_constant_rco_delay.txt","r");

  nlines = 0;
  while (1) {
    int surf, chip, rco;
    int rco_delay_bin;
    ncols = fscanf(fp,"%d %d %d %d",&surf, &chip, &rco,&rco_delay_bin);

    tcal_rco_delay_bin[surf][chip][rco]=rco_delay_bin;

    if (ncols < 0) break;        

    nlines++;  
  }
  fclose(fp_rco_delay);    
}





void Fill_WF_Arrays(PedSubbedEventBody_t theBody, float temp)
{  
  Int_t word;
  Int_t chanIndex=0;
  Int_t labChip=0;    

  float temp_scale=29.938/(31.7225-0.054*temp);

  for (int surf=0; surf<N_SURF; surf++){
    for (int chan=0; chan<N_CHN; chan++){ 
      int goodPoints=0;

      chanIndex=getChanIndex(surf,chan);
      
      int firstHitbus=theBody.channel[chanIndex].header.firstHitbus;
      int lastHitbus=theBody.channel[chanIndex].header.lastHitbus;
      //      printf("firsthitbus=%d last_hitbus=%d\n",firstHitbus,lastHitbus);
      int wrappedHitbus=(theBody.channel[chanIndex].header.chipIdFlag&0x8)>>3;
      WrappedHitbus[surf][chan]=wrappedHitbus;
      labChip=theBody.channel[chanIndex].header.chipIdFlag&0x3;
      int rcoBit=((theBody.channel[chanIndex].header.chipIdFlag&0x4)>>2);   
      first_hitbus[surf]= firstHitbus;
      last_hitbus[surf]= lastHitbus;
      chipnumber[surf][chan]=labChip;

      for(int samp=0;samp<N_SCA;samp++) {
	word=theBody.channel[chanIndex].data[samp];
	rawArray[surf][chan][samp]=word;    
	upperFour[surf][chan][samp]=(word&0xf000)>>12;
	labArray[surf][chan][samp]=word;
      }

      if(!wrappedHitbus) {
	int numHitBus=1+lastHitbus-firstHitbus;
	goodPoints=N_SCA-numHitBus;
      }
      else {
	goodPoints=lastHitbus-(firstHitbus+1);
      }


      int firstSamp,lastSamp;
      if(!wrappedHitbus) {
	firstSamp=lastHitbus+1;
	//	lastSamp=(N_SCA-1)+lastHitbus;//Ryan's error?
	lastSamp=N_SCA+firstHitbus;//My fix
      }
      else {
	firstSamp=firstHitbus+1;
	lastSamp=lastHitbus;
      }

      int startRco=rcoBit;
      if(!wrappedHitbus) 
	startRco=1-startRco;

      //switch RCO info for RCO delay
      if(firstHitbus<=tcal_rco_delay_bin[surf][labChip][startRco] && !wrappedHitbus) startRco=1-startRco;

      int ibin=0;
      for(int samp=firstSamp;samp<lastSamp;samp++) {
	int index=samp;
	int irco=startRco;
	if (index>=N_SCA) { 
	  index-=(N_SCA);
	  irco=1-startRco;
	}

	if (index==0) { //temp. fix to skip sca=0 where unexpected voltage apears
	  goodPoints--;
	  continue;
	}
	
	rawUnwrappedArray[surf][chan][ibin]=rawArray[surf][chan][index];
	unwrappedArray[surf][chan][ibin]=labArray[surf][chan][index];
	scaArray[surf][chan][ibin]=index; 
	rcobit[surf][chan][ibin]=irco;

	if (chan==8){//timing calibraion
	  float dt_bin=tcal_tbin[surf][labChip][irco][index]*temp_scale;	  
	  int index_prev=index-1;
	  if (index_prev==-1) index_prev=259;
	  float dt_bin_prev=tcal_tbin[surf][labChip][irco][index_prev];

	  if (ibin==0) TimeArray[surf][ibin]=dt_bin;       
	  else TimeArray[surf][ibin]=TimeArray[surf][ibin-1]+dt_bin;	

	  if (index==1) {	  
	    float epsilon_eff=tcal_epsilon[surf][labChip][irco];
	    TimeArray[surf][ibin]=TimeArray[surf][ibin]-epsilon_eff;
	  }
	}
	ibin++;	
      }
      
      goodpointsArray[surf][chan]=goodPoints;

      
      //2nd correction for RCO phase info. delay, RCO is determined by measuring clock period. 
      //to day CPU time, this method is used only if around the boundary of tcal_rco_delay_bin.
      if (chan==8){
	if (firstHitbus>tcal_rco_delay_bin[surf][labChip][startRco] && 
	    firstHitbus<=tcal_rco_delay_bin[surf][labChip][startRco]+2 && !wrappedHitbus){
	
	float t_LE[3];
	float t_TE[3];
	int LE_count=0;
	int TE_count=0;
	int ibin=0;
	for (ibin=0;ibin<goodPoints-1;ibin++){
	  float mv1=unwrappedArray[surf][chan][ibin];
	  float mv2=unwrappedArray[surf][chan][ibin+1];
	  if (LE_count<3 && mv1<0 && mv2>=0){
	    float t1=TimeArray[surf][ibin];
	    float t2=TimeArray[surf][ibin+1];
	    t_LE[LE_count]=Get_Interpolation_X(t1, mv1, t2, mv2, 0);
	    LE_count++;
	  }	    
	  if (TE_count<3 && mv1>0 && mv2<=0){
	    float t1=TimeArray[surf][ibin];
	    float t2=TimeArray[surf][ibin+1];
	    t_TE[TE_count]=Get_Interpolation_X(t1, mv1, t2, mv2, 0);
	    TE_count++;
	  }	    
	}	  
	
	if (LE_count>2 && TE_count>2){	    

	  float clock_pulse_width_LE;
	  if (LE_count==2) clock_pulse_width_LE=t_LE[1]-t_LE[0];
	  if (LE_count==3) clock_pulse_width_LE=(t_LE[2]-t_LE[0])/2.;
	  float clock_pulse_width_TE;
	  if (TE_count==2) clock_pulse_width_TE=t_TE[1]-t_TE[0];
	  if (TE_count==3) clock_pulse_width_TE=(t_TE[2]-t_TE[0])/2.;
	  float clock_pulse_width=(clock_pulse_width_LE+clock_pulse_width_TE)/2.;

	  if (clock_pulse_width<29.75 || clock_pulse_width>30.2){
	    for (int ibin=0;ibin<goodPoints;ibin++){
		
		for (int chan=0; chan<N_CHN; chan++) rcobit[surf][chan][ibin]=1-rcobit[surf][chan][ibin];
		//may be one rcobit array per board might be enough, need to modify later./jwnam
		int irco=rcobit[surf][chan][ibin];
		int index=scaArray[surf][chan][ibin];
		float dt_bin=tcal_tbin[surf][labChip][irco][index]*temp_scale;	  
		if (ibin==0) TimeArray[surf][ibin]=dt_bin;       
		else TimeArray[surf][ibin]=TimeArray[surf][ibin-1]+dt_bin;	
		if (index==1) {	  
		  float epsilon_eff=tcal_epsilon[surf][labChip][irco];
		  TimeArray[surf][ibin]=TimeArray[surf][ibin]-epsilon_eff;
		}
		
	      }
	    }
	  }
	}
      }
    }        
  }
}

float Get_Interpolation_X(float x1, float y1, float x2, float y2, float y){
  float x=(x2-x1)/(y2-y1)*(y-y1)+x1;
  return x;
}



