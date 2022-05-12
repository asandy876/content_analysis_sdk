// Copyright 2022 The Chromium Authors.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CONTENT_ANALYSIS_AGENT_SRC_AGENT_WIN_H_
#define CONTENT_ANALYSIS_AGENT_SRC_AGENT_WIN_H_

#include <windows.h>

#include <string>
#include <vector>

#include "agent_base.h"

namespace content_analysis {
namespace sdk {

// Agent implementaton for Windows.
class AgentWin : public AgentBase {
 public:
  AgentWin(Config config, std::unique_ptr<AgentEventHandler> handler);
  ~AgentWin() override;

  // Agent:
  void HandleEvents() override;
  int Stop() override;

 private:
  // Creates a new server endpoint of the pipe and returns the handle. If
  // successful ERROR_SUCCESS is returned and `handle` is set to the pipe's
  // server endpoint handle.  Otherwise an error code is returned and `handle`
  // is set to INVALID_HANDLE_VALUE.
  static DWORD CreatePipe(const std::string& name, HANDLE* handle);

  // Blocks and waits until a client connects.
  static DWORD ConnectPipe(HANDLE handle);

  void Shutdown();

  // Name used to create the pipes between the agent and Google Chrome browsers.
  std::string pipename_;

  // The handle to the pipe on which the agent is currently waiting for a
  // Google Chrome browser to connect to.
  HANDLE hPipe_ = INVALID_HANDLE_VALUE;

  // A list of handles to already connected Google Chrome browsers.
  std::vector<HANDLE> browsers_;
};

}  // namespace sdk
}  // namespace content_analysis

#endif  // CONTENT_ANALYSIS_AGENT_SRC_AGENT_WIN_H_