#pragma once
#include <array>
#include <condition_variable>
#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

#include <iostream>

#include <memory/block.hpp>

namespace Slate
{
    namespace Harpoon
    {
        template <typename Process, typename Type, typename=void>
        class Item : public Type
        {
        public:
            using Process_Type = Process;
            using Type::Type;
        };

        template <typename Process, typename Type>
        class Item<Process, Type, std::enable_if_t<std::is_fundamental_v<Type>>>
        {
            Type t;
        public:
            using Process_Type = Process;
            template <typename Type_>
            Object(Type_&& t) : t{ static_cast<Type>(std::forward<Type_>(t)) }
            {}
            operator Type() 
            {
                return t;
            }
            operator O() const 
            {
                return o; 
            }
        };
    }

    namespace Imp::Meta
    {
        template <typename Type>
        class Item_Types {};

        template <typename Process_, typename Type_>
        class Item_Types<Slate::Harpoon::Item<Process_, Type_>>
        {
        public:
            using Process = Process_;
            using Type = Type_;
        };
    }

    namespace Meta
    {
        template <typename Type>
        using Item_Process_Type = typename Imp::Meta::Item_Types<Type>::Process;

        template <typename Type>
        using Item_Type = typename Imp::Meta::Item_Types<Type>::Type;
    }

    namespace Harpoon
    {
        class Base_Process
        {
        protected:
            std::string name;
            static Memory::Block buffers;
        public:
            Base_Process(std::string const& name);
            virtual ~Base_Process() = default;
            virtual int Execute(std::vector<std::string> const& args) = 0;
            virtual void Create_Buffers() = 0;
        };

        inline std::vector<std::unique_ptr<Base_Process>>& Processes()
        {
            static std::vector<std::unique_ptr<Base_Process>> processes;
            return processes;
        }

        template <typename Type, std::size_t Size_>
        class Queue
        {
            std::array<Type, Size_> data;
            using Iterator = typename std::array<Type, Size_>::iterator;
            Iterator first;
            Iterator last;

            std::mutex push_mutex;
            std::condition_variable push_cv;

            std::mutex pop_mutex;
            std::condition_variable pop_cv;
        public:
            Queue() : data{}, first{ data.begin() }, last{ data.begin() }
            {}

            template <typename T>
            void Push(T&& t)
            {
                std::unique_lock lock{ push_mutex };
                push_cv.wait(lock, [this](){ return Size() < Size_; });

                if (last == data.end())
                    last = data.begin();
                *last++ = std::forward<T>(t);

                lock.unlock();
                if (Size() == 1)
                    pop_cv.notify_one();
                else
                    pop_cv.notify_all();
            }

            Type& Pop()
            {
                std::unique_lock lock{ pop_mutex };
                pop_cv.wait(lock, [this](){ return Size() > 0; });

                if (first == data.end())
                    first = data.begin();
                auto& elem = *first++;

                lock.unlock();
                if (Size() == 1)
                    push_cv.notify_one();
                else
                    push_cv.notify_all();
                
                return elem;
            }

            std::size_t Size()
            {
                std::size_t dist = std::distance(first, last);
                return first <= last ? dist : Size_ + dist;
            }
        };

        template <typename Input, typename Output, std::size_t Size>
        class Buffer
        {
            Queue<Input, Size>& input;
            Queue<Output, Size>& output;
        public:
            Buffer(Queue<Input, Size>& input, Queue<Output, Size>& output) : input{ input }, output{ output }
            {}

            template <typename Functor>
            void operator()(Functor&& f)
            {
                output.Push(f(input.Pop()));
            }
        };

        template <typename Output, std::size_t Size>
        class Buffer<void, Output, Size>
        {
            Queue<Output, Size>& output;
        public:
            Buffer(Queue<Output, Size>& output) : output{ output }
            {}

            template <typename Functor>
            void operator()(Functor&& f)
            {
                output.Push(f());
            }
        };

        template <typename Input, std::size_t Size>
        class Buffer<Input, void, Size>
        {
            Queue<Input, Size>& input;
        public:
            Buffer(Queue<Input, Size>& input) : input{ input }
            {}

            template <typename Functor>
            void operator()(Functor&& f)
            {
                f(input.Pop());
            }
        };

       

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

            bool Active() const 
            {
                return true;
            }

            int Execute(std::vector<std::string> const& args) final
            {
                using Main_Function = decltype(&Type::Main);
                using Input_Type = std::decay_t<Meta::Unwrap<Meta::Args<Main_Function>>>;
                using Ouput_Type = std::decay_t<Meta::Return_Type<Main_Function>>;

                Queue<std::string, 16> i;
                i.Push("1");
                i.Push("2");
                i.Push("3");
                i.Push("4");
                i.Push("5");
                Queue<int, 16> o;

                Buffer<Input_Type, Ouput_Type, 16> buffer { i, o };

                threads.push_back(std::thread{ [&]()
                {
                    while (Meta::Cast<Type>(*this).Active())
                        buffer([&](auto& ... x){ return Meta::Cast<Type>(*this).Main(x...); });
                } });

                for(auto& t : threads)
                    if (t.joinable())
                        t.join();

                for (int i = 0; i < 5; i++)
                    std::cout << o.Pop() << std::endl;

                return 0;
            }

            void Create_Buffers() final
            {
                using Main_Function = decltype(&Type::Main);
                using Input_Type = Meta::Unwrap<Meta::Args<Main_Function>>;
                using Ouput_Type = Meta::Return_Type<Main_Function>;

                if constexpr (Meta::Is_Container<Input_Type>)
                {
                    
                }
                else if ()
                {

                }


                this->buffers.Add()
            }
        };

        template <typename Type>
        typename Process<Type>::Hook Process<Type>::hook;
    }
}