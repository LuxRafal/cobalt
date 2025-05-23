// Copyright 2012 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ui/base/win/scoped_ole_initializer.h"

#include <ole2.h>

#include <ostream>

#include "base/check_op.h"

namespace ui {

ScopedOleInitializer::ScopedOleInitializer() : hr_(S_FALSE) {
  // TODO: GOOGAMCONS-164: Impelement
}

ScopedOleInitializer::~ScopedOleInitializer() {
  // TODO: GOOGAMCONS-164: Impelement
}

}  // namespace ui
