/* 
 * File:   WorkerSlave.cpp
 * Author: Babu
 * 
 * Created on 17. listopad 2012, 19:35
 */

#include "WorkerSlave.h"

WorkerSlave::WorkerSlave(TaskCrate* task, bool* startArray, Node* startNode, bool verbose) : WorkerParallel(task, startArray, startNode, verbose) {

}

WorkerSlave::~WorkerSlave() {
}

void WorkerSlave::workparalel() {
    double t1, t2;
    MPI_Barrier(MPI_COMM_WORLD); /* cekam na spusteni vsech procesu */
    t1 = MPI_Wtime(); /* pocatecni cas */
 
    while (running) {
        counter++;
        if (!staciklist.empty()) {
            color = BLACK;
            // cout << myRank << ": ";
            work();
        }
        if (counter % MODULO == 0) {
            counter = 1;
            if (staciklist.empty()) {
                color = WHITE;
                sendRequestForWork(nextProcesor);
            }
            handleCommunication();
        }
    }
    
    MPI_Barrier(MPI_COMM_WORLD); /* cekam na dokonceni vypoctu */
    t2 = MPI_Wtime(); /* koncovy cas */
}

void WorkerSlave::handleToken() {
    int colorReceived;
    MPI_Recv(&colorReceived, 1, MPI_INT, previousprocesor, MSG_TOKEN, MPI_COMM_WORLD, &status);
    int c;
    if (colorReceived == BLACK || color == BLACK) {
        c = BLACK;
    } else {
        c = WHITE;
    }
    sendToken(c);
}

void WorkerSlave::handleFinish() {
    cout << myRank << ": handle Finish from " << previousprocesor << endl;
    int cokoliv;
    running = false;
    MPI_Recv(&cokoliv, 1, MPI_INT, previousprocesor, MSG_FINISH, MPI_COMM_WORLD, &status);
    sendSolution();
    sendFinish();
}

void WorkerSlave::sendSolution() {
    cout << "rank:" << myRank << " send SOLUTION:";

    bool* array;

    char buffer[workBufferLenght];
    int position = 0;
    bool solutionExist = result->IsExistResult();

    if (solutionExist) {
        array = result->GetResultVector()->getStartConfiguration();
    } else {
        array = BinaryVector::getEmptyArray(sizeOfHelpArray);
    }

    BinaryVector::printArray(array, sizeOfHelpArray);
    cout << endl;

    MPI_Pack(&solutionExist, 1, MPI_C_BOOL, buffer, workBufferLenght, &position, MPI_COMM_WORLD);
    MPI_Pack(array, sizeOfHelpArray, MPI_C_BOOL, buffer, workBufferLenght, &position, MPI_COMM_WORLD);
    MPI_Send(buffer, position, MPI_PACKED, MASTER, TAG_SOLUTION, MPI_COMM_WORLD);

    delete array;
}
