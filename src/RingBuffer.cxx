/* -*- C++ -*-.***************************************************************************************************
 Author: Ben Strutt
 Email: b.strutt.12@ucl.ac.uk

 Description: 
             A simple ring buffer class.
*************************************************************************************************************** */

#include "RingBuffer.h"

RingBuffer::RingBuffer(UInt_t bufferSize){
  
  if(bufferSize < default_size){
    std::cerr << "Error in RingBuffer constructor! Constructor requires numElements >= 2" << std::endl;
    std::cerr << "Setting RingBuffer.numElements = " << default_size << std::endl;
    bufferSize = default_size;
  }
  sum = 0;
  fSize = bufferSize;
  numRingElements = 0;
  nextElement = 0;
  elements.resize(fSize+1, 0);  
}


double RingBuffer::insert(Double_t value){

  double retVal = 0;
  // if the buffer is full we must remove an old element
  if(numRingElements==fSize){
    // then need to remove one
    int oe = oldestElement();
    retVal = elements[oe];
    sum -= retVal;
    elements[oe] = 0; // overwrite value with zero, not neccessary but will probably help with debugging
  }

  // insert new element
  sum += value;  
  elements[nextElement] = value;

  // update count (used for getMean())
  numRingElements = numRingElements < fSize ? numRingElements + 1 : fSize;

  // update index pointing to next element
  // (runs through vector elements (with length fSize+1) in sequence)
  nextElement = nextElement < fSize ? nextElement + 1 : 0;			      

  return retVal;
}
