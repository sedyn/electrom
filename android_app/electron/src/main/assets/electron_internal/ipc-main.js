const IpcMainImpl = require('./ipc-main-impl.js');

const ipcMain = new IpcMainImpl();

ipcMain.on('error', () => {});

process._linkedBinding('electron_ipc_main')._init(ipcMain);

module.exports = ipcMain;
