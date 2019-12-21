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
        template <typename Process, typename Variable>
        class Item : public Variable
        {
        public:
            using Process_Type = Process;
            using Variable_Type = Variable;

            using Variable::Variable;

            Item(Variable const& v) : Variable{ v }
            {}
            Item(Variable&& v) : Variable{ v }
            {}
        };

        template <typename Process>
        class Item<Process, void>
        {
        public:
            using Process_Type = Process;
            using Variable_Type = void;
        };

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

        template <typename Type>
        std::mutex push_mutex;

        template <typename Type>
        std::mutex pop_mutex;

        template <typename Type>
        std::condition_variable push_cv;

        template <typename Type>
        std::condition_variable pop_cv;

        

        template <typename Type, std::size_t Size_>
        class Queue
        {
            std::array<Type, Size_> data;
            using Iterator = typename std::array<Type, Size_>::iterator;
            Iterator first;
            Iterator last;
        public:

            using Element_Type = Type;

            Queue() : data{}, first{ data.begin() }, last{ data.begin() }
            {}

            template <typename T>
            void Push(T&& t)
            {
                std::unique_lock lock{ push_mutex<Type> };
                push_cv<Type>.wait(lock, [this](){ return Size() < Size_; });

                if (last == data.end())
                    last = data.begin();
                *last++ = std::forward<T>(t);

                lock.unlock();
                if (Size() == 1)
                    pop_cv<Type>.notify_one();
                else
                    pop_cv<Type>.notify_all();
            }

            Type Pop()
            {
                std::unique_lock lock{ pop_mutex<Type> };
                pop_cv<Type>.wait(lock, [this](){ return Size() > 0; });

                if (first == data.end())
                    first = data.begin();
                auto& elem = *first++;

                lock.unlock();
                if (Size() == 1)
                    push_cv<Type>.notify_one();
                else
                    push_cv<Type>.notify_all();
                
                return elem;
            }

            std::size_t Size()
            {
                std::size_t dist = std::distance(first, last);
                return first <= last ? dist : Size_ + dist;
            }
        };

        template <typename Type>
        constexpr bool Has_Output_Queue = !std::is_same_v<typename Type::Element_Type::Variable_Type, void>;

        template <typename Input = void, typename Output = void>
        class Buffer
        {};

        template <>
        class Buffer<void, void>
        {
            static Memory::Block queues;
        protected:
            template <typename Type>
            Type& Queue()
            {
                return queues.Items<Type>().front();
            }
            template <typename Type>
            void Create_Queue_If_Needed()
            {
                if (!queues.Items<Type>().size())
                    queues.Items<Type>().emplace_back();
            }

            
        };

        template <typename ... Inputs, typename Output>
        class Buffer<Meta::Wrap<Inputs...>, Output> : public Buffer<>
        {
        public:
            Buffer()
            {
                if constexpr(Has_Output_Queue<Output>)
                    Create_Queue_If_Needed<Output>();
                (Create_Queue_If_Needed<Inputs>(), ...);
            }
            template <typename Functor>
            void operator()(Functor&& f)
            {
                if constexpr(Has_Output_Queue<Output>)
                    Queue<Output>().Push(f(Queue<Inputs>().Pop()...));
                else
                    f(Queue<Inputs>().Pop()...);
            }
        };

        template <typename Type>
        using Output = Item<Type, Meta::Return_Type<decltype(&Type::Main)>>;

        template <std::size_t Size>
        class Input
        {
        public:
            template <typename T>
            class Builder
            {
            public:
                using Type = Meta::Wrap<Queue<std::decay_t<T>, Size>>;
            };
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
            static Hook hook;           
        public:    
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
                
                using Input_Type =  Meta::For_Each<Meta::Args<Main_Function>, typename Input<32>::Builder>;
                using Output_Type = Queue<Output<Type>, 32>;

                Buffer<Input_Type, Output_Type> buffer;

                threads.push_back(std::thread{ [&]()
                {
                    while (Meta::Cast<Type>(*this).Active())
                        buffer([&](auto&& ... x){ return Meta::Cast<Type>(*this).Main(x...); });
                } });

                for(auto& t : threads)
                    if (t.joinable())
                        t.join();

                return 0;
            }
        };

        template <typename Type>
        typename Process<Type>::Hook Process<Type>::hook;
    }
}