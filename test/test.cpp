#include "process.hpp"

class Test : public Harpoon::Process<Test>
{
    int i{ 0 };
public:
    Test() : Process{ "Test" } {}

    // int Main()
    // {
    //     return i++ * 2;
    // }

    // void Main(std::string const& s)
    // {
    //     std::cout << std::stoi(s) * 2 << std::endl;
    // }

    int Main(std::string const& s)
    {
        return std::stoi(s) * 2;
    }

    bool Active()
    {
        return i++ < 5;
    }
};

