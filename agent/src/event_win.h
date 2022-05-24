// Copyright 2022 The Chromium Authors.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CONTENT_ANALYSIS_AGENT_SRC_EVENT_WIN_H_
#define CONTENT_ANALYSIS_AGENT_SRC_EVENT_WIN_H_

#include <windows.h>

#include "event_base.h"

namespace content_analysis {
namespace sdk {

// ContentAnalysisEvent implementaton for Windows.
class ContentAnalysisEventWin : public ContentAnalysisEventBase {
 public:
  ContentAnalysisEventWin(HANDLE handle, ContentAnalysisRequest request);
  ~ContentAnalysisEventWin() override;

  // Initialize the event.  This involves reading the request from Google
  // Chrome and making sure it is well formed.
  DWORD Init();

  // ContentAnalysisEvent:
  int Close() override;
  int Send() override;

 private:
  void Shutdown();

  HANDLE hPipe_ = INVALID_HANDLE_VALUE;
};

}  // namespace sdk
}  // namespace content_analysis

#endif  // CONTENT_ANALYSIS_AGENT_SRC_EVENT_WIN_H_