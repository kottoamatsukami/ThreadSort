#include <iostream>
#include <pthread.h>
#include <fstream>
#include <array>
#include <vector>

#define THREAD_COUNT   4

using namespace std;
 
vector<int> a;
int part = 0;
vector<int> parts;


void make_partition(vector<int> a)
{
    int step = ( a.size() - 1 )/THREAD_COUNT;
    if(step == 0) {return;}

    int len = 0;
    while(len < a.size()-1)
    {
        parts.push_back(len);
        len += step;
    }

    if(a.size()-1 - (len - step) >= 100)
        parts.push_back(a.size()-1);
    else
    {
        parts.pop_back();
        parts.push_back(a.size()-1);
    }
}

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
 
    while (i < n1 && j < n2) 
    {
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

    int low = -1;
    if(parts[thread_part] > 0)
        low = parts[thread_part] + 1;
    else
        low = parts[thread_part];

    int high = parts[thread_part+1];

    int mid = low + (high - low) / 2;

    if (low < high && low != -1) 
    {
        merge_sort(low, mid);
        merge_sort(mid + 1, high);
        merge(low, mid, high);
    }

    return nullptr;
}

void* merge_sorted_parts(void* arg)
{
    int thread_part = part;
    part += 2;
    
    int low = -1;
    if(parts[thread_part] > 0)
        low = parts[thread_part] + 1;
    else
        low = parts[thread_part];
        
    int mid = parts[thread_part+1];
    int high = parts[thread_part+2];

    merge(low, mid, high);

    return nullptr;
}
    
 
// Driver Code
int main()
{
    // Read the file
    std::fstream file("../input.txt", std::ios_base::in);

    int cur_number;
    while (file >> cur_number)
    {
        a.push_back(cur_number);
    }

    array<pthread_t,THREAD_COUNT> threads{};
    make_partition(a);

    int part_count = THREAD_COUNT;

    if(part_count >= a.size() / 2)
    {
        return 0;
    }
    else if(part_count >= 2)
    {
        for (int i = 0; i < THREAD_COUNT; i++)
            pthread_create(&threads[i],
                           NULL,
                           merge_sort,
                           (void*)NULL);

        for (int i = 0; i < THREAD_COUNT; i++)
            pthread_join(threads[i], NULL);

        while(part_count > 2)
        {
            part = 0;
            if(parts.size() % 2 != 0) //if even number of parts (size should be odd)
            {
                for (int i = 0; i < part_count; i += 2)
                    pthread_create(&threads[i],
                                   NULL,
                                   merge_sorted_parts,
                                   (void*)NULL);
                for (int i = 0; i < part_count; i += 2)
                    pthread_join(threads[i], NULL);
            }
            else
            {
                int thread_part = 0;

                int low = parts[thread_part];
                int mid = parts[thread_part+1];
                int high = parts[thread_part+2];

                merge(low, mid, high);

                parts.erase(parts.begin() + 1);
                part_count--;
                continue;
            }

            for(int i = parts.size() - 2; i >= 1; i -= 2)
            {
                parts.erase(parts.begin() + i);
            }
            part_count /= 2;
        }

        merge(parts[0], parts[1], parts[2]);
    }
    else if(part_count == 1)
    {
        merge_sort(0, a.size());
    }
    else
    {
        return 0;
    }

    // Save the file
    ofstream out_file;
    out_file.open("../out.txt");
    if (out_file.is_open()) {
        for (int i : a)
        {
            out_file << i << '\n';
        }
    }
    else {
        std::cerr << "Error opening file\n";
    }


    return 0;
}