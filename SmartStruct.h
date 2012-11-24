/* 
 * File:   SmartStruct.h
 * Author: honza
 *
 * Created on 11. listopad 2012, 14:09
 */

#ifndef SMARTSTRUCT_H
#define	SMARTSTRUCT_H

#include "Node.h"
#include "ParentNode.h"

class SmartStruct {
public:
    SmartStruct(int size, bool* array, Node* node);
    virtual ~SmartStruct();

    SmartStruct* getLEftStruct();
    SmartStruct* getRightStruct();
    bool* getArray() const {
        return array;
    }

    Node* getNode() const {
        return node;
    }

    int getSize() const {
        return size;
    }


private:   
    
    int size;
    Node* node;
    bool* array;

};

#endif	/* SMARTSTRUCT_H */

