#ifndef HAND_PREREQ
#define HAND_PREREQ
#if defined (WIN32) 
#if defined (_MSC_VER)
#pragma warning(disable: 4251)
#endif
//TODO
//#if defined(HAND_EXPORT)

#define  HAND_EXPORT __declspec(dllexport)
//#else
//#define  HAND_EXPORT __declspec(dllimport)
//#endif
#else
#define HAND_EXPORT
#endif

#endif  // HAND_PREREQ