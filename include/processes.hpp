#pragma once
#include <memory>
#include <vector>

#include "process.hpp"

namespace Harpoon
{
    static std::vector<std::unique_ptr<Process>>& Processes()
    {
        static std::vector<std::unique_ptr<Process>> processes;
        return processes;
    }
} // namespace Harpoon
