#include "process.hpp"

class Test : public Harpoon::Process<Test>
{
    int i{0};
public:
    Test() : Process{ "Test" } {}

    int Main()
    {
        return i++;
    }
};

