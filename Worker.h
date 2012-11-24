/* 
 * File:   Worker.h
 * Author: Babu
 *
 * Created on 20. říjen 2012, 22:53
 */

#ifndef WORKER_H
#define	WORKER_H

#include "TaskCrate.h"
#include "Node.h"
#include "ResultCrate.h"
#include <list>

class Worker {
public:
    Worker(TaskCrate* task, bool* starArray, Node* startNode, bool verbose);
    virtual ~Worker();
    void work();

    ResultCrate* getResult() const {
        return result;
    }
    
    ResultCrate* getSmallMistakeResult() const {
        return smallMistakeResult;
    }
    
    void workCycle();

    void doSomething();

protected:
    TaskCrate* task;
    bool* helpArray;
    bool verbose;
    int sizeOfHelpArray;
    int treeLevel;
    list<Node*> staciklist;
    //explicit match
    int bestHammWeight;
    ResultCrate* result;
    //implicit match
    int bestHammWeightSmallMistake;
    ResultCrate* smallMistakeResult;
       

    void playGameOfLife();
    void saveResult(BinaryVector* result);   
};

#endif	/* WORKER_H */

