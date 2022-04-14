// Copyright 2022 The Chromium Authors.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CONTENT_ANALYSIS_AGENT_INCLUDE_CONTENT_ANALYSIS_SDK_ANALYSIS_AGENT_H_
#define CONTENT_ANALYSIS_AGENT_INCLUDE_CONTENT_ANALYSIS_SDK_ANALYSIS_AGENT_H_

#include <memory>
#include <string>

#include "content_analysis/sdk/analysis.pb.h"

// This is the main include file for code using Content Analysis Connector
// Agent SDK.  No other include is needed.
//
// An agent begins by creating an instance of Agent using the factory
// function Agent::Create().  This instance should live as long as the agent
// intends to receive content analysis requests from Google Chrome.
//
// To receive a content analysis request from Google Chrome the agent calls
// Session::Create() passing it the agent created above.  A session represents
// one content analysis request.  The agent reads and analyses the request in
// to determine a verdict: allow or block.  When the verdict is known the
// response is sent back to Google Chrome.
//
// The agent is not required to serialize session handling.  That is, once
// a session is created, the agent can analyze it in the background while
// calling Session::Create() to receive the next content analysis request.
//
// Google Chrome thottles the number of requests sent to the agent to 5
// current requests at a time but this is subject to change.

namespace content_analysis {
namespace sdk {

// Represents one content analysis request as generated by a given user action
// in Google Chrome.
//
// The agent should retrieve information about the content analysis request
// using the GetRequest() method.  The agent should analyze the request and
// update the response, returned by GetResponse(), with a verdict (allow or
// block).  Once the verdict is set the response can be sent back to Google
// Chrome by calling Send().
//
// The default verdict is to allow the requested user action.  If the final
// verdict is to allow then the agent does not need to update the response and
// can simply call Send().
//
// If the final verdict should be to block, the agent should first update the
// response by calling SetSessionVerdictToBlock() before calling Send().
//
// This class is not thread safe.
//
// See the demo directory for an example of how to use this class.
class Session {
 public:
  virtual ~Session() = default;

  // Prepares the session for graceful shutdown.  Upon return calls to all
  // other methods of this class will fail.
  virtual int Close() = 0;

  // Retrieves a reference to the content analysis request received from
  // Google Chrome.
  virtual const ContentAnalysisRequest& GetRequest() const = 0;

  // Retrieves a reference to the content analysis response that will be
  // sent to Google Chrome as the verdict for the request of this session.
  // The agent may modify this response in place before calling Send.
  virtual ContentAnalysisResponse& GetResponse() = 0;

  // Retrieves a reference to the Acknowledgement sent by Google Chrome after
  // the agent responds with a verdict.
  virtual const Acknowledgement& GetAcknowledgement() const = 0;

  // Send the verdict to Google Chrome.  Once this method is called further
  // changes to the response are ignored.
  virtual int Send() = 0;

 protected:
  Session() = default;
  Session(const Session& rhs) = delete;
  Session(Session&& rhs) = delete;
  Session& operator=(const Session& rhs) = delete;
  Session& operator=(Session&& rhs) = delete;
};

// Represents an agent that can perform content analysis for the Google Chrome
// browser.  This class holds the server endpoint that Google Chrome connects
// to when content analysis is required.
//
// The `uri` argument to the constuctor represent a specific local content
// analysis partner.  Each partner uses a specific and unique URI.  Only
// one Agent can be created for a given partner URI.
//
// Agent instances should outlive all Session instances created with it.
// Otherwise those sessions become invalid.
//
// See the demo directory for an example of how to use this class.
class Agent {
 public:
  // A unique identifier for Google Chrome's content analysis partner.
  using Uri = std::string;

  // Returns a new agent instance and calls Start().
  static std::unique_ptr<Agent> Create(const Uri& uri);

  virtual ~Agent() = default;

  // Returns the URI of the agent.
  virtual const Uri& GetUri() const = 0;

  // Returns a new content analysis session from Google Chrome.
  // The session's associated request and response have already been
  // properly initialized and are ready for use.
  virtual std::unique_ptr<Session> GetNextSession() = 0;

  // Prepares the agent for graceful shutdown.
  // Unblocks any sessions waiting on that client.
  virtual int Stop() = 0;

 protected:
  Agent() = default;
  Agent(const Agent& rhs) = delete;
  Agent(Agent&& rhs) = delete;
  Agent& operator=(const Agent& rhs) = delete;
  Agent& operator=(Agent&& rhs) = delete;
};

// Update the tag or status of `response`.  This function assumes that the
// response contains only one Result.  If one already exists it is updated
// otherwise a new Result is created.
//
// If `tag` is not empty it will replace the result's tag.
// If `status` is not STATUS_UNKNOWN it will will replace the result's status.
int UpdateResponse(ContentAnalysisResponse& response,
                   const std::string& tag,
                   ContentAnalysisResponse::Result::Status status);

// Sets the response verdict of a session to `action`.  This is a convenience
// function that is equivalent to the following:
//
//   auto result = session->GetResponse().mutable_results(0);
//   auto rule = result->mutable_triggered_rules(0);
//   rule->set_action(action);
//
// This function assumes the session's response has already been initialized
// using UpdateResponse().
int SetSessionVerdictTo(
    Session* session,
    ContentAnalysisResponse::Result::TriggeredRule::Action action);

// Sets the reponse verdict of a session to "block".  This is a convenience
// function that is equivalent to the following:
//
//   SetSessionVerdictTo(ContentAnalysisResponse::Result::TriggeredRule::BLOCK);
int SetSessionVerdictToBlock(Session* session);

}  // namespace sdk
}  // namespace content_analysis

#endif  // CONTENT_ANALYSIS_AGENT_INCLUDE_CONTENT_ANALYSIS_SDK_ANALYSIS_AGENT_H_
