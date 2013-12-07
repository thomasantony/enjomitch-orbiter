#ifndef BINSEARCHOPTISUBJECT_H
#define BINSEARCHOPTISUBJECT_H

namespace EnjoLib
{
    class BinSearchOptiSubject
    {
        public:
            BinSearchOptiSubject();
            virtual ~BinSearchOptiSubject();

            /// Should return the function's value for a given argument.
            virtual double UpdateGetValue( double arg ) = 0;

        protected:
        private:
    };
}

#endif // BINSEARCHOPTISUBJECT_H