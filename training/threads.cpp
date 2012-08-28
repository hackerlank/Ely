//#include <string>
//#include <iostream>
//#include <sstream>
//#include <set>
//#include <queue>
//#include <list>
//#include <utility>
//#include <cstdlib>
//#include <event.h>
//#include <cmath>
//#include <throw_event.h>
//#include <reMutex.h>
//#include <reMutexHolder.h>
//#include <conditionVarFull.h>
//#include <genericAsyncTask.h>
//#include <asyncTaskManager.h>
//#include <asyncTaskChain.h>
//#include <genericThread.h>
//#include <pointerTo.h>
//#include <referenceCount.h>
//#include <typedWritableReferenceCount.h>
//#include "Utilities/Tools.h"

////the data
//struct MSG
//{
//	MSG() :
//			_ready(false)
//	{
//	}
//	bool _ready;
//	std::string _msg;
//} msg;
////condition variable with no shared mutex
//struct CVFWithMutex
//{
//	CVFWithMutex()
//	{
//		_cond = new ConditionVarFull(_mutex);
//	}
//	~CVFWithMutex()
//	{
//		delete _cond;
//	}
//	ConditionVarFull& getCV()
//	{
//		return *_cond;
//	}
//	ConditionVarFull* _cond;
//	Mutex _mutex;
//} msgCond;
//
//void msgProducer(void* data)
//{
//	std::string threadName = Thread::get_current_thread()->get_sync_name();
//	std::string msgIn;
//	ConditionVarFull& msgCondIn = msgCond.getCV();
//	while (std::cin >> msgIn)
//	{
//		MutexHolder lock(msgCondIn.get_mutex());
//		((MSG*) data)->_msg = msgIn;
//		((MSG*) data)->_ready = true;
////		msgCondIn.notify_all();
//		msgCondIn.notify();
//		std::cout << threadName << " got '" << msgIn << "'" << std::endl;
//		if (msgIn == "stopp")
//		{
//			break;
//		}
//	}
//
//}
//
//void msgConsumer(void* data)
//{
//	std::string threadName = Thread::get_current_thread()->get_sync_name();
//	std::string outMsg;
//	ConditionVarFull& msgCondIn = msgCond.getCV();
//	while (true)
//	{
//		msgCondIn.get_mutex().acquire();
//		std::cout << threadName << ": falling asleep" << std::endl;
//		msgCondIn.wait();
//		std::cout << threadName << ": awakening" << std::endl;
//		if (((MSG*) data)->_ready == false)
//		{
//			msgCondIn.get_mutex().release();
//			continue;
//		}
//		outMsg = ((MSG*) data)->_msg;
//		((MSG*) data)->_ready = false;
//		msgCondIn.get_mutex().release();
//		//process data
//		std::cout << threadName << ": " << outMsg << std::endl;
//		if (outMsg == "stopc")
//		{
//			break;
//		}
//	}
//}
//
//const int MAXPRODUCERS = 1;
//const int MAXCONSUMERS = 2;

//int main(int argc, char **argv)
//{
//	SMARTPTR(GenericThread) producers[MAXPRODUCERS];
//	SMARTPTR(GenericThread) consumers[MAXCONSUMERS];
//	//create producer/consumer threads
//	for (int prod = 0; prod < MAXPRODUCERS; ++prod)
//	{
//		std::ostringstream name;
//		name << "producer" << prod;
//		producers[prod] = new GenericThread(name.str(), name.str(),
//				&msgProducer, (void*) &msg);
//	}
//	for (int cons = 0; cons < MAXCONSUMERS; ++cons)
//	{
//		std::ostringstream name;
//		name << "consumer" << cons;
//		consumers[cons] = new GenericThread(name.str(), name.str(),
//				&msgConsumer, (void*) &msg);
//	}
//	//start producer/consumer threads
//	for (int prod = 0; prod < MAXPRODUCERS; ++prod)
//	{
//		std::cout << producers[prod]->start(TP_normal, true) << std::endl;
//	}
//	for (int cons = 0; cons < MAXCONSUMERS; ++cons)
//	{
//		std::cout << consumers[cons]->start(TP_normal, true) << std::endl;
//	}
//	//join producer/consumer threads
//	for (int prod = 0; prod < MAXPRODUCERS; ++prod)
//	{
//		std::cout << producers[prod]->get_sync_name() << " joined" << std::endl;
//		producers[prod]->join();
//	}
//	for (int cons = 0; cons < MAXCONSUMERS; ++cons)
//	{
//		std::cout << consumers[cons]->get_sync_name() << " joined" << std::endl;
//		consumers[cons]->join();
//	}
//	return 0;
//}

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
//	SMARTPTR(GenericThread) threads[MAXTHREADS];
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

#include <cstdlib>
#include <dlfcn.h>
#include <iostream>
#include <string>

typedef void (*PFUNC)(int *);

int main(int argc, char **argv)
{
	void *lib_handle;
	PFUNC pfn1, pfn2, pfn11;
	std::string* fn1, *fn2, *fn11;
	int x;
	char *error;

	lib_handle =
			dlopen(
					"/REPOSITORY/KProjects/Eclipse/ElyCallbacks/Debug-thread/libElyCallbacks.so",
					RTLD_LAZY);
	if (!lib_handle)
	{
		std::cerr << dlerror() << std::endl;
		exit(1);
	}

	//take the name and then the pointer
	fn1 = (std::string*) dlsym(lib_handle, "event1");
	if ((error = dlerror()) != NULL)
	{
		std::cerr << error << std::endl;
		exit(1);
	}
	pfn1 = (PFUNC) dlsym(lib_handle, fn1->c_str());
	if ((error = dlerror()) != NULL)
	{
		std::cerr << error << std::endl;
		exit(1);
	}

	//take the name and then the pointer
	fn2 = (std::string*) dlsym(lib_handle, "event2");
	if ((error = dlerror()) != NULL)
	{
		std::cerr << error << std::endl;
		exit(1);
	}
	pfn2 = (PFUNC) dlsym(lib_handle, fn2->c_str());
	if ((error = dlerror()) != NULL)
	{
		std::cerr << error << std::endl;
		exit(1);
	}

	//take the name and then the pointer
	fn11 = (std::string*) dlsym(lib_handle, "event3");
	if ((error = dlerror()) != NULL)
	{
		std::cerr << error << std::endl;
		exit(1);
	}
	pfn11 = (PFUNC) dlsym(lib_handle, fn11->c_str());
	if ((error = dlerror()) != NULL)
	{
		std::cerr << error << std::endl;
		exit(1);
	}

	//
	x = 1000;
	(*pfn1)(&x);
	x = 1001;
	(*pfn2)(&x);
	x = 1002;
	(*pfn11)(&x);

	dlclose(lib_handle);
	return 0;
}
