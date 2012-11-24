/* 
 * File:   WorkerParallel.cpp
 * Author: Babu
 * 
 * Created on 17. listopad 2012, 19:16
 */

#include "WorkerParallel.h"
#include "ParentNode.h"
#include "LeafNode.h"

WorkerParallel::WorkerParallel(TaskCrate* task, bool* startArray, Node* startNode, bool verbose) : Worker(task, startArray, startNode, verbose) {

}

WorkerParallel::~WorkerParallel() {    
}

void WorkerParallel::initParalel(int processors, int myRank, int workBufferLenght) {
    //    this->color = WHITE;
    this->processors = processors;
    this->myRank = myRank;
    this->running = true;
    this->counter = 0;
    resetWorkCounter();
    this->nextProcesor = (myRank + 1) % processors;
    this->previousprocesor = (myRank - 1) % processors;
    this->workBufferLenght = workBufferLenght;
    this->tokenCounter=0;
    this->color = BLACK;
}

void WorkerParallel::sendToken(int colorToSend) {
   // cout << "rank:" << myRank << " sending token: color=" << colorToSend << " sending to: " << nextProcesor << endl;
    MPI_Send(&colorToSend, 1, MPI_INT, nextProcesor, MSG_TOKEN, MPI_COMM_WORLD);
}

void WorkerParallel::sendFinish() {
   // cout << "rank:" << myRank << " send finishing:" << endl;
    MPI_Send(&myRank, 1, MPI_INT, nextProcesor, MSG_FINISH, MPI_COMM_WORLD);
}

void WorkerParallel::sendRequestForWork(int destination) {
    color = WHITE;
    if (iswaitingForRequestWork)return;
    iswaitingForRequestWork = true;
   // cout << "rank:" << myRank << " Request for work to destination:" << destination << endl;
    expectedWorkFrom = destination;
    MPI_Send(&myRank, 1, MPI_INT, destination, MSG_WORK_REQUEST, MPI_COMM_WORLD);
}

void WorkerParallel::handleNoWork() {    
    int senderRank;
    MPI_Recv(&senderRank, 1, MPI_INT, expectedWorkFrom, MSG_WORK_NOWORK, MPI_COMM_WORLD, &status);
    if (procesorWorkCounter == processors - 1) {
        //prepne a ceka na finish        
    } else {
        procesorWorkCounter++;
        iswaitingForRequestWork = false;
        sendRequestForWork((senderRank + 1) % processors);
    }
}

void WorkerParallel::handleCommunication() {
    int flag;
    MPI_Iprobe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &flag, &status);
    if (flag) {        
        //prisla zprava, je treba ji obslouzit
        //v promenne status je tag (status.MPI_TAG), cislo odesilatele (status.MPI_SOURCE)
        //a pripadne cislo chyby (status.MPI_ERROR)
        switch (status.MPI_TAG) {
            case MSG_WORK_REQUEST: // zadost o praci, prijmout a dopovedet
                // zaslat rozdeleny zasobnik a nebo odmitnuti MSG_WORK_NOWORK
                handleRequestForwork();
                break;
            case MSG_WORK_SENT: // prisel rozdeleny zasobnik, prijmout
                // deserializovat a spustit vypocet
                handleWork();
                break;
            case MSG_WORK_NOWORK: // odmitnuti zadosti o praci
                // zkusit jiny proces
                // a nebo se prepnout do pasivniho stavu a cekat na token
                handleNoWork();
                break;
            case MSG_TOKEN:
                handleToken();
                break;
            case MSG_FINISH: //konec vypoctu - proces 0 pomoci tokenu zjistil, ze jiz nikdo nema praci
                //a rozeslal zpravu ukoncujici vypocet
                //mam-li reseni, odeslu procesu 0
                //nasledne ukoncim spoji cinnost
                //jestlize se meri cas, nezapomen zavolat koncovou barieru MPI_Barrier (MPI_COMM_WORLD)
                handleFinish();
                break;
            default: cerr << "neznamy typ zpravy" << endl;
                break;
        }
    }
}

