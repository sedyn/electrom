# Electrom
# Architecture
## Files

### android_app/app/src/main/assets/electron_app
- Actual Javascript source code and asset files for electron app.

### android_app/electron
- 라이브러리로 분리될 electron API 구현체

### android_app/electron/src/main/cpp
#### android_context.cpp
- `CreateWebContents`
    - Node에서 BrowserWindow 클래스가 생성될 때 호출되는 JNI 함수
- `CommandToWebContents`
    - Android WebView에 명령이 필요할 때 실행되는 JNI 함수

#### electron.cpp
- `Java_com_electrom_process_MainProcess_startMainModule`
    - electron 메인 모듈 시작 함수 Node embedded 환경을 시작한다.

### android_app/electron/src/main/java
#### ElectronApp.kt
- `startMainProcess`
    - electron 자바스크립트 코드를 로드하기 위한 진입점

#### process/MainProcess.kt
- Node embedded 환경을 구성하고 NDK에서 실행되는 Java함수를 핸들링하는 클래스

#### process/RendererProcess.kt
- ElectronWebView를 관리하는 클래스

#### view/ElectronWebView.kt
- electron의 요구사항에 맞는 옵션이 설정되어 있는 WebView

## Current Goal
- Support only one renderer process
- Implement features in these projects first
    - [x] https://github.com/electron/simple-samples/tree/master/prices

## Not Goal
- webPrefernces options (e.g. Node integration)
    - but, you can use devTools option via chrome `chrome://inspect/#devices`


# Important concepts in Electron
## Wrappable
Wrappable makes relation between C++ instance and JS object.

```cpp
// Skip Isolate for simple code
template <typename T>
class Wrappable {
    v8::Global<v8::Object> wrapper_

    // Actually, this method is GetObjectTemplateBuilder
    virtual Local<Object> CreateWrapper();

    v8::MaybeLocal<v8::Object> GetWrapper() {
        if (!wrapper_.IsEmpty()) {
            return wrapper_;
        }

        // For singleton instance
        wrapper_.Reset(CreateWrapper());
        return v8::MaybeLocal<v8::Object>(wrapper_);
    }

    // For non-singleton instance
    virtual void InitWith(v8::Local<v8::Object> wrapper) {
        wrapper->SetAlignedPointerInInternalField(0, this);
        wrapper_.Reset(wrapper);
    }
}

class App : public Wrappable<App> {
    // Now we can get JS object from instance.
}
```

## TrackableObject
TrackableObject is used to track instance.

```cpp
// Skip Isolate for simple code
template <typename T>
class TrackableObject : public Wrappable<T> {

    TrackableObject() {
        weak_map_id = ++next_id_;
    }

    InitWith(v8::Local<v8::Object> wrapper) {
        Wrappable<TrackableObject>::InitWith(wrapper);
        weak_map_->Set(weak_map_id, wrapper);
    }

    static T* FromWeakMapID(int32_t id) {
        v8::MaybeLocal<v8::Object> object = weak_map->Get(id);
        if (object.IsEmpty())
            return nullptr;

        T* self = nullptr;
        gin::ConvertFromV8(object.ToLocalChecked(), &self);
        return self;
    }

    int32_t weak_map_id_;

    static WeakMap<int32_t>* weak_map_;

    static int32_t next_id_ = 0;
}

class BrowserWindow : public TrackableObject<BrowserWindow> {
    // Now we can track instance by id and get JS object from instance.
}
```

## EventEmitterMixin

# How Electron loads modules
## Internal `require('electron')`
Electron loads internal code before executing `MainStartupScript`.

Electron uses js2c in Node.js for embedding javascript code into C++ binary.

Electron uses Webpack to merge `lib/browser/**/*.ts` into single file `browser_init.js`. 
When Webpack complies these files, it transforms `require('electron')` 
to `require('lib/browser/api/exports/electron.ts')` because of Webpack config files in Electron.


## MainStartupScript `require('electron')`
```js
// If you start this code with `electron app.js`
// This code will return Electron modules.

// If you start this code with `node app.js`
// This code will return path of electron binary.
require('electron')
```

`browser_init.js` manipulates `Module` (in `require('module'`).

`Module` manages search path of `require` function and cache of Node.js libraries.

[lib/common/reset-search-path.ts](https://github.com/electron/electron/blob/main/lib/common/reset-search-paths.ts) adds Electron modules in `lib/browser/api/exports/electron.ts` into cache with keys; (`'electron'`, `'electron/main'`).
