#include "process.hpp"
#include <reflection/variables.hpp>

namespace MY_PROJECT
{
    using namespace Slate;
    using namespace Harpoon;

    namespace Variable
    {
        using namespace Slate::Variable;
        using Int0 = class : public Base<int>
        {
        public:
            int& Int0()
            {
                return Variable();
            }
            int const& Int0() const
            {
                return Variable();
            }
        };

        using Int1 = class : public Base<int>
        {
        public:
            int& Int1()
            {
                return Variable();
            }
            int const& Int1() const
            {
                return Variable();
            }
        };
    }
    namespace V = Variable;
    

    class Test : public Process<Test>
    {
        int i{ 0 };
    public:
        Test() : Process{ "Test" } 
        {
            this->queues.Items<Item<Test, V::Int0>>() = std::vector<Item<Test, V::Int0>>
            { 
                Item<Test, V::Int0>{ 1 },
                Item<Test, V::Int0>{ 2 },
                Item<Test, V::Int0>{ 3 },
                Item<Test, V::Int0>{ 4 },
                Item<Test, V::Int0>{ 5 },
            };
        }

        Item<Test, V::Int1> Main(Item<Test, V::Int0> const& i)
        {
            return Item<Test, V::Int1>{ i.Int0() * 2 };
        }

        bool Active()
        {
            return i++ < 5;
        }
    };
}