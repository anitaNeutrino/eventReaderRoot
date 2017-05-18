/////////////////////   By Oindree ////////////////////////
///////////////////     ANITA-4 WAIS pulsing happened Dec 8 - 11, 2016   ////////////////////////
///////////////////////// according to Slack records which are shown to me in my timezone ///////////////////////
////////////////////////// to run do root -l findWaisRuns.C"()" /////////////////

#include "AnitaDataset.h"

using namespace std; 

void findWaisRuns();

void findWaisRuns()

{
    int start = AnitaDataset::getRunAtTime(1481086800);
    int end = AnitaDataset::getRunAtTime(1481518800);
    
    cout << "Following two run numbers should be conservative lower and upper limit on when WAIS pulsing was done for ANITA-4" << endl;
    cout << "Human time (GMT): Wed, 07 Dec 2016 05:00:00 GMT -- run number is " << start << endl;
    cout << "Human time (GMT): Mon, 12 Dec 2016 05:00:00 GMT -- run number is " << end << endl;
    
}


