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
  // Represents one connection to a Google Chrome browser, or one pipe
  // listening for a Google Chrome browser to connect.
  class Connection {
   public:
    // Starts listening on a pipe with the given name. `is_first_pipe` should
    // be true only for the first pipe created by the agent.
    Connection(std::string& pipename, AgentEventHandler* handler,
               bool is_first_pipe);
    Connection(Connection&& other);
    Connection& operator=(Connection&& other);
    ~Connection();

    bool IsValid() const { return handle_ != INVALID_HANDLE_VALUE; }
    bool IsConnected() const { return is_connected_; }
    HANDLE GetWaitHandle() const { return overlapped_.hEvent; }

    // Resets this connection object to listen for a new Google Chrome browser.
    DWORD Reset(std::string& pipename);

    // Hnadles an event for this connection.  `wait_handle` corresponds to
    // this connections wait handle.
    int HandleEvent(HANDLE wait_handle);

   private:
    // Creates a new server endpoint of the pipe and returns the handle. If
    // successful ERROR_SUCCESS is returned and `handle` is set to the pipe's
    // server endpoint handle.  Otherwise an error code is returned and `handle`
    // is set to INVALID_HANDLE_VALUE.
    DWORD CreatePipe(const std::string& name,
                     bool is_first_pipe,
                     HANDLE* handle);

    // Listens for a new connection from Google Chrome.
    DWORD ConnectPipe();

    // Resets this connection object to listen for a new Google Chrome browser.
    DWORD ResetInternal(std::string& pipename, bool is_first_pipe);

    // Cleans up this connection object so that it can be reused with a new
    // Google Chroem browser instance.  The handles assocated with this object
    // are not closed.  On return, this object is neither connected nor
    // listening and any buffer used to hold browser messages are cleared.
    void Cleanup();

    // Queues a read on the pipe to receive a message from Google Chrome.
    // ERROR_SUCCESS and ERROR_IO_PENDING are both successful return values.
    // Other values represent an error with the connection.
    DWORD QueueReadFile();

    // Called when data from Google Chrome is available for reading from the
    // pipe. ERROR_SUCCESS and ERROR_MORE_DATA are both successful return
    // values.  Other values represent an error with the connection.
    DWORD OnReadFile(BOOL success, DWORD count);

    // Calls the appropriate method the handler depending on the message
    // received from Google Chrome.
    DWORD CallHandler();

    // Fills in the browser_info_ member of this Connection.  Assumes
    // IsConnected() is true.
    DWORD BuildBrowserInfo();

    // The handler to call for various agent events.
    AgentEventHandler* handler_ = nullptr;

    // Members used to communicate with Google Chrome.
    HANDLE handle_ = INVALID_HANDLE_VALUE;
    OVERLAPPED overlapped_;

    // True if this connection is assigned to a specific Google Chrome browser,
    // otherwise this connection is listening for a new browser.
    bool is_connected_ = false;

    // Information about the Google Chrome browser process.
    BrowserInfo browser_info_;

    // Members used to read messages from Google Chrome.
    std::vector<char> buffer_;
    char* cursor_ = nullptr;
    DWORD read_size_ = 0;
    DWORD final_size_ = 0;
  };

  // Returns handles that can be used to wait for events from all Connection
  // objects managed by this agent.
  void GetHandles(std::vector<HANDLE>& wait_handles) const;

  // Performs a clean shutdown of the agent.
  void Shutdown();

  // Name used to create the pipes between the agent and Google Chrome browsers.
  std::string pipename_;

  // A list of pipes to already connected Google Chrome browsers.
  // The first kNumPipeInstances pipes in the list correspond to listening
  // pipes.
  std::vector<Connection> connections_;
};

}  // namespace sdk
}  // namespace content_analysis

#endif  // CONTENT_ANALYSIS_AGENT_SRC_AGENT_WIN_H_