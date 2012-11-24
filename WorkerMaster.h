/* 
 * File:   Worker.h
 * Author: Babu
 *
 * Created on 20. říjen 2012, 22:53
 */

#ifndef WORKERMASTER_H
#define	WORKERMASTER_H


#include "WorkerParallel.h"


class WorkerMaster:public WorkerParallel {
public:
    WorkerMaster(TaskCrate* task, bool* starArray, Node* startNode, bool verbose);
    virtual ~WorkerMaster();
     virtual void workparalel();
     virtual void handleToken();   
     virtual void handleFinish();
     
     void getBestSolutionFromProcessors();
     void solveBestSolution(bool* recievedArray);
private:     
    bool waitingForToken;
};

#endif	/* WORKERMASTER_H */

