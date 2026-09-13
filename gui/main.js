const { app, BrowserWindow, ipcMain, dialog } = require('electron');
const path = require('path');
const fs = require('fs');
const { spawn } = require('child_process');

const BINARY_NAME = process.platform === 'win32' ? 'bush-tasks.exe' : 'bush-tasks';

// В dev — из ../build/, в prod — из resources/bin/
const BINARY_PATH = app.isPackaged
  ? path.join(process.resourcesPath, 'bin', BINARY_NAME)
  : path.join(__dirname, '..', 'build', BINARY_NAME);

function createWindow() {
  const win = new BrowserWindow({
    width: 800,
    height: 600,
    webPreferences: {
      preload: path.join(__dirname, 'preload.js'),
      contextIsolation: true,
      nodeIntegration: false,
    },
  });

  win.loadFile(path.join(__dirname, 'index.html'));
}

ipcMain.handle('run-command', async (event, command) => {
  return new Promise((resolve, reject) => {
    const child = spawn(BINARY_PATH, [], { stdio: ['pipe', 'pipe', 'pipe'] });

    let output = '';
    let errorOutput = '';

    child.stdout.on('data', (data) => { output += data.toString(); });
    child.stderr.on('data', (data) => { errorOutput += data.toString(); });

    child.on('error', (err) => {
      reject(new Error(`Failed to start bush-tasks: ${err.message}\nPath: ${BINARY_PATH}`));
    });

    child.on('close', (code) => {
      if (code !== 0 && errorOutput) {
        reject(new Error(errorOutput));
      } else {
        resolve(output);
      }
    });

    child.stdin.write(command + '\n');
    child.stdin.end();
  });
});

if (!fs.existsSync(BINARY_PATH)) {
  app.whenReady().then(() => {
    dialog.showErrorBox(
      'bush-tasks binary not found',
      `Could not find:\n${BINARY_PATH}\n\nBuild the C++ core first:\n  cmake -B build -G Ninja -DBUSH_TASKS_BUILD_GUI=ON\n  cmake --build build -j`
    );
    app.quit();
  });
} else {
  app.whenReady().then(createWindow);

  app.on('window-all-closed', () => {
    if (process.platform !== 'darwin') app.quit();
  });
}
