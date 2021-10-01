#include "node.h"
#include "libnode/include/node/env-inl.h"

#include <vector>

using namespace v8;

NodeBinding::NodeBinding() {
    uv_loop_ = uv_default_loop();
    dummy_uv_handle_ = new uv_async_t;
}

void NodeBinding::Initialize() {
    std::vector<std::string> argv = {"electron"};
    std::vector<std::string> exec_argv;
    std::vector<std::string> errors;

    int exit_code = node::InitializeNodeWithArgs(&argv, &exec_argv, &errors);
    if (exit_code != 0) {
        return;
    }
}

void NodeBinding::LoadEnvironment(node::Environment *env) {
    node::LoadEnvironment(env, node::StartExecutionCallback{});
}

node::Environment *NodeBinding::CreateEnvironment(
    v8::Handle<v8::Context> context,
    node::MultiIsolatePlatform *platform
) {
    isolate_data_ = node::CreateIsolateData(context->GetIsolate(), uv_loop_, platform);

    std::vector<std::string> args = {"electron"};
    std::vector<std::string> exec_args;

    node::Environment *env = node::CreateEnvironment(
        isolate_data_,
        context,
        args,
        exec_args
    );

    if (env == nullptr) {
        return nullptr;
    }

    node::IsolateSettings is;

    is.should_abort_on_uncaught_exception_callback = [](Isolate *) {
        return false;
    };

    is.policy = v8::MicrotasksPolicy::kExplicit;

    node::SetIsolateUpForNode(context->GetIsolate(), is);

    return env;
}

void NodeBinding::PrepareMessageLoop() {
    // Add dummy handle for libuv, otherwise libuv would quit when there is
    // nothing to do.
    uv_async_init(
        uv_loop_,
        dummy_uv_handle_,
        nullptr
    );
}

void NodeBinding::RunMessageLoop() {
    node::Environment *env = uv_env();
    if (!env) {
        return;
    }

    Locker locker(env->isolate());

}