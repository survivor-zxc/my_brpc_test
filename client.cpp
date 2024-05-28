
#include <gflags/gflags.h>
#include <butil/logging.h>
#include <butil/time.h>
#include <brpc/channel.h>
#include "test.pb.h"

DEFINE_string(protocol, "baidu_std", "Protocol type. Defined in src/brpc/options.proto");
DEFINE_string(connection_type, "", "Connection type. Available values: single, pooled, short");
DEFINE_string(server, "0.0.0.0:8000", "IP Address of server");
DEFINE_string(load_balancer, "", "The algorithm for load balancing");
DEFINE_int32(timeout_ms, 100, "RPC timeout in milliseconds");
DEFINE_int32(max_retry, 3, "Max retries(not including the first RPC)"); 
DEFINE_int32(interval_ms, 1000, "Milliseconds between consecutive requests");

void response_callback(brpc::Controller* cntl,
        TestResponse* response) {
    LOG(INFO) << "from server, response_callback: " << response->ans();
}

int main(int argc, char* argv[]) {
    gflags::ParseCommandLineFlags(&argc, &argv, true);
    brpc::Channel channel;
    brpc::ChannelOptions options;

    options.protocol = FLAGS_protocol;
    options.connection_type = FLAGS_connection_type;
    options.timeout_ms = FLAGS_timeout_ms/*milliseconds*/;
    options.max_retry = FLAGS_max_retry;
    if (channel.Init(FLAGS_server.c_str(), FLAGS_load_balancer.c_str(), &options) != 0) { // communicate with server
        LOG(ERROR) << "Fail to initialize channel";
        return -1;
    }

    TestService_Stub stub(&channel);

    TestRequest request;
    TestResponse response;
    brpc::Controller cntl;

    request.set_n1(111);
    request.set_n2(222);
    cntl.set_log_id(1);  // set by user
    
    google::protobuf::Closure* done = brpc::NewCallback(
            &response_callback, &cntl, &response);
    stub.Test(&cntl, &request, &response, done); // if done is null, then sync request. // channel_->CallMethod(descriptor()->method(0), controller, request, response, done);
    
    if (!cntl.Failed()) {
        LOG(INFO) << "Received response from " << cntl.remote_side()
            << " to " << cntl.local_side()
            << " (attached="
            << cntl.response_attachment() << ")"
            << " latency=" << cntl.latency_us() << "us";
    } else {
        LOG(WARNING) << cntl.ErrorText();
    }
    
    usleep(1 * 1000L);
    LOG(INFO) << "from server: " << response.ans(); // this response and response_callback's response are same thing.

    usleep(1 * 1000L);
    LOG(INFO) << "EchoClient is going to quit";
    return 0;
}
