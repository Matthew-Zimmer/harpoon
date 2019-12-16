#include "process.hpp"

namespace Slate::Harpoon
{
    Base_Process::Base_Process(std::string const& name) : name{ name } 
    {}

    Memory::Block& Base_Process::Queues()
    {
        static Memory::Block b;
        return b;
    }
}