void WorkerParallel::handleRequestForwork() {
    int senderRank;
    MPI_Recv(&senderRank, 1, MPI_INT, MPI_ANY_SOURCE, MSG_WORK_REQUEST, MPI_COMM_WORLD, &status);

    if (staciklist.empty()) {
        //cout << myRank << " no work for: " << senderRank << endl;
        MPI_Send(&myRank, 1, MPI_INT, senderRank, MSG_WORK_NOWORK, MPI_COMM_WORLD);
    } else {       

        Node* node = staciklist.back();
        
        //cout << myRank << " sending work for: " << senderRank<<"| array:";
        //BinaryVector::printArray(helpArray,sizeOfHelpArray);
        //node->printNode();

        int maxHamWeight = node->getMaximalHammWeight();
        int level = node->getLevel();
        bool isLeaf = node->isIsLeaf();
        bool value = node->getValue();

        char buffer[workBufferLenght];
        int position = 0;
        
        MPI_Pack(helpArray, sizeOfHelpArray, MPI_C_BOOL, buffer, workBufferLenght, &position, MPI_COMM_WORLD);
        MPI_Pack(&maxHamWeight, 1, MPI_INT, buffer, workBufferLenght, &position, MPI_COMM_WORLD);
        MPI_Pack(&level, 1, MPI_INT, buffer, workBufferLenght, &position, MPI_COMM_WORLD);
        MPI_Pack(&isLeaf, 1, MPI_C_BOOL, buffer, workBufferLenght, &position, MPI_COMM_WORLD);
        MPI_Pack(&value, 1, MPI_C_BOOL, buffer, workBufferLenght, &position, MPI_COMM_WORLD);
        MPI_Send(buffer, position, MPI_PACKED, senderRank, MSG_WORK_SENT, MPI_COMM_WORLD);
        
        staciklist.pop_back();
        delete node;
    }
}

void WorkerParallel::handleWork() {
    iswaitingForRequestWork = false;
    char buffer[workBufferLenght];
    int position = 0;   
    int maxHamWeight, level;
    bool isLeaf, value; 
    
    MPI_Recv(buffer, workBufferLenght, MPI_PACKED, expectedWorkFrom, MSG_WORK_SENT, MPI_COMM_WORLD, &status);
    MPI_Unpack(buffer, workBufferLenght, &position, helpArray, sizeOfHelpArray, MPI_C_BOOL, MPI_COMM_WORLD);  
    MPI_Unpack(buffer, workBufferLenght, &position, &maxHamWeight, 1, MPI_INT, MPI_COMM_WORLD);
    MPI_Unpack(buffer, workBufferLenght, &position, &level, 1, MPI_INT, MPI_COMM_WORLD);
    MPI_Unpack(buffer, workBufferLenght, &position, &isLeaf, 1, MPI_C_BOOL, MPI_COMM_WORLD);
    MPI_Unpack(buffer, workBufferLenght, &position, &value, 1, MPI_C_BOOL, MPI_COMM_WORLD);
    
   // cout <<"WORK:"<< myRank << ": work from " << expectedWorkFrom<<" | array="; 
    
    Node* node;
    if(isLeaf){
       node=new LeafNode(maxHamWeight,level,value);
    }else{
       node=new ParentNode(maxHamWeight,level,value); 
    }
//    BinaryVector::printArray(helpArray,sizeOfHelpArray);
//    node->printNode();

    staciklist.push_front(node);
    resetWorkCounter();        
}


//HELP METHODS

void WorkerParallel::resetWorkCounter() {
    this->procesorWorkCounter = 1;
}

queue<SmartStruct*> WorkerParallel::prepareWorkForProcessors(TaskCrate* taskCrate, int numberOfProcessors){
    queue<SmartStruct*> smartQueue;

    int size = (taskCrate->getSize());
    bool* initialArray = BinaryVector::getEmptyArray(size);

    SmartStruct* first = new SmartStruct(size, initialArray, NULL);
    smartQueue.push(first);
    while (smartQueue.size() != numberOfProcessors) {
        SmartStruct* pop = smartQueue.front();
        smartQueue.pop();
        smartQueue.push(pop->getLEftStruct());
        smartQueue.push(pop->getRightStruct());
        delete pop;
    }
    return smartQueue;
}