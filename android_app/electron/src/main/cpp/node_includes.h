#ifndef ANDROID_APP_NODE_INCLUDES_H_
#define ANDROID_APP_NODE_INCLUDES_H_

#include "libnode/include/node/node_binding.h"

#define NODE_LINKED_MODULE_CONTEXT_AWARE(modname, regfunc) \
  NODE_MODULE_CONTEXT_AWARE_CPP(modname, regfunc, nullptr, NM_F_LINKED)

#endif
