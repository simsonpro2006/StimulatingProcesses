#include <unistd.h>
#include "tools.h"
using namespace std;

const int TICK_LEN = 100;
bool running =true;
struct loadatt
{
    Processor *S;
    Processor *P;
    Processor *F;
};
void loadbalancing(Processor *S,Processor *P,Processor *F)
{
    S->setlock(true);
    P->setlock(true);
    F->setlock(true);
    if (S->getsize()==0||P->getsize()==0||F->getsize()==0)
    {   cout<<endl<<"load balancing start "<<endl;
        cout<<" size of S before "<< S->getsize()<< endl;
        cout<<" size of P before "<< P->getsize()<< endl;
        cout<<" size of F before "<< F->getsize()<< endl;
        std::vector<PCB> allPCB;
        int amount =0;

        while (S->getsize() > 0)
        {
            allPCB.push_back(S->popProcess());
                     
        }
        while (P->getsize() > 0)
        {
            allPCB.push_back(P->popProcess());
        }
        while (F->getsize() > 0)
        {
            allPCB.push_back(F->popProcess());
        }

        if (allPCB.size()>0)
        {   
            if (allPCB.size()== 2)
            {
                S->queue(allPCB[0]);
                P->queue(allPCB[1]);
            }
            else
            {
                amount = allPCB.size()/3.0;
                amount = (int)std::round(amount);
                for(int i = 0; i<amount;i++)
                {
                    S->queue(allPCB[i]);
                }
                for(int i= amount; i<(amount*2); i++)
                {
                    P->queue(allPCB[i]);
                }
                for(int i= amount*2; i<allPCB.size(); i++)
                {
                    F->queue(allPCB[i]);
            }
            }
            
       }
        cout<<" size of S after "<< S->getsize()<< endl;
        cout<<" size of P after "<< P->getsize()<< endl;
        cout<<" size of F after "<< F->getsize()<< endl<<endl;
    }
     S->setlock(false);
    P->setlock(false);
    F->setlock(false);
}
void *Load(void *s)
{
    loadatt *pt = reinterpret_cast<loadatt*>(s);
    int counter= 0;
    while(running)
    {
        counter ++;
        if (counter%10 ==0)
        {
            loadbalancing(pt->S,pt->P,pt->F);
        }
        usleep(TICK_LEN);
    }
    return nullptr;
}
struct attribute
{
    //Processortype type;
    int numberofProcesses;
    Processor *newprocess;

};
void *ThreadSJF(void * type)
{
    attribute *a = reinterpret_cast<attribute*>(type);
    Processor *p = a->newprocess;

    //Processor newprocess(p->type);
    string msg = "";

    for (int i=0 ; i< a->numberofProcesses; i++)
    {   
        PCB a;
        msg = string("id of process number ") + to_string(i) + " " + to_string(a.getworkleft());
        Threadsafeprint(msg);
        p->queue(a);
    }
    int counter= 0;
    while (running)
    {
        p->setlock(true);
        if (p->getsize()> 0)
        {
            msg = string("process ID ") + to_string(p->getID()) + string(" size of my array is ") + to_string(p->getsize()); 
            Threadsafeprint(msg);
            counter ++;

            
            if (counter%5== 0)
            {
                p->randomizestate();
            }
            if (counter%2 == 0)
            {
                p->aging();
            }
            p->run();   
            p->cleanqueue();
        }
        p->setlock(false);
        usleep(TICK_LEN);
    }
    msg = string("size of my array is at the end ")+ to_string(p->getsize());
    Threadsafeprint(msg);
    return nullptr;

}
int main()
{
    srand (time(NULL));
    int m = 10;//SJF
    int n = 13;//Priority
    int p = 23;//FCFS
    int numThread =3;
    Processortype type1 = Processortype::SJF;
    Processortype type2 = Processortype::Priority;
    Processortype type3 = Processortype::FCFS;
    Processor SJFProcess(type1,0);
    Processor PriorityProcess(type2,1);
    Processor FCFSProcess(type3,2);
    attribute *pt1 = new attribute();
    attribute *pt2 = new attribute();
    attribute *pt3 = new attribute();
    loadatt *ptload= new loadatt();


    pt1->numberofProcesses= m;
    //pt1->type = type1;
    pt1->newprocess =  &SJFProcess;
    pt2->numberofProcesses= n;
    //pt1->type = type1;
    pt2->newprocess =  &PriorityProcess;
    pt3->numberofProcesses= p;
    //pt1->type = type1;
    pt3->newprocess =  &FCFSProcess;
    ptload->S=&SJFProcess;
    ptload->P=&PriorityProcess;
    ptload->F=&FCFSProcess;
    void * SJFpointer = reinterpret_cast<void*>(pt1);
    void * Prioritypointer = reinterpret_cast<void*>(pt2);
    void * FCFSpointer = reinterpret_cast<void*>(pt3);
    pthread_t pthreadid1;
    pthread_t pthreadid2;
    pthread_t pthreadid3;
    pthread_t pthreadid4;
    pthread_create (&pthreadid1,NULL,ThreadSJF,SJFpointer);
    pthread_create (&pthreadid2,NULL,ThreadSJF,Prioritypointer);
    pthread_create (&pthreadid3,NULL,ThreadSJF,FCFSpointer);
    pthread_create (&pthreadid4,NULL,Load,ptload);
    while(running)
    {
        usleep(1000);
        SJFProcess.setlock(true);
        PriorityProcess.setlock(true);
        FCFSProcess.setlock(true);
        int leftproc = 0;
        leftproc += SJFProcess.getsize();
        leftproc += PriorityProcess.getsize();
        leftproc += FCFSProcess.getsize();
        if (leftproc == 0)
        {
            running = false;
        }
        SJFProcess.setlock(false);
        PriorityProcess.setlock(false);
        FCFSProcess.setlock(false);
    }
    pthread_join(pthreadid1,NULL);
    pthread_join(pthreadid2,NULL);
    pthread_join(pthreadid3,NULL);
    pthread_join(pthreadid4,NULL);
    delete ptload;
    delete pt1;
    delete pt2;
    delete pt3;

};