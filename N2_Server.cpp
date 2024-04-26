#include <iostream>
#include <fstream>
#include <thread>
#include <list>
#include <mutex>
#include <queue>
#include <future>
#include <functional>
#include <condition_variable>
#include <random>
#include <cmath>
#include <unordered_map>

template <typename T>
class TaskServer
{
public:
    void start()
    {
        stop_flag_ = false;
        server_thread_ = std::thread(&TaskServer::server_thread, this);
    }
    void stop()
    {
        {
            std::unique_lock<std::mutex> lock(mutex_);
            stop_flag_ = true;
            cv_.notify_one(); 
        }
        server_thread_.join();
    }
    size_t add_task(std::function<T()> task)
    {
        std::unique_lock<std::mutex> lock(mutex_);
        tasks_.push({ next_id_++, std::async(std::launch::deferred, task) });
        cv_.notify_one();
        return tasks_.back().first;
    }
    T request_result(size_t id)
    {
        std::unique_lock<std::mutex> lock(mutex_);
        cv_.wait(lock, [this, id]() { return results_.find(id) != results_.end(); });
        T result = results_[id];
        results_.erase(id);
        return result;
    }

private:
    std::mutex mutex_;
    std::condition_variable cv_;
    std::thread server_thread_;
    bool stop_flag_ = false;
    size_t next_id_ = 1;
    std::queue<std::pair<size_t, std::future<T>>> tasks_;
    std::unordered_map<size_t, T> results_;

    void server_thread() {
        while (true) {
            std::unique_lock<std::mutex> lock(mutex_);
            cv_.wait(lock, [this]() { return !tasks_.empty() || stop_flag_; }); 
            if (tasks_.empty() && stop_flag_) {
                break; 
            }
            if (!tasks_.empty()) {
                auto task = std::move(tasks_.front()); 
                tasks_.pop(); 
                results_[task.first] = task.second.get();
                cv_.notify_one(); 
            }
        }
    }
};

template <typename T>
class TaskClient {
public:
    void run_client(TaskServer<T>& server, std::function<std::pair<T, T>()> task_generator) {
        auto task = task_generator();
        int id = server.add_task([task]() { return task.second; });
        task_ids_.push_back({ id, task.first });
    }

    std::list<std::pair<T, T>> client_to_result(TaskServer<T>& server) {
        std::list<std::pair<T, T>> results;
        for (const auto& id_pair : task_ids_) {
            T result = server.request_result(id_pair.first);
            results.push_back({ id_pair.second, result });
        }
        return results;
    }

private:
    std::vector<std::pair<int, T>> task_ids_;
};

template<typename T>
std::pair<T, T> random_sin() {
    static std::default_random_engine generator;
    static std::uniform_real_distribution<T> distribution(-3.14159, 3.14159);
    T x = distribution(generator);
    return { x, std::sin(x) };
}

template<typename T>
std::pair<T, T> random_sqrt() {
    static std::default_random_engine generator;
    static std::uniform_real_distribution<T> distribution(1.0, 10.0);
    T value = distribution(generator);
    return { value, std::sqrt(value) };
}

template<typename T>
std::pair<T, T> random_pow() {
    static std::default_random_engine generator;
    static std::uniform_real_distribution<T> distribution(1.0, 10.0);
    T value = distribution(generator);
    return { value, std::pow(value, 2.0) };
}


int main() {
    TaskServer<double> server;
    server.start();


    TaskClient<double> client1;
    TaskClient<double> client2;
    TaskClient<double> client3;

    for (int i = 0; i < 10000; ++i) {
        client1.run_client(server, random_sin<double>);
        client2.run_client(server, random_sqrt<double>);
        client3.run_client(server, random_pow<double>);
    }

    std::list<std::pair<double, double>> t1 = client1.client_to_result(server);
    std::list<std::pair<double, double>> t2 = client2.client_to_result(server);
    std::list<std::pair<double, double>> t3 = client3.client_to_result(server);

    server.stop();


    std::ofstream file1("t1.txt");
    for (const auto& p : t1)
        file1 << "sin " << p.first << " " << p.second << std::endl;
    file1.close();

    std::ofstream file2("t2.txt");
    for (const auto& p : t2)
        file2 << "sqrt " << p.first << " " << p.second << std::endl;
    file2.close();

    std::ofstream file3("t3.txt");
    for (const auto& p : t3)
        file3 << "pow " << p.first << " " << p.second << std::endl;
    file3.close();

    return 0;
}