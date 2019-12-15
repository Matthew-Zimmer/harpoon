#include "process.hpp"

using namespace Slate;

class Test : public Harpoon::Process<Test>
{
    int i{ 0 };
public:
    using Input = 

    Test() : Process{ "Test" } {}

    int Main(std::string const& s)
    {
        return std::stoi(s) * 2;
    }

    bool Active()
    {
        return i++ < 5;
    }
};

/*
#include <type_traits>
#include <utility>

template <typename T, typename O, typename=void>
class Object : public O
{
public:
    using Process_Type = T;
    using O::O;
};

template <typename T, typename O>
class Object<T, O, std::enable_if_t<std::is_fundamental_v<O>>>
{
    O o;
public:
    using Process_Type = T;
    template <typename Type>
    Object(Type&& t) : o{ static_cast<O>(std::forward<Type>(t)) }
    {}
    operator O() { return o; }
    operator O() const { return o; }
};

template <typename ... Types>
class Wrap {};

#include <tuple>

namespace Variable
{
    using Amount = class X
    {
        double data;
    public:
        template <typename Type>
        X(Type&& x) : data{ x }
        {}

        double& Amount()
        {
            return data;
        }

        double const& Amount() const
        {
            return data;
        }
    };

    using Rate = class XX
    {
        int data;
    public:
        template <typename Type>
        XX(Type&& x) : data{ x }
        {}

        int& Rate()
        {
            return data;
        }
        
        int const& Rate() const
        {
            return data;
        }
    };
}
namespace V = Variable;

template <typename ... T, typename ... O>
class Object<Wrap<T...>, Wrap<O...>, void> : public O...
{
public:
    using Process_Type = Wrap<T...>;
};

class P0 {};
class P1 {};
class P2 {};

class A 
{
};
class B
{
public:
    explicit B(A a){}
};

auto f(Object<Wrap<P0, V::Amount>, Wrap<P1, V::Rate>> const& o) -> Object<P2, int>
{
    return Object<P2, int>{ o.Amount() * o.Rate() };
}


int main()
{

}
*/