// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the Apache 2.0 License.
#pragma once

#include "consensus/aft/raft_types.h"
#include "consensus/aft/request.h"
#include "enclave/rpc_map.h"
#include "state.h"

namespace enclave
{
  class RPCSessions;
  class RPCMap;
}

namespace aft
{
  class RequestMessage;

  struct RequestCtx
  {
    std::shared_ptr<enclave::RpcContext> ctx;
    std::shared_ptr<enclave::RpcHandler> frontend;
  };

  class Executor
  {
  public:
    virtual ~Executor() = default;
    virtual std::unique_ptr<RequestCtx> create_request_ctx(
      uint8_t* req_start, size_t req_size) = 0;

    virtual std::unique_ptr<RequestCtx> create_request_ctx(
      Request& request) = 0;

    virtual kv::Version execute_request(
      std::unique_ptr<RequestMessage> request, bool is_create_request) = 0;

    virtual std::unique_ptr<aft::RequestMessage> create_request_message(
      const kv::TxHistory::RequestCallbackArgs& args) = 0;

    virtual kv::Version commit_replayed_request(kv::Tx& tx) = 0;
  };

  class ExecutorImpl : public Executor
  {
  public:
    ExecutorImpl(
      RequestsMap& pbft_requests_map_,
      std::shared_ptr<State> state_,
      std::shared_ptr<enclave::RPCMap> rpc_map_,
      std::shared_ptr<enclave::RPCSessions> rpc_sessions_) :
      pbft_requests_map(pbft_requests_map_),
      state(state_),
      rpc_map(rpc_map_),
      rpc_sessions(rpc_sessions_)
    {}

    std::unique_ptr<RequestCtx> create_request_ctx(
      uint8_t* req_start, size_t req_size) override;

    std::unique_ptr<RequestCtx> create_request_ctx(Request& request) override;

    kv::Version execute_request(
      std::unique_ptr<RequestMessage> request, bool is_create_request) override;

    std::unique_ptr<aft::RequestMessage> create_request_message(
      const kv::TxHistory::RequestCallbackArgs& args) override;

    kv::Version commit_replayed_request(kv::Tx& tx) override;

  private:
    RequestsMap& pbft_requests_map;
    std::shared_ptr<State> state;
    std::shared_ptr<enclave::RPCMap> rpc_map;
    std::shared_ptr<enclave::RPCSessions> rpc_sessions;
  };
}
