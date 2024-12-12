#include<iostream>
#include<vector>
#include<thread>
#include<mutex>
#include <chrono>
#define int long long
using namespace std; 

mutex g_mutex;
int gloabal_sum = 0;

void cal_mutex(int i, int n, int x){
    // cout<<"Thread "<<i<<" is running\n"; 
    int tmp = 0;   
    int bound= (i==x) ? n : i * (n/x);
    for(int j = (i-1) * (n/x) + 1 ; j <= bound ; ++j){
        tmp += j;
    }   
    //lock_guard is used to lock the mutex, it will unlock the mutex when it goes out of scope
    lock_guard<mutex> lock(g_mutex);
    gloabal_sum += tmp;
}

void cal(int i, int n, int x, int&sum){
    // cout<<"Thread "<<i<<" is running\n"; 
    int tmp = 0;   
    int bound= (i==x) ? n : i * (n/x);
    for(int j = (i-1) * (n/x) + 1 ; j <= bound ; ++j){
        tmp += j;
    }   
    sum += tmp;
}

void sol(int n){
    int sum = 0;
    cal(1,n,1,ref(sum));
    cout<<"Result by single thread: "<<sum<<"\n";
}

void sol_in_multithread(int x, int n){
    vector<thread> threads;
    vector<int> results(x,0);
    for(int  i=1;i<=x;++i){
        threads.emplace_back(thread(cal,i,n,x,ref(results[i-1])));
    }
    
    for(auto &th: threads){
        th.join();
    }

    int sum = 0;  
    for(auto &res: results){
        sum += res;
    }   
    cout<<"Result by mulitple threads: "<<sum<<"\n";   
}

void sol_in_multithread_with_mutex(int x, int n){
    vector<thread> threads;
    
    for(int  i=1;i<=x;++i){
        threads.emplace_back(thread(cal_mutex,i,n,x));
    }
    
    for(auto &th: threads){
        th.join();
    }

    cout<<"Result by mulitple threads with mutex: "<<gloabal_sum<<"\n";   
}

template<typename F, typename... Args>
double timer(F&& f, string func_name, Args&&... args) {
    auto start_time = chrono::high_resolution_clock::now();
    f(forward<Args>(args)...);
    auto end_time = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::microseconds>(end_time - start_time);
    cout << "Time taken by " << func_name << ": " << duration.count() / 1e6 << " seconds\n\n";
    return (double)duration.count();
}

//compile: g++ main.cpp
signed main(){
    int thread_n, n;
    cout<<"Enter number of threads and n: ";
    cin>>thread_n>>n;
   // Use timer for sol
    double t1 = timer(sol, "solve in single thread", n);
    // Use timer for sol_in_thread
    double t2 = timer(sol_in_multithread, "solve in multithreads", thread_n, n);
    // Use timer for sol_in_multithread_with_mutex
    double t3 = timer(sol_in_multithread_with_mutex, "solve in multithreads with Mutex", thread_n, n);
    cout<<"multithreads vs. single thread speedup: "<<t1 / t2 <<" times\n";
    return 0;
}