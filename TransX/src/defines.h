#ifndef DEFINES_H_INCLUDED
#define DEFINES_H_INCLUDED

//#define FIRST_INSTANCE
#define SECOND_INSTANCE_RED

#ifdef FIRST_INSTANCE
    #undef SECOND_INSTANCE_RED
#endif

#ifdef SECOND_INSTANCE_RED
    #undef FIRST_INSTANCE
#endif

#endif // DEFINES_H_INCLUDED
