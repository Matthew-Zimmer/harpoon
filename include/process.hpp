#pragma once
#include <memory>
#include <string>
#include <thread>
#include <vector>

#include <iostream>

namespace Meta 
{
    template <typename ... Ts>
    class Wrap {};
}

namespace Imp::Meta
{
    template <typename ... Types_>
    class Unwrap {};

    template <typename Type_>
    class Unwrap<::Meta::Wrap<Type_>> 
    {
    public:
        using Type = Type_;
    };

    template <>
    class Unwrap<::Meta::Wrap<>> 
    {
    public:
        using Type = void;
    };

    template <typename>
    class Function_Types {};

    template <typename Return_Type_, typename Object_Type_, typename ... Args_>
    class Function_Types<Return_Type_(Object_Type_::*)(Args_...)>
    {
    public:
        using Return_Type = Return_Type_;
        using Object_Type = Object_Type_;
        using Args = ::Meta::Wrap<Args_...>;
    };

    template <typename Return_Type_, typename ... Args_>
    class Function_Types<Return_Type_(*)(Args_...)>
    {
    public:
        using Return_Type = Return_Type_;
        using Args = ::Meta::Wrap<Args_...>;
    };
}

namespace Meta
{
    template <typename Type>
    using Unwrap = typename Imp::Meta::Unwrap<Type>::Type;

    template <typename Type>
    using Return_Type = typename Imp::Meta::Function_Types<Type>::Return_Type;

    template <typename Type>
    using Object_Type = typename Imp::Meta::Function_Types<Type>::Object_Type;

    template <typename Type>
    using Args = typename Imp::Meta::Function_Types<Type>::Args;

    template <typename Convert, typename Type>
    Convert& Cast(Type& t)
    {
        return static_cast<Convert&>(t);
    }
    
    template <typename Convert, typename ... Steps, typename Type>
    auto Cast(Type& t) -> std::enable_if_t<sizeof...(Steps) != 0, Convert&>
    {
        return static_cast<Convert&>(Cast<Steps...>(t));
    }

    template <typename Convert, typename Type>
    Convert const& Cast(Type const& t)
    {
        return static_cast<Convert const&>(t);
    }

    template <typename Convert, typename ... Steps, typename Type>
    auto Cast(Type const& t) -> std::enable_if_t<sizeof...(Steps) != 0, Convert const&>
    {
        return static_cast<Convert const&>(Cast<Steps...>(t));
    }
}

namespace Harpoon
{
    class Base_Process
    {
    protected:
        std::string name;
    public:
        Base_Process(std::string const& name);
        virtual ~Base_Process() = default;
        virtual int Execute(std::vector<std::string> const& args) = 0;
    };

    inline std::vector<std::unique_ptr<Base_Process>>& Processes()
    {
        static std::vector<std::unique_ptr<Base_Process>> processes;
        return processes;
    }

    template <typename Input, typename Output>
    class Buffer
    {};

    template <typename Output>
    class Buffer<void, Output>
    {
    public:
        std::vector<Output> data;
    };

    template <typename Input>
    class Buffer<Input, void>
    {};

	template <typename Type>
	class Process : public Base_Process
	{
		class Hook
		{
		public:
			Hook()
			{
				Processes().push_back(std::make_unique<Type>());
			}
			void operator()(){}
		};
        std::vector<std::thread> threads;
	public:
		static Hook hook;
		
        Process(std::string const& name) : Base_Process{ name }
		{
			hook();
		}

        int Execute(std::vector<std::string> const& args) final
        {
            using Main_Function = decltype(&Type::Main);
            using Input_Type = Meta::Unwrap<Meta::Args<Main_Function>>;
            using Ouput_Type = Meta::Return_Type<Main_Function>;

            Buffer<Input_Type, Ouput_Type> buffer { };

            threads.push_back(std::thread{ [&]()
            {
                int i = 10;
                while (true)
                {
                    buffer.data.push_back(Meta::Cast<Type>(*this).Main());
                    if (--i == 0)
                        return;
                }
            } });

            for(auto& t : threads)
                if (t.joinable())
                    t.join();

            for (auto& x : buffer.data)
                std::cout << x << std::endl;

            return 0;
        }
	};

	template <typename Type>
	typename Process<Type>::Hook Process<Type>::hook;
}