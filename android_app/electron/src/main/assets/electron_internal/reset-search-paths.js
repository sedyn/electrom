// lib/common/reset-search-paths.ts
const Module = require('module');
Module.globalPaths.length = 0;

const electronExports = require('./exports-electron.js');

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
