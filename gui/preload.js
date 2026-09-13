const { contextBridge, ipcRenderer } = require('electron');

contextBridge.exposeInMainWorld('bushAPI', {
  // Отправляет команду в C++ ядро и возвращает результат
  run: (command) => ipcRenderer.invoke('run-command', command),
});
