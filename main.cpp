/* 
 * File:   main.cpp
 * Author: honza
 *
 * Created on 7. říjen 2012, 15:41
 */

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

using namespace std;

#define INITTAG 0
#define WORKTAG 1
#define DIETAG 2
#define BUF_MIN_LENGHT 20

void master() {
    int numberOfProcessors;
    MPI_Comm_size(MPI_COMM_WORLD, &numberOfProcessors);
    cout << "Processors:" << numberOfProcessors << endl;
    TaskParser* parser = new TaskParser(true);
    TaskCrate* taskCrate = parser->getTaskFromFile("testTasks/test44.txt");
    queue<SmartStruct*> smartQueue = WorkerParallel::prepareWorkForProcessors(taskCrate, numberOfProcessors);

    int initSize = taskCrate->getSize();
    int lenght = BUF_MIN_LENGHT + initSize;

    for (int i = 1; i < numberOfProcessors; i++) {
        MPI_Send(&initSize, 1, MPI_INT, i, INITTAG, MPI_COMM_WORLD);

        SmartStruct* task = smartQueue.front();
        smartQueue.pop();

        cout << i << ":startNode";
        task->getNode()->printNode();

        int size = task->getSize();
        bool* array = task->getArray();
        Node* node = task->getNode();
        int maxHamWeight = node->getMaximalHammWeight();
        int level = node->getLevel();
        bool isLeaf = node->isIsLeaf();
        bool value = node->getValue();

        char buffer[lenght];
        int position = 0;

        MPI_Pack(array, size, MPI_C_BOOL, buffer, lenght, &position, MPI_COMM_WORLD);
        MPI_Pack(&maxHamWeight, 1, MPI_INT, buffer, lenght, &position, MPI_COMM_WORLD);
        MPI_Pack(&level, 1, MPI_INT, buffer, lenght, &position, MPI_COMM_WORLD);
        MPI_Pack(&isLeaf, 1, MPI_C_BOOL, buffer, lenght, &position, MPI_COMM_WORLD);
        MPI_Pack(&value, 1, MPI_C_BOOL, buffer, lenght, &position, MPI_COMM_WORLD);
        MPI_Send(buffer, position, MPI_PACKED, i, WORKTAG, MPI_COMM_WORLD);

        delete task;
    }
    delete parser;

    SmartStruct* task = smartQueue.front();
    smartQueue.pop();

    task->getNode()->printNode();

    WorkerMaster* master = new WorkerMaster(taskCrate, task->getArray(), task->getNode(), true);
    master->initParalel(numberOfProcessors, 0, BUF_MIN_LENGHT + initSize);
    master->workparalel();
    delete master;
}

void slave() {
    int my_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    int numberOfProcessors;
    MPI_Comm_size(MPI_COMM_WORLD, &numberOfProcessors);

    int initSize;
    MPI_Status status;
    MPI_Recv(&initSize, 1, MPI_INT, 0, INITTAG, MPI_COMM_WORLD, &status);

    int lenght = BUF_MIN_LENGHT + initSize;

    char buffer[lenght];
    int position = 0;
    int maxHamWeight, level;
    bool isLeaf, value;
    bool* array = (bool*) malloc(initSize);

    for (int i = 0; i < initSize; i++) {
        array[i] = false;
    }

    MPI_Recv(buffer, lenght, MPI_PACKED, 0, WORKTAG, MPI_COMM_WORLD, &status);
    MPI_Unpack(buffer, lenght, &position, array, initSize, MPI_C_BOOL, MPI_COMM_WORLD);
    MPI_Unpack(buffer, lenght, &position, &maxHamWeight, 1, MPI_INT, MPI_COMM_WORLD);
    MPI_Unpack(buffer, lenght, &position, &level, 1, MPI_INT, MPI_COMM_WORLD);
    MPI_Unpack(buffer, lenght, &position, &isLeaf, 1, MPI_C_BOOL, MPI_COMM_WORLD);
    MPI_Unpack(buffer, lenght, &position, &value, 1, MPI_C_BOOL, MPI_COMM_WORLD);

    Node* node;
    if (!isLeaf) {
        node = new ParentNode(maxHamWeight, level, value);
    } else {
        node = new LeafNode(maxHamWeight, level, value);
    }

    TaskParser* parser = new TaskParser(false);
    TaskCrate* taskCrate = parser->getTaskFromFile("testTasks/test44.txt");
    WorkerSlave* worker = new WorkerSlave(taskCrate, array, node, false);
    worker->initParalel(numberOfProcessors, my_rank, BUF_MIN_LENGHT + initSize);
    worker->workparalel();
    delete worker;
    delete parser;
}

/*
 * 
 */
int main(int argc, char** argv) {
    int my_rank;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    if (my_rank == 0) {        
        master();
        
    } else {
        slave();
    }
    // cout<<": "<<my_rank<<" is calling MPI_finalize:"<<endl;
    MPI_Finalize();
}