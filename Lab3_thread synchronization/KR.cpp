#include <iostream>
#include <mutex>
#include <vector>
#include <condition_variable>
#include <thread>

using namespace std;

constexpr chrono::milliseconds WaitForEndTime(200);
constexpr chrono::seconds WaitInfinityTime(100);
constexpr chrono::duration < double, milli> WaitAfterOperation(5);
constexpr int MaxSynchoTime(500);
constexpr chrono::duration < double, milli> WaitForCheck(50);

mutex Vector_Lock;
mutex Main_Mutex;
mutex* Marker_Mutex;
condition_variable* cond_val_continue;
condition_variable* cond_val_end;
condition_variable* cond_val_temporal_workend;

bool all_zero(vector<int>& threads)
{
	for (size_t i = 0; i < threads.size(); i++)
	{
		if (threads[i] == 1)
			return false;
	}
	return true;
}

void Marker(vector<int>& Arr, int num)
{
	unique_lock<mutex> Marker_Wait(Marker_Mutex[num - 1]);
	Vector_Lock.lock();

	int index;
	int marked_count = 0;
	vector<int> marked_array(Arr.size());

	for (size_t i = 0; i < Arr.size(); i++)
	{
		marked_array[i] = 0;
	}

	srand(num);

	while (true)
	{
		index = rand() % Arr.size();

		if (Arr[index] == 0)
		{
			this_thread::sleep_for(WaitAfterOperation);
			Arr[index] = num;
			marked_count++;
			marked_array[index] = 1;
			this_thread::sleep_for(WaitAfterOperation);
		}
		else
		{
			cout << "Thread number: " << num << ", marked_count: " << marked_count << ", unmarked index: " << index << endl;

			cond_val_temporal_workend[num - 1].notify_one();
			Vector_Lock.unlock();

			cond_val_continue[num - 1].wait_for(Marker_Wait, WaitInfinityTime);

			if (cond_val_end[num - 1].wait_for(Marker_Wait, WaitForEndTime) == cv_status::timeout)
			{
				Vector_Lock.lock();
				break;
			}
			else
			{
				Vector_Lock.lock();
			}
		}
	}

	for (size_t i = 0; i < Arr.size(); i++)
	{
		if (marked_array[i] == 1)
		{
			Arr[i] = 0;
		}
	}

	Vector_Lock.unlock();

	return;
}

int main() {

	unique_lock<mutex> Main_Lock(Main_Mutex);
	Vector_Lock.lock();

	int size = 0;
	int thread_emount = 0;
	cout << "Enter array size: ";
	cin >> size;
	cout << "Enter marker thread emount: ";
	cin >> thread_emount;
	
	vector<int> Arr(size);
	for (int i = 0; i < Arr.size(); ++i)
		Arr[i] = 0;

	int synchro_time = 0;
	if (thread_emount != 0)
		synchro_time = MaxSynchoTime / thread_emount;
	chrono::duration < double, milli> synchronization_time(synchro_time);

	vector<thread> Threads(thread_emount);
	cond_val_continue = new condition_variable[thread_emount];
	cond_val_end = new condition_variable[thread_emount];
	cond_val_temporal_workend = new condition_variable[thread_emount];
	Marker_Mutex = new mutex[thread_emount];

	for (size_t i = 0; i < Threads.size(); i++)
	{
		Vector_Lock.unlock();
		Threads[i] = thread(Marker, ref(Arr), i + 1);
		cond_val_temporal_workend[i].wait_for(Main_Lock, WaitInfinityTime);
		Vector_Lock.lock();
	}

	vector<int> end_of_threads(thread_emount, 1);

	while (!all_zero(end_of_threads))
	{
		int break_num;
		cout << "Enter num of Marker() to break: ";
		do
		{
			cin >> break_num;
			if (break_num < 1 || break_num > thread_emount)
			{
				cout << "This one doesn't exist, try another: ";
				continue;
			}
			else if (end_of_threads[break_num - 1] == 0)
			{
				cout << "This one is stopped, try another: ";
				continue;
			}
			break;
		} while (true);

		break_num--;

		Vector_Lock.unlock();
		cond_val_continue[break_num].notify_all();

		Threads[break_num].join();

		Vector_Lock.lock();

		cout << "Array: ";
		for (int i = 0; i < Arr.size(); ++i)
		{
			cout << Arr[i] << " ";
		}
		cout << endl;

		end_of_threads[break_num] = 0;

		for (size_t i = 0; i < thread_emount; i++)
		{
			if (end_of_threads[i] != 0)
			{
				Vector_Lock.unlock();
				cond_val_continue[i].notify_all();
				this_thread::sleep_for(WaitForCheck);
				cond_val_end[i].notify_all();
				cond_val_temporal_workend[i].wait_for(Main_Lock, WaitInfinityTime);
				Vector_Lock.lock();
			}
		}
	}

	Vector_Lock.unlock();

	delete[] Marker_Mutex;
	delete[] cond_val_temporal_workend;
	delete[] cond_val_end;
	delete[] cond_val_continue;

	return 0;
}