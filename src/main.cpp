#include <algorithm>

#include "process.hpp"

using namespace Harpoon;

int main(int argc, char** argv)
{
    std::vector<std::string> args;
    std::copy(argv, argv + argc, std::back_inserter(args));

    for (auto& p : Processes())
        p->Execute(args);
    
    return 0;//to fix return something based on the processes;
}