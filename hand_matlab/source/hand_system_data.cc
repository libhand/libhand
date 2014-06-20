// Copyright (c) 2011, Marin Saric <marin.saric@gmail.com>
// All rights reserved.
//
// This file is a part of LibHand. LibHand is open-source software. You can
// redistribute it and/or modify it under the terms of the LibHand
// license. The LibHand license is the BSD license with an added clause that
// requires academic citation. You should have received a copy of the
// LibHand license (the license.txt file) along with LibHand. If not, see
// <http://www.libhand.org/>

// HandSystemData

# include "hand_system_data.h"

# include "mex.h"
# include "marin_mexutil.h"

namespace libhand {

static HandSystemData *static_hand_system_data = NULL;

HandSystemData *HandSystemData::Init() {
  HandSystemData *ret = GetHandleFromMatlab();

  if (!ret) {
    if (static_hand_system_data) {
      SetHandleInMatlab(static_hand_system_data);
      return GetHandleFromMatlab();
    }

    InitInternal();
    if (!mexIsLocked()) mexLock();

    return GetHandleFromMatlab();
  }

  return ret;
}

HandSystemData *HandSystemData::GetHandleFromMatlab() {
  return (HandSystemData *) LoadMatlabPtr("hand_render_system_data");
}

void HandSystemData::SetHandleInMatlab(HandSystemData *sys_data) {
  StoreMatlabPtr("hand_render_system_data", (void *) sys_data);
}

void HandSystemData::InitInternal() {
  HandSystemData *hsys = NULL;

  try {
    hsys = new HandSystemData;

    hsys->hand_renderer().Setup();
  } catch (const std::exception &e) {
    delete hsys;

    throw MatlabErrorException("Hand system setup failed, "
                               "exception caught: %s", e.what());
  } catch (...) {
    delete hsys;

    throw MatlabErrorException("Hand system setup failed, "
                               "exception caught!");
  }

  static_hand_system_data = hsys;
  SetHandleInMatlab(static_hand_system_data);
  mexAtExit(&HandSystemData::UnloadData);
}

HandSystemData::HandSystemData() : has_scene_(false) {
}

void HandSystemData::UnloadData() {
  SetHandleInMatlab(NULL);

  if (static_hand_system_data) {
    delete static_hand_system_data;
    static_hand_system_data = NULL;
  }

  if (mexIsLocked()) mexUnlock();
}

}  // namespace libhand
