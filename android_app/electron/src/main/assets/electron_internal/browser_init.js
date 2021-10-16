// set EventEmitter first
const events = require('events');
process._linkedBinding('electron_browser_event_emitter').setEventEmitterPrototype(events.EventEmitter.prototype);

// We modified the original process.argv to let node.js load the init.js,
// we need to restore it here.
process.argv.splice(1, 1);

const Module = require('module');
Module.globalPaths.length = 0;

// lib/browser/api/module-lists.ts
const browserModules = [
    {
        name: 'app', loader: () => {
            const { app } = process._linkedBinding('electron_browser_app');
            return app;
        }
    },
    {
        name: 'BrowserWindow', loader: () => {
            const { BrowserWindow } = process._linkedBinding('electron_browser_window');
            return BrowserWindow;
        }
    }
]

// lib/common/define-properties.ts
const handleESModule = (loader) => () => {
    const value = loader();
    if (value.__esModule && value.default) return value.default;
    return value;
}

function defineProperties(targetExports, moduleList) {
    const descriptors = {};
    for (const module of moduleList) {
        descriptors[module.name] = {
            enumerable: !module.private,
            get: handleESModule(module.loader)
        };
    }
    return Object.defineProperties(targetExports, descriptors);
}

// lib/browser/api/exports/electron.ts
const electronExports = {};
const electronBrowserModules = defineProperties(electronExports, browserModules);

// lib/common/reset-search-paths.ts
const makeElectronModule = (name) => {
    const electronModule = new Module('electron', null);
    electronModule.id = 'electron';
    electronModule.loaded = true;
    electronModule.filename = name;
    Object.defineProperty(electronModule, 'exports', {
        get: () => electronExports
    });
    Module._cache[name] = electronModule
}

makeElectronModule('electron');

const originalResolveFilename = Module._resolveFilename;
Module._resolveFilename = function(request, parent, isMain, options) {
    if (request === 'electron' || request.startsWith('electron/')) {
        return 'electron'
    } else {
        return originalResolveFilename(request, parent, isMain, options);
    }
}

console.log('READY')

// lib/browser/init.ts
Module._load(process.argv[1], Module, true)
