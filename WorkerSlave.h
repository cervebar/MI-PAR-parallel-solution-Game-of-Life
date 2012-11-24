/* 
 * File:   WorkerSlave.h
 * Author: Babu
 *
 * Created on 17. listopad 2012, 19:35
 */

#ifndef WORKERSLAVE_H
#define	WORKERSLAVE_H

#include "WorkerParallel.h"


class WorkerSlave : public WorkerParallel {
public:
    WorkerSlave(TaskCrate* task, bool* starArray, Node* startNode, bool verbose);
    virtual ~WorkerSlave();
    virtual void workparalel();
   virtual void handleToken();
   virtual void handleFinish();
   
   void sendSolution();

    
private:

};

#endif	/* WORKERSLAVE_H */

