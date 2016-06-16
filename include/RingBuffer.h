/* -*- C++ -*-.***************************************************************************************************
 Author: Ben Strutt
 Email: b.strutt.12@ucl.ac.uk

 Description: 
             A simple ring buffer class.
*************************************************************************************************************** */

#ifndef RINGBUFFER_H
#define RINGBUFFER_H

#include <queue>
#include <iostream>

// ROOT data types
#include <Rtypes.h>

class RingBuffer{

public:
  RingBuffer(UInt_t numElementsInput);

  void insert(Double_t value); /* Put an element in */
  Double_t getSum(); /* Sum of ring buffer */
  Double_t getMean(); /* Mean of ring buffer */

private:
  Double_t sum;
  UInt_t maxSize;
  std::queue<double> elements;

};


#endif
