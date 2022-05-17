// Copyright 2022 The Chromium Authors.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CONTENT_ANALYSIS_AGENT_SRC_EVENT_BASE_H_
#define CONTENT_ANALYSIS_AGENT_SRC_EVENT_BASE_H_

#include "content_analysis/sdk/analysis_agent.h"

namespace content_analysis {
namespace sdk {

// Base ContentAnalysisEvent class with code common to all platforms.
class ContentAnalysisEventBase : public ContentAnalysisEvent {
 public:
   // ContentAnalysisEvent:
  int Close() override;
  const ContentAnalysisRequest& GetRequest() const override { return request_; }
  ContentAnalysisResponse& GetResponse() override { return response_; }

 protected:
  ContentAnalysisEventBase() = default;

  ContentAnalysisRequest* request() { return &request_; }
  ContentAnalysisResponse* response() { return &response_; }

private:
  ContentAnalysisRequest request_;
  ContentAnalysisResponse response_;
};

}  // namespace sdk
}  // namespace content_analysis

#endif  // CONTENT_ANALYSIS_AGENT_SRC_EVENT_BASE_H_
