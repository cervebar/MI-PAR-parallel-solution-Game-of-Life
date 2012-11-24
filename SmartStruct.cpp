/* 
 * File:   SmartStruct.cpp
 * Author: honza
 * 
 * Created on 11. listopad 2012, 14:09
 */

#include <vector>

#include "SmartStruct.h"

SmartStruct::SmartStruct(int size, bool* array, Node* node) {
    this->size = size;
    this->array = array;
    this->node = node;
}

SmartStruct::~SmartStruct() {
//    delete array;
//    delete node;
}

SmartStruct* SmartStruct::getLEftStruct() {
    SmartStruct* newStruct;
    if(node==NULL){
      bool* newArray= BinaryVector::copyArray(array, size);
      ParentNode* newNode=new ParentNode(size,size,true);    
      newStruct=new SmartStruct(size,newArray,newNode);
    }else{
      bool* newArray= BinaryVector::copyArray(array, size);
      int pointer=size-node->getLevel();
      newArray[pointer]=node->getValue();
      Node* n=node->makeLeftTrueNode();  
      newStruct = new SmartStruct(size,newArray,n);
    }
    return newStruct;
}

SmartStruct* SmartStruct::getRightStruct() {
     SmartStruct* newStruct;
    if(node==NULL){
      bool* newArray= BinaryVector::copyArray(array, size);
      ParentNode* newNode=new ParentNode(size-1,size,false);    
      newStruct=new SmartStruct(size,newArray,newNode);
    }else{
      bool* newArray= BinaryVector::copyArray(array, size);
      int pointer=size-node->getLevel();
      newArray[pointer]=node->getValue();
      Node* n=node->makeRightFalseNode();  
      newStruct = new SmartStruct(size,newArray,n);
    }
    return newStruct;
}
