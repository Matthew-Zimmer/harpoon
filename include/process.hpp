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
        template <typename Type>
        using Extract_Process = Meta::Extract<Type, 0>;

        template <typename Type>
        using Extract_Variable = Meta::Extract<Type, 1>;

        template <typename ... Types>
        class Complex_Item : public Is<Complex_Item<Types...>, Meta::Convert<Meta::For_Each<Meta::Wrap<Types...>, Extract_Variable>, Variables>>
        {
        public:
            using Process_Type = Meta::For_Each<Meta::Wrap<Types...>, Extract_Process>;
            using Data_Type = Meta::For_Each<Meta::Wrap<Types...>, Extract_Variable>;
        };

        template <typename P, typename V>
        class Simple_Item : public Is<Simple_Item<P, V>, Variables<V>>
        {
        public:
            using Process_Type = Meta::Wrap<P>;
            using Data_Type = Meta::Wrap<V>;
        };

        template <typename ... Types>
        using Item = std::conditional_t<(Meta::Is_Container<Types> && ...), Complex_Item<Types...>, Simple_Item<Types...>>;
    }

    namespace Harpoon
    {
        class Base_Process
        {
        protected:
            std::string name;
            static Memory::Block queues;
        public:
            Base_Process(std::string const& name);
            virtual ~Base_Process() = default;
            virtual int Execute(std::vector<std::string> const& args) = 0;
            virtual void Create_Queues() = 0;
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
                using Output_Type = std::decay_t<Meta::Return_Type<Main_Function>>;

                Buffer<Input_Type, Output_Type, 16> buffer { this->queues.Items<Input_Type>(), this->queues.Items<Output_Type>() };

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

            void Create_Queues() final
            {
                using Main_Function = decltype(&Type::Main);
                using Input_Queue_Type = Queue<std::decay_t<Meta::Unwrap<Meta::Args<Main_Function>>>, 16>;
                using Output_Queue_Type = Queue<std::decay_t<Meta::Return_Type<Main_Function>>, 16>;

                if (!this->queues.Items<Input_Queue_Type>().size())
                    this->queues.Add(Input_Queue_Type{});

                if (!this->queues.Items<Output_Queue_Type>().size())
                    this->queues.Add(Output_Queue_Type{});
            }
        };

        template <typename Type>
        typename Process<Type>::Hook Process<Type>::hook;
    }
}