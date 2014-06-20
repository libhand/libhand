// Copyright (c) 2011 Marin Saric
// Portions Copyright (c) 2009-2011 Matt Gallagher
// 
// This software is provided 'as-is', without any express or implied
// warranty. In no event will the authors be held liable for any damages
// arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented; you must not
// claim that you wrote the original software. If you use this software in
// a product, an acknowledgment in the product documentation would be
// appreciated but is not required.
//
// 2. Altered source versions must be plainly marked as such, and must not
// be misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any source
// distribution.
// 
// MacAppLoop
//
// The MacAppLoop class abstracts away the MacOS X Cocoa internals
//
// This class header has a pure C++ interface and does not depend on you
// doing an #include of any other files with your code.
//
// You also do NOT need to setup an Application Bundle, Info.plist, nib
// files or do any of the other very MacOS X specific steps.
//
// To compile mac_app_loop.mm, you need either Apple's version of gcc or
// clang. Remember to link with "-framework Cocoa" flag.
//
// This abstraction isolates you from needing to know about how the
// Cocoa stuff works.
//
// If you just want to "play nice" when calling Cocoa-based routines,
// it is probably enough to declare a MacMemoryPool before calling
// such code. Creating a MacMemoryPool is comparable to the cost of
// allocating a tiny block of memory. Thus, you can use it in
// per-frame loops, etc.
//
// --------------------------------------------------------------------------
// Code example for MacAppLoop:
// The user of the "mac_app_loop.h" only needs to do this:
//
// # include "mac_app_loop.h"
//
// class MyAppLoop : public MacAppLoop {
//   void RenderFrame(double time_since_last_frame) {
//      do something...
//   }
// };
//
// and then
//
// int main(int argc, char **argv) {
//   MyAppLoop app_loop;
//
//   // make sure to call Setup as early as possible in your code
//   app_loop.Setup();
//
//   app_loop.Run();
//   return 0;
// }
// --------------------------------------------------------------------------
// Code example for MacMemoryPool:
//
// # include "mac_app_loop.h"
//
// void MyFunctionThatCallsCocoaSystems() {
//    MacMemoryPool memory_pool;
//
//    SomeCocoaBasedRoutine();
// }

#ifndef MAC_APP_LOOP_H
#define MAC_APP_LOOP_H

// We hide the actual implementation in this class
class MacAppLoopPrivate;

class MacAppLoop {
 public:
  MacAppLoop();
  virtual ~MacAppLoop();

  // Call this before Run().
  // refresh_hz - how many times per second do is RenderFrame called
  void Setup(double refresh_hz = 60.0);

  // Call this to get the App to Shutdown
  void RequestShutdown();

  // Override this method to get something to happen
  virtual void RenderFrame(double time_since_last_frame) {}

  // Call this to enter the application loop
  void Run();

 private:
  MacAppLoopPrivate *private_;

  // Disallow copying of this object
  MacAppLoop(const MacAppLoop &rhs);
  MacAppLoop& operator= (const MacAppLoop &rhs);
};

// Declare a MacMemoryPool in places that call Cocoa based code.
// You will need at least one MacMemoryPool per application
class MacMemoryPool {
public:
  MacMemoryPool();
  ~MacMemoryPool();

private:
  void *data_;

  // Disallow copying of this object
  MacMemoryPool(const MacAppLoop &rhs);
  MacMemoryPool& operator= (const MacAppLoop &rhs);
};

#endif  // MAC_APP_LOOP_H
