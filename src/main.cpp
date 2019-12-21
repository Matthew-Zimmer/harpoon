#include <algorithm>

#include "process.hpp"

using namespace Slate::Harpoon;

int main(int argc, char** argv)
{
    std::vector<std::string> args;
    std::copy(argv, argv + argc, std::back_inserter(args));
    
    std::vector<std::thread> threads;

    for (auto& p : Processes())
        threads.emplace_back(p->Execute(args));

    for (auto& thread : threads)
        if (thread.joinable())
            thread.join();

    return 0;//to fix return something based on the processes;
}