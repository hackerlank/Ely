#include <string>
#include <iostream>
#include <sstream>
#include <set>
#include <queue>
#include <list>
#include <utility>
#include <cstdlib>
#include <event.h>
#include <cmath>
#include <throw_event.h>
#include <reMutex.h>
#include <reMutexHolder.h>
#include <conditionVarFull.h>
#include <genericAsyncTask.h>
#include <asyncTaskManager.h>
#include <asyncTaskChain.h>
#include <genericThread.h>
#include <pointerTo.h>
#include <referenceCount.h>
#include <typedWritableReferenceCount.h>
#include "Utilities/Tools.h"

std::string msg;
ConditionVarFull msgCond;

void msgProducer(void* data)
{
	std::string msgIn;
	while (std::cin >> msgIn)
	{
		MutexHolder lock(msgCond.get_mutex());
		*((std::string*)data) = msgIn;
		msgCond.notify_all();
	}

}

void msgConsumer(void* data)
{
	while(true)
	{
		msgCond.get_mutex().acquire();
	    msgCond.wait();
	    data_cond.wait(
	        lk,[]{return !data_queue.empty();});
	    data_chunk data=data_queue.front();
	    data_queue.pop();
	    lk.unlock();
	    process(data);
	    if(is_last_chunk(data))
	        break;
	}
}

int main(int argc, char **argv)
{
	int inputs[MAXTHREADS];
	PT(GenericThread) threads[MAXTHREADS];
	//create thread objects
	for (int j = 0; j < MAXTHREADS; ++j)
	{
		int num = (rand() % MAXNUM + 1);
		inputs[j] = num;
		std::ostringstream name;
		name << "thread" << j;
		threads[j] = new GenericThread(name.str(), name.str(), &f,
				(void*) &inputs[j]);
	}
	//start threads
	for (int j = 0; j < MAXTHREADS; ++j)
	{
		std::cout << threads[j]->start(TP_normal, true) << std::endl;
	}
	//join threads
	for (int j = 0; j < MAXTHREADS; ++j)
	{
		std::cout << "thread " << threads[j]->get_sync_name() << " joined"
				<< std::endl;
		threads[j]->join();
	}
	return 0;
}

//void f(void* data)
//{
//	std::string threadName = Thread::get_current_thread()->get_sync_name();
//	int i = *((int*) data);
//	for (int k = 0; k < i; ++k)
//	{
//		std::cout << threadName << ": " << sqrt((double) k) << std::endl;
//	}
//}
//
//const int MAXTHREADS = 2;
//const int MAXNUM = 100000000;
//
//int main(int argc, char **argv)
//{
////	srand(100);
//	int inputs[MAXTHREADS];
//	PT(GenericThread) threads[MAXTHREADS];
//	//create thread objects
//	for (int j = 0; j < MAXTHREADS; ++j)
//	{
//		int num = (rand() % MAXNUM + 1);
//		inputs[j] = num;
//		std::ostringstream name;
//		name << "thread" << j;
//		threads[j] = new GenericThread(name.str(), name.str(), &f,
//				(void*) &inputs[j]);
//	}
//	//start threads
//	for (int j = 0; j < MAXTHREADS; ++j)
//	{
//		std::cout << threads[j]->start(TP_normal, true) << std::endl;
//	}
//	//join threads
//	for (int j = 0; j < MAXTHREADS; ++j)
//	{
//		std::cout << "thread " << threads[j]->get_sync_name() << " joined"
//				<< std::endl;
//		threads[j]->join();
//	}
//	return 0;
//}
//
