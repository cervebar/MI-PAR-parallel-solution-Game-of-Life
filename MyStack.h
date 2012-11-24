/* 
 * File:   MyStack.h
 * Author: Babu
 *
 * Created on 19. listopad 2012, 20:24
 */

#ifndef MYSTACK_H
#define	MYSTACK_H

#include <stack>

#include "Node.h"

class MyStack {
public:
    MyStack();
    virtual ~MyStack();
private:
    stack<Node*> stacikMy; 

};

#endif	/* MYSTACK_H */

