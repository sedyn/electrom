# 일렉트론 구조 파악
의식의 흐름대로 일단 정리

가장 궁금했던 점은 main process와 renderer process에 electron 모듈이 주입되는 방법에 대해서 연구
- electron에서는 2014년부터 [gin](https://github.com/chromium/chromium/tree/master/gin)이라고 하는 chromium에서 사용되는 v8 <-> C++를 연동해주는 라이브러리를 사용하고있음
    - `electron_api_browser_window.cc:539`에서는 `export` 객체에 `BrowserWindow`클래스를 등록하고 있음
- electron_api_(feature).{cc,h}에서 확인할 수 있음
- third_party/electron_node/... -> Node.js 프로젝트 (.gclient_entries 확인)
- 각 electron API의 Intialize 시그니쳐에 대한 정보: https://nodejs.org/api/addons.html#addons_context_aware_addons

## 폴더별
Windows 기준으로 보는 정리 중

- shell/app/browser/*
    - app, BrowserWindow, WebContents
- shell/app/render/*
    - ipcRenderer

# 참고 URL
- https://cameronnokes.com/blog/deep-dive-into-electron's-main-and-renderer-processes/
- https://stackoverflow.com/questions/9475792/how-does-require-in-node-js-work

# 봐야할 파일들
- module_system.cc
    - ModuleSystem::AddRoutes
        - require, loadScript 참고

- builtins-api.cc ~> electron 모듈 객체 생성시 호출됨

- third_party/electron_node/src/node.cc
    - `StartExecution`
        - (lib/)internal/main/run_amin_module.js
- third_party/electron_node/src/api/environment.cc
    - `LoadEnvironment`
- third_party/electron_node/src/node_main_instance.cc
