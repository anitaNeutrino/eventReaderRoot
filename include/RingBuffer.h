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

  // I have no idea what the proper stl iterators look like on the inside,
  // so this is probably not how you're supposed to do this  

  class iterator{
  public:
    iterator(RingBuffer* rb=NULL, UInt_t vi=0) : ringBuffer(rb), vecIndex(vi){}
    virtual ~iterator(){}

    iterator& operator=(const iterator& rhs){
      this->ringBuffer = rhs.ringBuffer;
      this->vecIndex = rhs.vecIndex;
      return (*this);
    }

    iterator& operator++(){
      vecIndex = vecIndex < ringBuffer->size ? vecIndex + 1 : 0;
      return (*this);
    }

    iterator operator++(int){
      iterator tmp(*this);
      operator++();
      return tmp;
    }
    
    iterator& operator--(){
      vecIndex = vecIndex == 0 ? ringBuffer->size: vecIndex - 1;
      return (*this);
    }

    iterator operator--(int){
      iterator tmp(*this);
      operator--();
      return tmp;
    }

    // the underlying order probably isn't what people expect, so I'm gonna leave commented out for now.
    
    // iterator& operator+=(int i){
    //   vecIndex = (vecIndex + i) % ringBuffer->size;
    //   return (*this);
    // }

    // iterator& operator-=(int i){
    //   operator+=(i);
    //   vecIndex = vecIndex < 0 ? vecIndex + ringBuffer->size : vecIndex;
    //   return (*this);
    // }

    // iterator operator+(int i){
    //   iterator tmp(*this);
    //   tmp+=i;
    //   return tmp;
    // }

    // iterator operator-(int i){
    //   iterator tmp(*this);
    //   tmp-=i;
    //   return tmp;
    // }
    
    double& operator*(){
      return ringBuffer->elements[vecIndex];
    }
    
    double* operator->(){
      return &(operator*());
    }

    bool operator==(const iterator& rhs) const{
      return (vecIndex == rhs.vecIndex && ringBuffer == rhs.ringBuffer);
    }

    bool operator!=(const iterator& rhs) const{
      return !(operator==(rhs));
    }
    
  private:        
    RingBuffer* ringBuffer;
    mutable Int_t vecIndex;
  };

  friend class iterator;  
  

public:
  explicit RingBuffer(UInt_t numElementsInput);

  void insert(Double_t value); /* Put an element in */
  Double_t getSum(){return sum;}
  Double_t getMean(){return numRingElements > 0 ? sum/numRingElements : 0;}
  

  iterator begin(){ // should point to the oldest element    
    return iterator(this, oldestElement());
  }
  iterator end(){
    return iterator(this, nextElement);
  }
  
private:  

  Double_t sum;
  UInt_t size;
  UInt_t numRingElements; // ranges from 0 -> size as the buffer fills (remains at size once the buffer is full)
  UInt_t nextElement; // indexes where we will insert the next element, the 
  std::vector<double> elements; // this will be a vector of (size+1) so that .end() iterator can use normal iterator!=RingBuffer::end() syntax for loops.

  // helper function
  UInt_t oldestElement(){
    // if we haven't completely filled the buffer, then the oldest element is zero
    UInt_t oldestElement = 0; 
    if(numRingElements==size){ // if we have filled the buffer,
      // then the oldest is the one in front of the nextElement
      oldestElement = nextElement + 1;
      oldestElement = oldestElement >= size + 1? oldestElement - (size + 1) : oldestElement;
    }
    return oldestElement;
  }

};


#endif
