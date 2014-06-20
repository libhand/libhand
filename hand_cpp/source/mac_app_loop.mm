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
// The idea of a NSTimer controlled Cocoa application stems from
// masterfalcon's (David Rogers') implementation of SampleBrowser for
// OGRE (Object-oriented Graphics Rendering Engine).
//
// The art of implementing a bundle-less, nib-less, Info.plist-less
// Objective-C object is owed to Matt Gallagher, and an appropriate
// zlib license copyright notice is included with the
// MacAppLoopPrivate::Setup code
//
// This file must be compiled with Objective-C++ aware compiler,
// either clang or Apple's version of gcc
//
// The MacAppLoop header file does not need Objective-C++ and can be
// included by C++ programs.
// The linkage parameters need to be "-framework Cocoa"

# include "mac_app_loop.h"

# include <iostream>
# include <exception>
# import <Cocoa/Cocoa.h>

// The private implementation of the Mac Application Loop
// The MacAppLoop class forwards all of its calls to MacAppLoopPrivate
// The MacAppLoop methods are further down in the file
class MacAppLoopPrivate {
public:
  MacAppLoopPrivate(MacAppLoop &app_loop);
  ~MacAppLoopPrivate();

  void SetRefreshHz(double refresh_hz) { refresh_hz_ = refresh_hz; }
  inline double refresh_hz() { return refresh_hz_; }

  void Setup(double refresh_hz);

  void RequestShutdown() { exit_called_ = true; }
  inline bool wants_shutdown() { return exit_called_; }

  void RenderFrame(double time_since_last_frame);

  void Run();

private:
  // Link to the public instance of the class
  MacAppLoop &app_loop_;

  // Internal variables
  double refresh_hz_;  // Refresh rate in Hz
  bool is_setup_;      // true if Setup() called
  bool exit_called_;   // true if RequestShutdown() called

  // Link to the Objective-C Cocoa object
  id app_delegate_;

  // The Cocoa envrionment autorelease pool
  NSAutoreleasePool *pool_;
};

//--------------------------------------------------------------------------
//               The implementation of MacAppLoop
//           (simply forwards to MacAppLoopPrivate)

MacAppLoop::MacAppLoop() 
: private_(new MacAppLoopPrivate(*this)) {
}

MacAppLoop::~MacAppLoop() { delete private_; }

void MacAppLoop::Setup(double refresh_hz) { private_->Setup(refresh_hz); }
void MacAppLoop::RequestShutdown() { private_->RequestShutdown(); }
void MacAppLoop::Run() { private_->Run(); }

//--------------------------------------------------------------------------
//           The MacAppDelegate Objective-C++ code region
//

// Supress the warnings for not implementing buttonPressed
@implementation NSApplication (_suppressUnimplementedActionWarning)
- (void) buttonPressed:(id)sender { /* Do nothing */ }
@end

// MacAppDelegate is the Objective-C++ Cocoa application delegate based
// in essential design on the SampleBrowser_OSX.h code
@interface MacAppDelegate : NSObject <NSApplicationDelegate> {
  //  An instance of the NSTimer class that will
  //  fire at the desired refresh rate.
  NSTimer *timer;

  // A pointer to a private implementation of MacAppLoop
  // (the MacAppLoopPrivate code is  further below in this file)
  MacAppLoopPrivate *app_loop_private;
}

// MacAppDelegate methods:
//  initWithPrivateLoop is used as a "constructor"
- (MacAppDelegate *)initWithPrivateLoop:(MacAppLoopPrivate *)loop;

//  renderFrame is periodically called by the NSTimer instance 'timer'
- (void)renderFrame:(id)sender;

//  appShutdown is invoked when the MacAppLoop requests a shutdown
- (void)appShutdown;
@end

// Implementation of the MacAppDelegate methods follow
@implementation MacAppDelegate
// The MacAppDelegate "constructor" accepts a pointer to an instance
// of the MacAppLoopPrivate, an instance of C++ class which is a
// private implementation
- (MacAppDelegate *)initWithPrivateLoop:(MacAppLoopPrivate *)loop {
  self = [super init];

  if (self) app_loop_private = loop;

  return self;
}

// The MacOS X Cocoa subsystem will call this method as soon as the
// application is ready to start executing UI code.
- (void)applicationDidFinishLaunching:(NSNotification *)application {
  double refresh_hz = app_loop_private->refresh_hz();

  // If we have a sane refresh rate specified, we create an instance
  // of the NSTimer class, specifying [self renderFrame] as the target.
  // The timer invokes renderFrame at the frequency of refresh_hz per second
  if (refresh_hz > 0.) {
    timer = [NSTimer
             scheduledTimerWithTimeInterval:((NSTimeInterval)
                                             (1.0 / refresh_hz))
                                     target:self
                                   selector:@selector(renderFrame:)
                                   userInfo:nil
                                    repeats:YES];
  }
}

