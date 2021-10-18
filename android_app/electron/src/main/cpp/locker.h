#ifndef ANDROID_APP_LOCKER_H
#define ANDROID_APP_LOCKER_H

#include "libnode/include/node/v8.h"

namespace gin_helper {

    class ElectronLocker {
    public:
        explicit ElectronLocker(v8::Isolate *isolate) {
            if (IsBrowserProcess())
                locker_ = std::make_unique<v8::Locker>(isolate);
        }

        ~ElectronLocker() = default;

        static inline bool IsBrowserProcess() { return v8::Locker::IsActive(); }

    private:
        std::unique_ptr <v8::Locker> locker_;
    };

}

#endif
