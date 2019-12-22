#include "process.hpp"

namespace Slate::Harpoon
{
    Base_Process::Base_Process(std::string const& name) : name{ name } 
    {}

    Memory::Block& Buffer<void, void>::Queues()
    {
        static Memory::Block queues;
        return queues;
    };
}