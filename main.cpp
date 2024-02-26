#include <iostream>
#include <pthread.h>
#include <time.h>
#include <vector>

#define SIZE 20
#define THREAD_COUNT 4

using namespace std;

vector<int> a(SIZE);
int part = 0;

void merge(int low, int mid, int high)
{
    const unsigned lsize = mid - low + 1;
    const unsigned rsize = high - mid;
    vector<int> left(lsize);
    vector<int> right(rsize);

	int n1 = mid - low + 1, n2 = high - mid, i, j;

	for (i = 0; i < n1; i++)
		left[i] = a[i + low];

	for (i = 0; i < n2; i++)
		right[i] = a[i + mid + 1];

	int k = low;
	i = j = 0;

	while (i < n1 && j < n2) {
		if (left[i] <= right[j])
			a[k++] = left[i++];
		else
			a[k++] = right[j++];
	}

	while (i < n1) {
		a[k++] = left[i++];
	}
	while (j < n2) {
		a[k++] = right[j++];
	}
}

void merge_sort(int low, int high)
{
	int mid = low + (high - low) / 2;
	if (low < high) 
    {
		merge_sort(low, mid);
		merge_sort(mid + 1, high);
		merge(low, mid, high);
	}
}

void* merge_sort(void* arg)
{
	int thread_part = part++;

	int low = thread_part * (SIZE / 4);
	int high = (thread_part + 1) * (SIZE / 4) - 1;

	int mid = low + (high - low) / 2;
	if (low < high) 
    {
		merge_sort(low, mid);
		merge_sort(mid + 1, high);
		merge(low, mid, high);
	}

    return nullptr;
}

int main()
{
    srand(time(0)); //comment this to get same values every time
	for (int i = 0; i < SIZE; i++)
		a[i] = rand() % 10000;

	clock_t start, end;

	start = clock();
	pthread_t threads[THREAD_COUNT];

	for (int i = 0; i < THREAD_COUNT; i++)
		pthread_create(&threads[i], NULL, merge_sort,
										(void*)NULL);

	for (int i = 0; i < 4; i++)
		pthread_join(threads[i], NULL);

	merge(0, (SIZE / 2 - 1) / 2, SIZE / 2 - 1);
	merge(SIZE / 2, SIZE/2 + (SIZE-1-SIZE/2)/2, SIZE - 1);
	merge(0, (SIZE - 1)/2, SIZE - 1);

	end = clock();

	cout << "Sorted array: ";
	for (int i = 0; i < SIZE; i++)
		cout << a[i] << " ";

	cout << "\nTime taken: " << ((end - start) / 
			(double)CLOCKS_PER_SEC) * 1000 << " millisec" << endl;

	bool isSorted = true;
    for (int i = 1; i < SIZE; i++)
    {
        if(a[i-1] > a[i])
            isSorted = false;
    }
    isSorted ? cout << "SORTED" << endl : cout << "UNSORTED" << endl;

	return 0;
}