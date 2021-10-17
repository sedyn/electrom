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
    },
    {
        name: 'ipcMain', loader: () => {
            return require('./ipc-main.js');
        }
    }
];

module.exports = browserModules;
