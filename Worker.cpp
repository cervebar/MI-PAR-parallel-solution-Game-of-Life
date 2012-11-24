/* 
 * File:   Worker.cpp
 * Author: Babu a Bouchy
 * 
 * Created on 20. říjen 2012, 22:53
 */

#include "Worker.h"
#include "ParentNode.h"

Worker::Worker(TaskCrate* task, bool* startArray, Node* startNode, bool verbose) {
    this->task = task;
    this->helpArray = startArray;
    this->verbose = verbose;
    this->sizeOfHelpArray = task->getSize();
    this->treeLevel = task->getSize();
    this->bestHammWeight = -1;
    this->result=new ResultCrate(task);
    this->bestHammWeightSmallMistake=-1;
    this->smallMistakeResult=new ResultCrate(new TaskCrate(task));

    if (startNode == NULL) {
        Node * leftTrue = new ParentNode(sizeOfHelpArray, sizeOfHelpArray, true);
        Node * rightFalse = new ParentNode(sizeOfHelpArray - 1, sizeOfHelpArray, false);
        staciklist.push_front(rightFalse);
        staciklist.push_front(leftTrue);
    } else {
       staciklist.push_front(startNode);
    } 
}

Worker::~Worker() {
    delete helpArray;      
}

void Worker::workCycle(){
    while(!staciklist.empty()){
        work();
    }
    cout << "ending" << endl;
}

void Worker::work() {
        //1 take from the top of stack----------------------------------------------
        Node* node = staciklist.front();
        staciklist.pop_front();
        
//        BinaryVector::printArray(this->helpArray,this->sizeOfHelpArray);
//        cout << "working node:";
//        node->printNode();     
        
        //2 if the bestHamming weight is greater than can ever be in this branch
        //  we can throw this subtree away.-----------------------------------------
        if (node->getMaximalHammWeight() <= bestHammWeight) {
            delete node;
            return;
        }
        //3 set up the pointer / the max level versus node level--------------------
         int pointer = this->treeLevel - node->getLevel();
        //4 there where the pointer shows write the value

        this->helpArray[pointer] = node->getValue();
        //5 is the node LEAF?
        if (node->isIsLeaf()) {
            playGameOfLife();  
           // BinaryVector::printArray(this->helpArray,this->sizeOfHelpArray);        
        } else {
            Node * leftTrue = node->makeLeftTrueNode();
            Node * rightFalse = node->makeRightFalseNode();
            staciklist.push_front(leftTrue);
            staciklist.push_front(rightFalse);
        }
        delete node;    
}

void Worker::playGameOfLife() {     
    BinaryVector* bv;
    bv = new BinaryVector(helpArray, task->GetWidth(), task->GetHeight());
    bv->iterateForQTimes(task->GetIterations());
    int distance = BinaryVector::hammingActualArrayDistance(bv, task->getResultingConfigurationVector());

    //final configurations are the same
    if (BinaryVector::equals(bv, task->getResultingConfigurationVector())) {
//        cout<<"match";
//       // bv->printVector();
//        cout<<endl;

        int bvWeight = BinaryVector::hammingStartArrayWeight(bv);
        if (bvWeight > bestHammWeight) {
            bestHammWeight = bvWeight;
            result->setExistResult(true);
            result->setHammingDistance(distance);
            result->setHammingWeight(bvWeight);
            result->setResult(new BinaryVector(bv));
        }

    }//final configuration is not the same but the mistake is small
    else if (distance <= task->GetAMistake()) {
        int bvWeight = BinaryVector::hammingStartArrayWeight(bv);
        if (bvWeight > bestHammWeightSmallMistake) {
            bestHammWeightSmallMistake = bvWeight;
            smallMistakeResult->setExistResult(true);
            smallMistakeResult->setHammingDistance(distance);
            smallMistakeResult->setHammingWeight(bvWeight);
            smallMistakeResult->setResult(new BinaryVector(bv));
        }
    }//nothing changed    
    delete bv;
}

void Worker::doSomething() {

}



