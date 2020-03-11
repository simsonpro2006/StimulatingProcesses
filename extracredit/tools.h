#ifndef TOOLS_H_INCLUDED
#define TOOLS_H_INCLUDED
#include <iostream>
#include <string>
#include <random>
#include <stdio.h>      /* printf, scanf, puts, NULL */
#include <stdlib.h>     /* srand, rand */
#include <time.h>  
#include <vector>
#include  <pthread.h>
#include <math.h>
using namespace std;


pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER; 

void Threadsafeprint(std::string message)
{
    pthread_mutex_lock(&lock);
    cout<<message<<endl;
    pthread_mutex_unlock(&lock);
}

enum ProcessState
{
    create, waiting, executing, ready, terminating
};
enum Processortype
{
    SJF,Priority, FCFS
};


class PCB
{
    public:
        PCB();
        ProcessState getState ();
        void setState(ProcessState State);
        int getworkleft();
        void setworkleft(int timeprocessed);
        int getId();
        int getpriority();
        void setpriority(int priorityupdate);


    private:
        ProcessState currentstate;
        int id;
        int workleft;
        int priority;
        

        


};


PCB::PCB()
{
    currentstate = ProcessState::create;
    id = rand() % 1000 ;
    workleft = rand()%15+5;
    priority = rand()%128;

}

void PCB::setState(ProcessState State)
{
    currentstate = State;
}

ProcessState PCB::getState ()
{
    return currentstate;
}
int PCB::getworkleft()
{
    return workleft;
}
void PCB::setworkleft(int timeprocessed)
{
    workleft -= timeprocessed;
}
int PCB::getId()
{
    return id;
}
int PCB::getpriority()
{
    return priority;
}
void PCB::setpriority(int priorityupdated)
{
    priority = priorityupdated;
}
class Processor
{
    public:
        
        Processor(Processortype processortype,int ID);
        void queue(PCB processes);
        void run();
        void cleanqueue();
        PCB popProcess();
        int getsize();
        int getID();
        void randomizestate();
        void aging();
        void setlock(bool activate);


    private:
        Processortype processortype;
        int procID;
        std::vector<PCB> listofmyPCB;
        pthread_mutex_t proclock = PTHREAD_MUTEX_INITIALIZER; 
        void SJF();
        void Priority();
        void FCFS();



};
Processor::Processor(Processortype type,int ID)
{
    processortype = type;
    procID = ID;
}
int Processor::getID()
{
    return procID;
}
void Processor::cleanqueue()
{
    for (int i = 0; i< listofmyPCB.size(); i++)
    {
        if (listofmyPCB[i].getState()== ProcessState::terminating)
        {
            listofmyPCB.erase (listofmyPCB.begin()+i);
        }
    }
}
void Processor::queue(PCB processes)
{
    processes.setState(ProcessState::ready);
    listofmyPCB.push_back(processes);
}
void Processor::run()
{
    if (processortype == Processortype::SJF)
    {
        SJF();
    }
    if (processortype == Processortype::FCFS)
    {
        FCFS();
    }
    if (processortype == Processortype::Priority)
    {
        Priority();
    }

}
void Processor::SJF()
{
    int PCBindex = 0;


    for (int i = 1 ; i< listofmyPCB.size();i++)
    {
        if(listofmyPCB[i].getworkleft()<listofmyPCB[PCBindex].getworkleft())
        {
            PCBindex =i;
        }
    }

    std::string message = string ("process ID ") + std::to_string(getID()) +string(" work left at postion ")+std::to_string(PCBindex) + string(":") + std::to_string(listofmyPCB[PCBindex].getworkleft()) 
        + std::string(" and the state of the process is ")+ std::to_string(listofmyPCB[PCBindex].getState());
    Threadsafeprint(message);

    if (listofmyPCB[PCBindex].getState()== ProcessState::ready)
    {
        listofmyPCB[PCBindex].setState(ProcessState::executing);
        int bursttime = rand()%11;
        listofmyPCB[PCBindex].setworkleft(bursttime);
        if (listofmyPCB[PCBindex].getworkleft()<= 0)
        {
            listofmyPCB[PCBindex].setState(ProcessState::terminating);
        }
        else
        {
            listofmyPCB[PCBindex].setState(ProcessState::ready);
        }
        

    }

    
    
}
void Processor::Priority()
{
    int PCBindex = 0;
    for (int i = 1 ; i< listofmyPCB.size();i++)
    {
        if(listofmyPCB[i].getpriority()<listofmyPCB[PCBindex].getpriority())
        {
            PCBindex =i;
        }
    }

    std::string message = string ("process ID ") + std::to_string(getID()) +string(" work left at postion ")+std::to_string(PCBindex) + string(":") + std::to_string(listofmyPCB[PCBindex].getworkleft()) 
        + std::string(" and the state of the process is ")+ std::to_string(listofmyPCB[PCBindex].getState());
    Threadsafeprint(message);

    if (listofmyPCB[PCBindex].getState()== ProcessState::ready)
    {

        listofmyPCB[PCBindex].setState(ProcessState::executing);
        listofmyPCB[PCBindex].setworkleft(1);
        if (listofmyPCB[PCBindex].getworkleft()<= 0)
        {
            listofmyPCB[PCBindex].setState(ProcessState::terminating);
        }
        else
        {
            listofmyPCB[PCBindex].setState(ProcessState::ready);
        }
    
    }

}
void Processor::FCFS()
{
    PCB cur = listofmyPCB[0];

    std::string message = string ("process ID ") + std::to_string(getID()) +string(" work left at postion ")+std::to_string(0) + string(":") + std::to_string(cur.getworkleft()) 
        + std::string(" and the state of the process is ")+ std::to_string(cur.getState());
    Threadsafeprint(message);

    if (cur.getState()== ProcessState::ready)
    {
        listofmyPCB.erase(listofmyPCB.begin());

        int bursttime = rand()%11;
        
        cur.setState(ProcessState::executing);
        cur.setworkleft(bursttime);

        
        if (cur.getworkleft()<= 0)
        {
            cur.setState(ProcessState::terminating);
        }
        else
        {
            cur.setState(ProcessState::ready);
        }
        
        listofmyPCB.push_back(cur);
    }
    
}
PCB Processor::popProcess()
{
    PCB a = listofmyPCB.back();
    listofmyPCB.pop_back();
    return a;
}
int Processor::getsize()
{
    return listofmyPCB.size();
}

void Processor::randomizestate()
{
    for(int i =0; i< listofmyPCB.size();i++)
    {
        if (listofmyPCB[i].getState() != ProcessState::terminating)
        {
            int a = rand()%2;
            if (a== 0)
            {
                listofmyPCB[i].setState(ProcessState::ready);
            }
            else
            {
                listofmyPCB[i].setState(ProcessState::waiting);
            }
            

        }
    }
}
void Processor::aging()
{
    for (int i = 0; i<listofmyPCB.size();i++)
    {
        if (listofmyPCB[i].getState() != ProcessState::terminating)
        {
            int a = listofmyPCB[i].getpriority() -1;
            listofmyPCB[i].setpriority(a);
        }
    }
}
void Processor::setlock(bool activate)
{
    if (activate)
    {
        pthread_mutex_lock(&proclock);
        return;
    }
    pthread_mutex_unlock(&proclock);
    
}

#endif