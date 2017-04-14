/* -*- C++ -*-.***************************************************************************************************
 Author: Ben Strutt
 Email: b.strutt.12@ucl.ac.uk

 Description: 
             A simple ring buffer class.
*************************************************************************************************************** */

#include "RingBuffer.h"

RingBuffer::RingBuffer(UInt_t bufferSize){
  
  if(bufferSize < 2){
    std::cerr << "Error in RingBuffer constructor! Constructor requires numElements >= 2" << std::endl;
    std::cerr << "Setting RingBuffer.numElements = 2" << std::endl;
    bufferSize = 2;
  }
  size = bufferSize;
  sum = 0;
  elements.resize(size+1, 0);
}


void RingBuffer::insert(Double_t value){

  // if the buffer is full we must remove an old element
  if(numRingElements==size){
    // then need to remove one
    int oe = oldestElement();
    sum -= elements[oe];
    elements[oe] = 0; // overwrite value with zero
  }

  // insert new element
  sum += value;  
  elements[nextElement] = value;

  // update count (used for getMean())
  numRingElements = numRingElements < size ? numRingElements + 1 : size;

  // update index pointing to next element
  // (runs through vector elements (with length size+1) in sequence)
  nextElement = nextElement < size ? nextElement + 1 : 0;			      

}
