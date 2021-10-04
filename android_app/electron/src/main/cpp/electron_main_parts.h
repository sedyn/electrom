#ifndef ANDROID_APP_ELECTRON_MAIN_PARTS_H
#define ANDROID_APP_ELECTRON_MAIN_PARTS_H

#include "node.h"
#include "javascript_environment.h"

class ElectronMainParts {
public:
    ElectronMainParts();

    void Initialize(const char *main_module_path);

    int RunMessageLoop();

    void UvRunOnce();

private:
    std::unique_ptr<NodeBinding> node_binding_;
    std::unique_ptr<JavascriptEnvironment> js_env_;
    // std::unique_ptr<Environment> node_env_;
};

#endif //ANDROID_APP_ELECTRON_MAIN_PARTS_H
