
/******************************************************************************

                              Online C++ Compiler.
               Code, Compile, Run and Debug C++ program online.
Write your code in this editor and press "Run" button to compile and execute it.

*******************************************************************************/

#include <iostream>
#include <string>
#include <fstream>
#include <sstream>

#include<mutex>
#include<deque>
#include <thread>
#include <condition_variable>

using namespace std;

//#ifndef LOGSTORAGE_H
//#define LOGSTORAGE_H



// Pure virtual class act as a interface. This interface class set few rules which will be implemented by actual logging class
class ILogger
{
public:
	virtual void write(std::string msg) = 0;
	virtual void free() = 0;
};

enum LOGGERTYPE {DBLOGGER, FILELOGGER,CONSOLELOGGER};

/*
// Helper function which convert string to enum type
LOGGERTYPE HelperFunction(std::string logtype)
{
	if(logtype.compare(logtype,std::string("File")) == 0)
		return FILELOGGER;
	else if(logtype.compare(logtype,std::string("DB")) == 0 )
		return DBLOGGER;
	else
		return CONSOLELOGGER;
}
*/
class DBLogger : public ILogger
{
public:
	DBLogger(){}
	void write() {}
	void Free(){}
	~DBLogger(){}
};

class HttpLogger : public ILogger
{
public:

	HttpLogger(){}
	void write() {}
	void Free(){}
	~HttpLogger(){}

};

class FileLogger : public ILogger
{
private:
	std::string m_filename;
	ofstream fout;

public:
	FileLogger()
	{ 
		// Here we can take the filename with path from the configure file but here I am using hardcode sample.txt file.
		m_filename = "sample.txt";
	        fout.open(m_filename,std::ios::app);		
	}
	void write(std::string msg) 
	{
		if(fout.is_open())
			fout << msg << std::endl;	
	}
	void free()
	{
		delete this;
	}
	~FileLogger()
	{
		if(fout.is_open())
			fout.close();
	}
};

class ConsoleLogger : public ILogger
{
public:
	ConsoleLogger(){}
	void write() {}
	void Free(){}
	~ConsoleLogger(){}

};


// Class which will create the object of the actual logger class.
class LogStorage
{
public:
	static ILogger* GetLogger()
	{
		/*switch(HelperFunction(logtype))
		{
			case DBLOGGER:
				return new DBLogger();
			case FILELOGGER:
				return new FileLogger();
				
			case CONSOLELOGGER: 
				return new ConsoleLogger();
			default:
				break;	
		}		
		return nullptr;*/
		return new FileLogger();
	}
};

#define LOG_DEBUG(Message) (ThreadSafeLogger::Debug(Message))
#define LOG_INFO(Message) (ThreadSafeLogger::Info(Message))
#define LOG_ERROR(Message) (ThreadSafeLogger::Error(Message))



class ThreadSafeLogger
{

public: 
static bool m_IsIntialized ;
public:
	

	std::mutex log_mutex;
	std::condition_variable cv;
	ILogger* logger;
	std::deque<std::string> dq;
public: 
	
	static void LoggerIntializer()
	{
		if(!m_IsIntialized )
		{
			m_IsIntialized = true;
			ThreadSafeLogger& logger_instance = GetInstance();
			logger_instance.logger = LogStorage::GetLogger();
			logger_instance.StartWorkerThread();
		}			
	}

	//template<typename... Args>
	static void Debug(const std::string msg)
	{
	    std::string debug ("[Debug]\t"); 
		GetInstance().log(debug,msg);
	}

	//template<typename... Args>
	static void Info(const std::string msg)
	{
		GetInstance().log(std::string("[Info]\t"),msg);
	}

	//template<typename... Args>
	static void Error(const std::string msg)
	{
		GetInstance().log(std::string("[Error]\t"),msg);
	}
private:

	ThreadSafeLogger(){}
    ThreadSafeLogger(const ThreadSafeLogger&) = delete;
	ThreadSafeLogger operator= (const ThreadSafeLogger&) = delete;
	~ThreadSafeLogger()
	{
		if(logger)
			logger->free();
	}
	


	static ThreadSafeLogger& GetInstance()
	{
		static ThreadSafeLogger tslogger;
		return tslogger;
	}
	
	
	
	static void logwriter(ThreadSafeLogger* ptr)
	{
	   
        ptr->dq.clear();
		while(1)
		{
			unique_lock<mutex> lock(ptr->log_mutex);
			ptr->cv.wait(lock, [&]() { return ! ptr->dq.empty();} ); 
			
			std::string data = ptr->dq.back();
            std::cout << data;
            ptr->logger->write(data);
			ptr->dq.pop_back();
			lock.unlock();
		}
	}
	
	void StartWorkerThread()
	{
		std::thread* t = new std::thread(logwriter, this);
		//std::async(logwriter);
		//t.join();
	}
	
	//template<typename... Args>
	void log(std::string message_prior, std::string msg)
	{
		stringstream str;

		str << message_prior;
		str << msg;
		//str << args...;
		str <<endl;	

        //cout << msg;
		std::unique_lock<mutex> lock(log_mutex);
		dq.push_front(str.str());
		lock.unlock();

		cv.notify_all();
		std::this_thread::sleep_for(chrono::seconds(1));
	}
		
};

bool ThreadSafeLogger::m_IsIntialized = false;
//#endif

void Thread1(std::string msg)
{
    LOG_DEBUG(msg);    
}

void Thread2(std::string msg)
{
    LOG_DEBUG(msg);    
}

void Thread3(std::string msg)
{
    LOG_DEBUG(msg);    
}

int main()
{
    
    ThreadSafeLogger::LoggerIntializer();
    
std::thread t(Thread1, std::string("gupta"));
    t.join();
    std::string hitesh = "Hitesh";
    
	
    LOG_DEBUG(hitesh);
    LOG_DEBUG(hitesh);
    LOG_DEBUG(hitesh);
    LOG_DEBUG(hitesh);
    
std::thread t1(Thread2, std::string("test1"));
   t1.join();
std::thread t2(Thread3, std::string("test2"));
    t2.join();
    cout<<"Hello World";

    return 0;
}
