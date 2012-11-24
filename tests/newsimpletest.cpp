/* 
 * File:   newsimpletest.cpp
 * Author: Babu
 *
 * Created on 19.11.2012, 21:35:48
 */

#include <stdlib.h>
#include <iostream>
#include "BinaryVector.h"
#include "TaskParser.h"
#include <math.h>
#include "ParentNode.h"
#include "Node.h"
#include "LeafNode.h"
#include "mpi.h"
#include "SmartStruct.h"
#include "WorkerMaster.h"
#include "WorkerSlave.h"
#include <queue>
/*
 * Simple C++ Test Suite
 */

void test1() {
    TaskParser* parser = new TaskParser(true);
    TaskCrate* taskCrate = parser->getTaskFromFile("testTasks/test33.txt");
    queue<SmartStruct*> smartQueue = WorkerParallel::prepareWorkForProcessors(taskCrate, 3);
    SmartStruct* task = smartQueue.front();
    smartQueue.pop();
    
    WorkerMaster* master=new WorkerMaster(taskCrate,task->getArray(),task->getNode(),true);
    master->initParalel(3,0,6);
    master->workCycle();
    
    delete parser;
    delete taskCrate;
    delete task;
    delete master;
}


int main(int argc, char** argv) {
    std::cout << "%SUITE_STARTING% newsimpletest" << std::endl;
    std::cout << "%SUITE_STARTED%" << std::endl;

    std::cout << "%TEST_STARTED% test1 (newsimpletest)" << std::endl;
    test1();
    std::cout << "%TEST_FINISHED% time=0 test1 (newsimpletest)" << std::endl;
    
    std::cout << "%SUITE_FINISHED% time=0" << std::endl;

    return (EXIT_SUCCESS);
}

