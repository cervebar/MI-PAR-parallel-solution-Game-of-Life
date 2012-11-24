/* 
 * File:   WorkerParallel.h
 * Author: Babu
 *
 * Created on 17. listopad 2012, 19:16
 */

#ifndef WORKERPARALLEL_H
#define	WORKERPARALLEL_H

#include "Worker.h"
#include "mpi.h"
#include "SmartStruct.h"
#include <queue>

#define BLACK 0
#define WHITE 1
#define TOKEN_MODULO 1000000
#define MODULO 1000

#define MSG_WORK_REQUEST 1000
#define MSG_WORK_SENT    1001
#define MSG_WORK_NOWORK  1002
#define MSG_TOKEN        1003
#define MSG_FINISH       1004
#define TAG_SOLUTION 1005

#define MASTER 0

class WorkerParallel : public Worker {
public:  
     WorkerParallel(TaskCrate* task, bool* starArray, Node* startNode, bool verbose);
    virtual ~WorkerParallel();
    virtual void workparalel()=0;
    void initParalel(int processors,int myRank,int buffersize);
    void sendToken(int colorToSend);
    void sendRequestForWork(int destination);
    void sendFinish();
    void handleCommunication();
    void handleRequestForwork();
    void handleNoWork();
    void handleWork();
    
    virtual void handleToken()=0;
    virtual void handleFinish()=0;
    
    static queue<SmartStruct*> prepareWorkForProcessors(TaskCrate* taskCrate, int numberOfProcessors);
    
protected:
    bool running;
    int color;    
    long counter;
    int processors;
    int myRank;
    MPI_Status status;
    bool iswaitingForRequestWork;
    int procesorWorkCounter;
    int nextProcesor;
    int previousprocesor;
    int expectedWorkFrom;
    int workBufferLenght;
    
    unsigned long tokenCounter;
    
    void resetWorkCounter();
};

#endif	/* WORKERPARALLEL_H */

