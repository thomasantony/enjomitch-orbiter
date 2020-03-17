#ifndef LOOPTHREADEDTPL_HPP
#define LOOPTHREADEDTPL_HPP

#include "../Util/LoopThreaded.hpp"

#include <future>
#include <vector>
#include <functional>

namespace EnjoLib
{
/**
Statically maps a set of conversions to a given core.
Use only when the processing times of the tasks are the same, or risk thread starvation.
*/
template <class TIn, class TOut>
class LoopThreadedTpl
{
    public:
        LoopThreadedTpl(int sizeInput,
                        std::function<TOut(const TIn&)> funcConv,
                        int numCores = std::thread::hardware_concurrency())
        : m_loopThreaded(sizeInput, numCores)
        , m_datasets(numCores)
        , m_funcConv(funcConv)
        {
            const int sizePerCore = sizeInput / numCores;
            for (auto & dataset : m_datasets)
            {
                dataset.reserve(sizePerCore);
            }
        }
        virtual ~LoopThreadedTpl(){}

        void AddInputVector(const std::vector<TIn> & vinput)
        {
            for (int i = 0; i < int(vinput.size()); ++i) {
                AddInput(i, vinput[i]);
            }
        }

        std::vector<TOut> AddInputVectorGetOutputVector(const std::vector<TIn> & vinput)
        {
            AddInputVector(vinput);
            return GetOutputVector();
        }

        void AddInput(int iter, const TIn & input)
        {
            const int dataSetIdx = m_loopThreaded.GetIdxOfDatasetForIteration(iter);
            m_datasets.at(dataSetIdx).push_back(&input);
        }

        std::vector<TOut> GetOutputVector()
        {
            std::vector<TOut> output;
            std::vector<std::future<std::vector<TOut>>> futures;
            for (const std::vector<const TIn *> data : m_datasets)
                futures.push_back(std::async (FunVec, data, m_funcConv));

            for(auto &e : futures)
            {
                for (const TOut & tick : e.get())
                    output.push_back(tick);
            }
            return output;
        }

    protected:

    private:
        static std::vector<TOut> FunVec(const std::vector<const TIn *> & sss, std::function<TOut(const TIn&)> func)
        {
            std::vector<TOut> ticks;
            for (const TIn * s : sss)
                ticks.push_back(func(*s));
            return ticks;
        }

        LoopThreaded m_loopThreaded;
        std::vector<std::vector<const TIn *>> m_datasets; // Pointer uses a lot less memory for complex input objects
        std::function<TOut(const TIn&)> m_funcConv;
};

    /**
    Global function version, that infers the template parameters from the function parameter.
    */
    template <class TIn, class TOut>
    std::vector<TOut> ConvertVectorThreaded(const std::vector<TIn> & vinput,
                                            std::function<TOut(const TIn&)> funcConv,
                                            int numCores = std::thread::hardware_concurrency())
    {
        LoopThreadedTpl<TIn, TOut> loop(vinput.size(), funcConv, numCores);
        return loop.AddInputVectorGetOutputVector(vinput);
    }
}
#endif // LOOPTHREADEDTPL_HPP
