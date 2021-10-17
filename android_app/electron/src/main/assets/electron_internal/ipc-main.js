const IpcMainImpl = require('./ipc-main-impl.js');

const ipcMain = new IpcMainImpl();

ipcMain.on('error', () => {});

module.exports = ipcMain;
