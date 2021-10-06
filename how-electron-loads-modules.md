# How electron loads modules

## require('electron')
electron은 main 소스를 실행하기 전 `electron/js2c/${process_type}_init`을 실행한다.

```cpp
  std::string init_script = "electron/js2c/" + process_type + "_init";

  args.insert(args.begin() + 1, init_script);

  isolate_data_ =
      node::CreateIsolateData(context->GetIsolate(), uv_loop_, platform);

  node::Environment* env;
  if (browser_env_ != BrowserEnvironment::kBrowser) {
    // Only one ESM loader can be registered per isolate -
    // in renderer processes this should be blink. We need to tell Node.js
    // not to register its handler (overriding blinks) in non-browser processes.
    uint64_t flags = node::EnvironmentFlags::kDefaultFlags |
                     node::EnvironmentFlags::kNoRegisterESMLoader |
                     node::EnvironmentFlags::kNoInitializeInspector;
    v8::TryCatch try_catch(context->GetIsolate());
    env = node::CreateEnvironment(
        isolate_data_, context, args, exec_args,
        static_cast<node::EnvironmentFlags::Flags>(flags));
    DCHECK(env);

    // This will only be caught when something has gone terrible wrong as all
    // electron scripts are wrapped in a try {} catch {} by webpack
    if (try_catch.HasCaught()) {
      LOG(ERROR) << "Failed to initialize node environment in process: "
                 << process_type;
    }
  } else {
    env = node::CreateEnvironment(isolate_data_, context, args, exec_args);
    DCHECK(env);
  }
```

해당 파일들은 원래 `require('electron')`을 사용하고 있으나 webpack으로 빌드되면서 다음과 같이 변경된다.

>  `require('./lib/${process_type}/api/exports/electron.ts')`

해당 변경에 대한 설정은 `electron/build/webpack/webpack.config.base.js` 파일에 존재한다.

```js
const electronRoot = path.resolve(__dirname, '../..');

return {
    mode: 'development',
    devtool: false,
    entry,
    target: alwaysHasNode ? 'node' : 'web',
    output: {
    filename: outputFilename
    },
    resolve: {
    alias: {
        '@electron/internal': path.resolve(electronRoot, 'lib'),
        electron$: electronAPIFile,
        'electron/main$': electronAPIFile,
        'electron/renderer$': electronAPIFile,
        'electron/common$': electronAPIFile,
        // Force timers to resolve to our dependency that doesn't use window.postMessage
        timers: path.resolve(electronRoot, 'node_modules', 'timers-browserify', 'main.js')
    },
    extensions: ['.ts', '.js']
    },
    module: {
    rules: [{
        test: (moduleName) => !onlyPrintingGraph && ignoredModules.includes(moduleName),
        loader: 'null-loader'
    }, {
        test: /\.ts$/,
        loader: 'ts-loader',
        options: {
        configFile: path.resolve(electronRoot, 'tsconfig.electron.json'),
        transpileOnly: onlyPrintingGraph,
        ignoreDiagnostics: [
            // File '{0}' is not under 'rootDir' '{1}'.
            6059
        ]
        }
    }]
    },
```

또한 실제 export 되는 모듈들은 다음과 같이 등록된다. 

```ts
// electron/lib/${process_type}/api/module-list.ts

export const browserModuleList: ElectronInternal.ModuleEntry[] = [
  { name: 'app', loader: () => require('./app') },
  { name: 'autoUpdater', loader: () => require('./auto-updater') },
  { name: 'BaseWindow', loader: () => require('./base-window') },
  { name: 'BrowserView', loader: () => require('./browser-view') },
  { name: 'BrowserWindow', loader: () => require('./browser-window') },
  { name: 'contentTracing', loader: () => require('./content-tracing') },
  { name: 'crashReporter', loader: () => require('./crash-reporter') },
  { name: 'dialog', loader: () => require('./dialog') },
  { name: 'globalShortcut', loader: () => require('./global-shortcut') },
  { name: 'ipcMain', loader: () => require('./ipc-main') },
  { name: 'inAppPurchase', loader: () => require('./in-app-purchase') },
  { name: 'Menu', loader: () => require('./menu') },
  { name: 'MenuItem', loader: () => require('./menu-item') },
  { name: 'MessageChannelMain', loader: () => require('./message-channel') },
  { name: 'nativeImage', loader: () => require('./native-image') },
  { name: 'nativeTheme', loader: () => require('./native-theme') },
  { name: 'net', loader: () => require('./net') },
  { name: 'netLog', loader: () => require('./net-log') },
  { name: 'Notification', loader: () => require('./notification') },
  { name: 'powerMonitor', loader: () => require('./power-monitor') },
  { name: 'powerSaveBlocker', loader: () => require('./power-save-blocker') },
  { name: 'protocol', loader: () => require('./protocol') },
  { name: 'screen', loader: () => require('./screen') },
  { name: 'session', loader: () => require('./session') },
  { name: 'ShareMenu', loader: () => require('./share-menu') },
  { name: 'systemPreferences', loader: () => require('./system-preferences') },
  { name: 'TouchBar', loader: () => require('./touch-bar') },
  { name: 'Tray', loader: () => require('./tray') },
  { name: 'View', loader: () => require('./view') },
  { name: 'webContents', loader: () => require('./web-contents') },
  { name: 'WebContentsView', loader: () => require('./web-contents-view') },
  { name: 'webFrameMain', loader: () => require('./web-frame-main') }
];

if (BUILDFLAG(ENABLE_DESKTOP_CAPTURER)) {
  browserModuleList.push(
    { name: 'desktopCapturer', loader: () => require('./desktop-capturer') }
  );
}

if (BUILDFLAG(ENABLE_VIEWS_API)) {
  browserModuleList.push(
    { name: 'ImageView', loader: () => require('./views/image-view') }
  );
}

```

## Internal bindings