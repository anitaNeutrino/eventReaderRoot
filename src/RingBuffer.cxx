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
  maxSize = bufferSize;
  sum = 0;
}


void RingBuffer::insert(Double_t value){

  if(elements.size() >= this->maxSize){
    this->sum -= elements.front();
    elements.pop();
  }
  
  elements.push(value);
  this->sum += value;
}


Double_t RingBuffer::getSum(){
  return this->sum;
}

Double_t RingBuffer::getMean(){
  // std::cout << elements.size() << "\t" << this->sum << std::endl;
  Double_t mean = 0;
  if(elements.size() >= 1){
    mean = this->sum/elements.size();
  }
  return mean;
}
