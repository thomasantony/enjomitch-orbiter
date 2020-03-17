#ifndef OPTIMULTISUBJECT_H
#define OPTIMULTISUBJECT_H

#include <vector>

namespace EnjoLib
{
    class OptiMultiSubject
    {
        public:
            OptiMultiSubject();
            virtual ~OptiMultiSubject();

            //virtual double Get(const std::vector<double> & in) = 0;
            virtual double Get(const double * in, int n) = 0;

            virtual std::vector<double> GetStart() const = 0;
            virtual std::vector<double> GetStep() const = 0;


            struct Bounds
            {
                Bounds(double min, double max) : min(min), max(max) {}
                double min;
                double max;
            };
            virtual std::vector<Bounds> GetBounds() const { return std::vector<Bounds>(); };


        protected:
        private:
    };
}

#endif // OPTIMULTISUBJECT_H
