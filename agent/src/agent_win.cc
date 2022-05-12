// Copyright 2022 The Chromium Authors.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <utility>
#include <vector>

#include <windows.h>
#include <sddl.h>

#include "common/utils_win.h"

#include "agent_win.h"
#include "event_win.h"

namespace content_analysis {
namespace sdk {

const DWORD kBufferSize = 4096;

// static
std::unique_ptr<Agent> Agent::Create(
    Config config,
    std::unique_ptr<AgentEventHandler> handler) {
  return std::make_unique<AgentWin>(std::move(config), std::move(handler));
}

AgentWin::AgentWin(
    Config config,
    std::unique_ptr<AgentEventHandler> handler)
  : AgentBase(std::move(config), std::move(handler)) {
  if (handler() == nullptr)
    return;

  std::string pipename =
      internal::GetPipeName(configuration().name,
                            configuration().user_specific);
  if (pipename.empty())
    return;

  pipename_ = pipename;
  CreatePipe(&hPipe_);
}

AgentWin::~AgentWin() {
  Shutdown();
}

void AgentWin::HandleEvents() {
  // TODO: handle events.
}

int AgentWin::Stop() {
  Shutdown();
  return AgentBase::Stop();
}


// static
DWORD AgentWin::CreatePipe(const std::string& name, HANDLE* handle) {
  DWORD err = ERROR_SUCCESS;

  *handle = CreateNamedPipeA(name.c_str(),
    PIPE_ACCESS_DUPLEX, PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT |
        PIPE_REJECT_REMOTE_CLIENTS,
    PIPE_UNLIMITED_INSTANCES, kBufferSize, kBufferSize, 0, nullptr);
  if (*handle == INVALID_HANDLE_VALUE) {
    err = GetLastError();
  }

  return err;
}

// static
DWORD AgentWin::ConnectPipe(HANDLE handle) {
  BOOL connected = ConnectNamedPipe(handle, nullptr) ||
      (GetLastError() == ERROR_PIPE_CONNECTED);
  return connected ? ERROR_SUCCESS :  GetLastError();
}

void AgentWin::Shutdown() {
  for (auto handle : browsers_) {
    CloseHandle(hPipe_);
  }
  browsers_.clear();

  if (hPipe_ != INVALID_HANDLE_VALUE) {
    CloseHandle(hPipe_);
    hPipe_ = INVALID_HANDLE_VALUE;
  }
}

}  // namespace sdk
}  // namespace content_analysis
