#ifndef ANDROID_APP_TRACKABLE_OBJECT_H
#define ANDROID_APP_TRACKABLE_OBJECT_H

#include "wrappable.h"
#include "event_emitter_mixin.h"
#include "converter.h"

#include <unordered_map>

namespace electron {
    template<typename K>
    class KeyWeakMap {
    public:
        struct KeyObject {
            K key;
            KeyWeakMap *self;
        };

        KeyWeakMap() {}

        void Set(v8::Isolate *isolate, const K &key, v8::Local<v8::Object> object) {
            KeyObject key_object = {key, this};
            auto &p = map_[key] = std::make_pair(key_object, v8::Global<v8::Object>(isolate, object));
        }

        v8::MaybeLocal<v8::Object> Get(v8::Isolate *isolate, const K &key) {
            auto iter = map_.find(key);
            if (iter == map_.end()) {
                return v8::MaybeLocal<v8::Object>();
            } else {
                return v8::Local<v8::Object>::New(isolate, iter->second.second);
            }
        }

    private:
        std::unordered_map<K, std::pair<KeyObject, v8::Global<v8::Object>>> map_;
    };

}


namespace gin_helper {
    template<typename T>
    class TrackableObject : public Wrappable<T>,
                            public EventEmitterMixin<T> {
    public:
        static T *FromWeakMapID(v8::Isolate *isolate, int id) {
            if (!weak_map_)
                return nullptr;

            v8::HandleScope scope(isolate);
            v8::MaybeLocal<v8::Object> object = weak_map_->Get(isolate, id);
            if (object.IsEmpty())
                return nullptr;

            T *self = nullptr;
            gin::ConvertFromV8(isolate, object.ToLocalChecked(), &self);
            return self;
        }

    protected:
        TrackableObject() {
            weak_map_ = new electron::KeyWeakMap<int>;
            weak_map_id_ = ++next_id_;
        }

        void InitWith(v8::Isolate *isolate, v8::Local<v8::Object> wrapper) {
            gin_helper::WrappableBase::InitWith(isolate, wrapper);
            if (!weak_map_) {
                weak_map_ = new electron::KeyWeakMap<int>;
            }

            weak_map_->Set(isolate, weak_map_id_, wrapper);
        }

        int weak_map_id_ = 0;

    private:

        static int next_id_;
        static electron::KeyWeakMap<int> *weak_map_;
    };

    template<typename T>
    int TrackableObject<T>::next_id_ = 0;

    template<typename T>
    electron::KeyWeakMap<int> *TrackableObject<T>::weak_map_ = nullptr;
}


#endif //ANDROID_APP_TRACKABLE_OBJECT_H
