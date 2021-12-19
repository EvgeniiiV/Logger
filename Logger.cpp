#include <iostream>
#include <vector>
#include <windows.h>
#include <string>
#include <fstream>
#include <mutex>
#include <shared_mutex>
#include <thread>
using namespace std;


class Logger {
private:

	mutex c_out;
	shared_mutex shm;
	fstream log;
	SYSTEMTIME lt;

public:
	Logger()
	{
		log.open("log.txt", fstream::in | fstream::out | fstream::app);
		if (!log.is_open())
			cout << "Error of open()\n";
		else cout << "log.txt: open() is ok\n";
	}

	~Logger() {
		log.close();
		//remove("log.txt");
	};

	void write_log(const string& sender, const string& receiver, const string& text)
	{
		GetLocalTime(&lt);
		lock_guard<shared_mutex>w_guard(shm);
		log << lt.wYear << "." << lt.wMonth << "." << lt.wDay << " " << lt.wHour << ":" << lt.wMinute << " " << "Sender: " << sender << " Receiver: " << receiver << " Text: " << text << '\n';
	}

	void show(const string& str)
	{
		lock_guard<mutex>c_guard(c_out);
		cout << "thread ID: " << this_thread::get_id() << "  ";
		cout << str << endl;
	}

	void read_log(int num_of_string)
	{
		shm.lock_shared();
		ifstream LOG("log.txt");
		log.clear();
		log.seekg(0, ios::beg);
		string line;
		for (int i = 0; i < num_of_string; i++)
		{
			getline(LOG, line);
		}
		shm.unlock_shared();
		show("String N" + to_string(num_of_string) + ": " + line);
	}

};

int main()
{
	Logger l;
	thread th1([&]() {l.write_log("A", "A", "AAA"); });
	thread th2([&]() {l.write_log("B", "B", "BBB"); });
	thread th3([&]() {l.write_log("C", "C", "CCC"); });
	thread th4([&]() {l.write_log("D", "D", "DDD"); });

	thread th5([&]() {l.read_log(1); });
	thread th6([&]() {l.read_log(2); });
	thread th7([&]() {l.read_log(3); });
	thread th8([&]() {l.read_log(4); });

	th1.join();
	th2.join();
	th3.join();
	th4.join();

	th5.join();
	th6.join();
	th7.join();
	th8.join();

	return 0;
}