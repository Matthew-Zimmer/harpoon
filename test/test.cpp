#include "process.hpp"
#include <reflection/variables.hpp>

namespace MY_PROJECT
{
    using namespace Slate;
    using namespace Harpoon;

    namespace Variable
    {
        using namespace Slate::Variable;

        template <typename Type>
        class Base2 : public Base<Type>
        {
        public:
            using Base<Type>::Base;
            operator Type&()
            {
                return this->Variable();
            }

            operator Type const& () const
            {
                return this->variable();
            }
        };
        using Int0 = class X1 : public Base2<int>
        {
        public:
            using Base2::Base2;
            int& Int0()
            {
                return this->variable();
            }
            int const& Int0() const
            {
                return this->variable();
            }

            constexpr static char const* Name{"Int0"};
        };

        using Int1 = class X2 : public Base2<int>
        {
        public:
            using Base2::Base2;
            int& Int1()
            {
                return this->variable();
            }
            int const& Int1() const
            {
                return this->variable();
            }
            constexpr static char const* Name{ "Int1" };
        };
    }
    namespace V = Variable;

    class Test1 : public Process<Test1>
    {
        int i{ 0 };
        int j{ 8 };
    public:
        Test1() : Process{ "Test1" } 
        {}

        V::Int0 Main()
        {
            return j++;
        }

        bool Active()
        {
            return i++ < 20000;
        }
    };
    

    class Test2 : public Process<Test2>
    {
        int i{ 0 };
    public:
        Test2() : Process{ "Test2" } 
        {}

        V::Int1 Main(Output<Test1> const& i)
        {
            return i * 2;
        }

        bool Active()
        {
            return i++ < 20000;
        }
    };

    class Test3 : public Process<Test3>
    {
        int i{ 0 };
    public:
        Test3() : Process{ "Test3" } 
        {}

        void Main(Output<Test2> const& i)
        {
            std::cout << i << std::endl;
        }

        bool Active()
        {
            return i++ < 20000;
        }
    };
}