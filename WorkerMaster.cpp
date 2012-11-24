/* 
 * File:   Worker.cpp
 * Author: Babu a Bouchy
 * 
 * Created on 20. říjen 2012, 22:53
 */

#include "WorkerMaster.h"
#include "Worker.h"

WorkerMaster::WorkerMaster(TaskCrate* task, bool* startArray, Node* startNode, bool verbose) : WorkerParallel(task, startArray, startNode, verbose) {
    this->waitingForToken = false;
}

WorkerMaster::~WorkerMaster() {
}

void WorkerMaster::workparalel() {    
    double t1, t2;
    MPI_Barrier(MPI_COMM_WORLD); /* cekam na spusteni vsech procesu */
    t1 = MPI_Wtime(); /* pocatecni cas */

    while (running) {
        tokenCounter++;
        counter++;
        if (counter % MODULO == 0) {
            counter = 1;
            if (staciklist.empty()) {
                color = WHITE;
                sendRequestForWork(nextProcesor);
            } else {
                color = BLACK;
                // cout << myRank << ": ";
                work();
            }
            handleCommunication();
        }
        if (tokenCounter % TOKEN_MODULO == 0) {
            tokenCounter = 1;
            if (color == WHITE) {
                if (!waitingForToken) {
                    waitingForToken = true;
                    sendToken(color);
                }
            }
        }
    }
    //waiting for solutions
    getBestSolutionFromProcessors();
    
    result->printResult();
    delete result;
    
    smallMistakeResult->printResult();
    delete smallMistakeResult;
    
    MPI_Barrier(MPI_COMM_WORLD); /* cekam na dokonceni vypoctu */
    t2 = MPI_Wtime(); /* koncovy cas */
    printf ("Spotrebovany cas je %f.\n",t2-t1);
}

void WorkerMaster::handleToken() {
    // cout << myRank << " handleToken" << endl;
    int colorReceived;
    MPI_Recv(&colorReceived, 1, MPI_INT, previousprocesor, MSG_TOKEN, MPI_COMM_WORLD, &status);

    if (colorReceived == BLACK || color == BLACK) {
        //cout << "0: black - am I still waiting" << endl;
    } else {
        // cout << "0: white - am I finishing" << endl;
        running = false;
        sendFinish();
        return;
    }
    waitingForToken = false;
}

/*may not occur*/
void WorkerMaster::handleFinish() {
}

void WorkerMaster::getBestSolutionFromProcessors() {
    cout << myRank << " waiting for solution=========================================================" << endl;
    cout << "My solution is: " << result->IsExistResult() << ", distance:" << result->GetHammingDistance() << ", ham weight:" << result->GetHammingWeight() << endl;

    for (int i = 1; i < processors; i++) {
        char buffer[sizeOfHelpArray];
        int position = 0;
        bool array[sizeOfHelpArray];
        bool existSolution;

        MPI_Recv(buffer, workBufferLenght, MPI_PACKED, i, TAG_SOLUTION, MPI_COMM_WORLD, &status);
        MPI_Unpack(buffer, workBufferLenght, &position, &existSolution, 1, MPI_C_BOOL, MPI_COMM_WORLD);
        MPI_Unpack(buffer, workBufferLenght, &position, &array, sizeOfHelpArray, MPI_C_BOOL, MPI_COMM_WORLD);

        cout << "0: recieved from " << i << " solution: ";
        if (existSolution) {
            cout << "exist ";
            BinaryVector::printArray(array, sizeOfHelpArray);
        } else {
            cout << "doesn´t exist ";
        }
        cout << endl;

        if (existSolution) {
            solveBestSolution(array);
        }
    }
}

void WorkerMaster::solveBestSolution(bool* recievedArray) {
    BinaryVector* bv;
    bv = new BinaryVector(recievedArray, task->GetWidth(), task->GetHeight());
    int distance = BinaryVector::hammingActualArrayDistance(bv, task->getResultingConfigurationVector());
    int bvWeight = BinaryVector::hammingStartArrayWeight(bv);

    cout << "Solution: hamDistance:" << distance << ", weight:" << bvWeight << endl;

    if (distance > result->GetHammingDistance()) {
        bv->iterateForQTimes(task->GetIterations());
        bestHammWeight = bvWeight;
        result->setExistResult(true);
        result->setHammingDistance(distance);
        result->setHammingWeight(bvWeight);
        result->setResult(new BinaryVector(bv));
    }
    if (distance == result->GetHammingDistance()) {
        if (bvWeight > bestHammWeight) {
            bv->iterateForQTimes(task->GetIterations());
            bestHammWeight = bvWeight;
            result->setExistResult(true);
            result->setHammingDistance(distance);
            result->setHammingWeight(bvWeight);
            result->setResult(new BinaryVector(bv));
        }
    }
}