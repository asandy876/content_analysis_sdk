// Copyright 2022 The Chromium Authors.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <utility>

#include "event_win.h"

namespace content_analysis {
namespace sdk {

ContentAnalysisEventWin::ContentAnalysisEventWin(HANDLE handle,
                                                 ContentAnalysisRequest req)
    : hPipe_(handle) {
  // TODO(rogerta): do some basic validation of the request.
  *request() = std::move(req);
}

ContentAnalysisEventWin::~ContentAnalysisEventWin() {
  Shutdown();
}

DWORD ContentAnalysisEventWin::Init() {
  // Prepare the response so that ALLOW verdicts are the default().
  UpdateResponse(*response(),
      request()->tags_size() > 0 ? request()->tags(0) : std::string(),
      ContentAnalysisResponse::Result::SUCCESS);
  return ERROR_SUCCESS;
}

int ContentAnalysisEventWin::Close() {
  Shutdown();
  return ContentAnalysisEventBase::Close();
}

int ContentAnalysisEventWin::Send() {
  if (!WriteMessageToPipe(hPipe_, response()->SerializeAsString()))
    return -1;

  return 0;
}

void ContentAnalysisEventWin::Shutdown() {
  if (hPipe_ != INVALID_HANDLE_VALUE) {
    // This event does not own the pipe, so don't close it.
    FlushFileBuffers(hPipe_);
    hPipe_ = INVALID_HANDLE_VALUE;
  }
}

// Writes a string to the pipe. Returns True if successful, else returns False.
bool WriteMessageToPipe(HANDLE pipe, const std::string& message) {
  if (message.empty())
    return false;
  DWORD written;
  // TODO: use overlapped IO?
  return WriteFile(pipe, message.data(), message.size(), &written, nullptr);
}


}  // namespace sdk
}  // namespace content_analysis
