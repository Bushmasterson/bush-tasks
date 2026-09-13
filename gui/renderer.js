const list = document.getElementById('task-list');
const input = document.getElementById('task-input');
const addBtn = document.getElementById('add-btn');

async function refreshTasks() {
  try {
    const output = await window.bushAPI.run('tasks');
    // Парсим вывод C++ (формат: "1. [pending] [high] Buy milk (created: ...)")
    const lines = output.split('\n').filter(l => l.trim());
    list.innerHTML = lines.map(line => `<li>${line}</li>`).join('');
  } catch (err) {
    list.innerHTML = `<li style="color: red;">Ошибка: ${err.message}</li>`;
  }
}

addBtn.addEventListener('click', async () => {
  const text = input.value.trim();
  if (!text) return;
  await window.bushAPI.run(`add ${text}`);
  input.value = '';
  refreshTasks();
});

input.addEventListener('keydown', (e) => {
  if (e.key === 'Enter') addBtn.click();
});

// Загружаем задачи при старте
refreshTasks();
