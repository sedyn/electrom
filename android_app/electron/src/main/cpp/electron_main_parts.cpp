#include "electron_main_parts.h"
#include "libnode/include/node/env-inl.h"

using namespace v8;

using node::Environment;

ElectronMainParts::ElectronMainParts() {
    node_binding_ = std::make_unique<NodeBinding>();
}

void ElectronMainParts::Initialize(const char *main_module_path) {
    js_env_ = std::make_unique<JavascriptEnvironment>(node_binding_->uv_loop());

    HandleScope scope(js_env_->isolate());

    node_binding_->Initialize(main_module_path);

    Environment *env = node_binding_->CreateEnvironment(
            js_env_->context(), js_env_->platform(), main_module_path
    );

    env->options()->unhandled_rejections = "warn";

    // TODO Binding Electron Module

    node_binding_->LoadEnvironment(env);
    node_binding_->set_uv_env(env);
}

int ElectronMainParts::RunMessageLoop() {
    node_binding_->PrepareMessageLoop();
    node_binding_->RunMessageLoop();
    return 0;
}

void ElectronMainParts::UvRunOnce() {
    node_binding_->UvRunOnce();
}