// The renderframe method is called periodically by the timer instance
- (void)renderFrame:(id)sender {
  // We check whether the Mac application loop requested a shutdown
  if (app_loop_private->wants_shutdown()) {
    [self appShutdown];
    return;
  }

  // No shutdown was requested so render the frame, passing the time
  // elapsed since the last frame
  app_loop_private->RenderFrame([timer timeInterval]);
}

//  appShutdown is invoked when the MacAppLoop requests a shutdown
- (void)appShutdown {
  // Destroy the timer object
  [timer invalidate];
  timer = nil;
  
  // Do [NSapp stop] as soon as possible in the run loop
  // (does not execute immediately)
  // This will exit the run loop and return control to the caller of [NSApp run]
  [NSApp performSelector:@selector(stop:)
              withObject:nil 
              afterDelay:0.0];
}

- (void)dealloc {
  if(timer) {
    [timer invalidate];
    timer = nil;
  }

  [super dealloc];
}
@end

//--------------------------------------------------------------------------
//                MacAppLoopPrivate implementation

MacAppLoopPrivate::MacAppLoopPrivate(MacAppLoop &app_loop)
: app_loop_(app_loop),
  refresh_hz_(0),
  is_setup_(false),
  exit_called_(false),
  pool_(NULL) {
}

MacAppLoopPrivate::~MacAppLoopPrivate() {
  // Destroy the autorelease pool
  if (is_setup_) {
    [pool_ release];
  }
}

void MacAppLoopPrivate::Setup(double refresh_hz) {
  if (is_setup_ || exit_called_) return;

  SetRefreshHz(refresh_hz);

  // ----------------------------------------------------------------------
  // The code below uses an altered version of Matt Galagher's example:
  // http://cocoawithlove.com/2010/09/minimalist-cocoa-programming.html
  //
  // Copyright (c) 2009-2011 Matt Gallagher. All rights reserved.
  //
  // This software is provided 'as-is', without any express or implied
  // warranty. In no event will the authors be held liable for any damages
  // arising from the use of this software. Permission is granted to anyone
  // to use this software for any purpose, including commercial
  // applications, and to alter it and redistribute it freely, subject to
  // the following restrictions: 
  //
  // 1. The origin of this software must not be misrepresented; you must
  //    not claim that you wrote the original software. If you use this
  //    software in a product, an acknowledgment in the product
  //    documentation would be appreciated but is not required.
  //
  // 2. Altered source versions must be plainly marked as such, and must
  //    not be misrepresented as being the original software.
  //
  // 3. This notice may not be removed or altered from any source
  //    distribution.

  pool_ = [NSAutoreleasePool new];
  [NSApplication sharedApplication];
  [NSApp setActivationPolicy:NSApplicationActivationPolicyRegular];

  id menubar = [[NSMenu new] autorelease];
  id appMenuItem = [[NSMenuItem new] autorelease];
  [menubar addItem:appMenuItem];
  [NSApp setMainMenu:menubar];
  id appMenu = [[NSMenu new] autorelease];
  id appName = [[NSProcessInfo processInfo] processName];
  id quitTitle = [@"Quit " stringByAppendingString:appName];
  id quitMenuItem =
    [[[NSMenuItem alloc]
      initWithTitle:quitTitle
      action:@selector(stop:)
      keyEquivalent:@"q"] autorelease];

  [appMenu addItem:quitMenuItem];
  [appMenuItem setSubmenu:appMenu];
  // ----------------------------------------------------------------------
  
  app_delegate_ = [[MacAppDelegate alloc]
                   initWithPrivateLoop:this];

  is_setup_ = true;
}

void MacAppLoopPrivate::Run() {
  if (!is_setup_ || exit_called_) return;

  [[NSApplication sharedApplication] setDelegate:app_delegate_];
  [NSApp activateIgnoringOtherApps:YES];
  [NSApp run];

  // Set exit_called_, so that Run cannot be re-entered
  exit_called_ = true;
}

void MacAppLoopPrivate::RenderFrame(double time_since_last_frame) {
  try {
    app_loop_.RenderFrame(time_since_last_frame);
  } catch (const std::exception& e) {
    std::cerr << "MacAppLoop::RenderFrame: "
              << "Caught an exception: "
              << e.what() << std::endl
              << "MacAppLoop::RenderFrame: shutting down." << std::endl;
    RequestShutdown();
  } catch (...) {
    std::cerr << "MacAppLoop::RenderFrame: "
              << "Exception caught, shutting down." << std::endl;
    RequestShutdown();
  }
}

//--------------------------------------------------------------------------
//                MacMemoryPool implementation

MacMemoryPool::MacMemoryPool() {
  data_ = (void *) [NSAutoreleasePool new];
}

MacMemoryPool::~MacMemoryPool() {
  [((NSAutoreleasePool *) data_) release];
}
