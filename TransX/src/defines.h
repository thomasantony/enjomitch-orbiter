#ifndef DEFINES_H_INCLUDED
#define DEFINES_H_INCLUDED

//#define FIRST_INSTANCE
//#define SECOND_INSTANCE_RED // Uncomment to create red themed TransX (under the same file name)
// Normally the above are defined in project settings

#ifdef FIRST_INSTANCE
    #undef SECOND_INSTANCE_RED
#endif

#ifdef SECOND_INSTANCE_RED
    #undef FIRST_INSTANCE
#endif

#endif // DEFINES_H_INCLUDED
