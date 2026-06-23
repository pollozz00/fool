const API = 'https://localhost:7126/api';
let currentUser = null;
let currentTab = 'rating';

//ВКЛАДКИ

function showTab(tab) {
  currentTab = tab;

  document.getElementById('pageRating').classList.toggle('hidden', tab !== 'rating');
  document.getElementById('pageMatches').classList.toggle('hidden', tab !== 'matches');
  document.getElementById('btnTabRating').classList.toggle('active', tab === 'rating');
  document.getElementById('btnTabMatches').classList.toggle('active', tab === 'matches');

  if (tab === 'matches') {
    if (!currentUser) {
      document.getElementById('notLoggedIn').classList.remove('hidden');
      document.getElementById('historySection').classList.add('hidden');
    } else {
      document.getElementById('notLoggedIn').classList.add('hidden');
      document.getElementById('historySection').classList.remove('hidden');
      loadHistory();
    }
  }
}

//РЕЙТИНГ

async function loadRating() {
  const loading = document.getElementById('loading');
  const error = document.getElementById('error');
  const table = document.getElementById('ratingTable');

  loading.classList.remove('hidden');
  error.classList.add('hidden');
  table.classList.add('hidden');

  try {
    const res = await fetch(`${API}/Rating/leaderboard`);
    if (!res.ok) throw new Error('Помилка сервера');
    const players = await res.json();
    renderTable(players);
  } catch (e) {
    loading.classList.add('hidden');
    error.classList.remove('hidden');
    error.textContent = 'Не вдалось підключитись до сервера. Переконайся що API запущений.';
  }
}

function renderTable(players) {
  document.getElementById('loading').classList.add('hidden');
  const table = document.getElementById('ratingTable');
  const tbody = document.getElementById('ratingBody');

  tbody.innerHTML = '';

  if (players.length === 0) {
    tbody.innerHTML = '<tr><td colspan="6" style="text-align:center;color:#888;padding:2rem;">Поки що ніхто не зіграв жодної гри</td></tr>';
    table.classList.remove('hidden');
    return;
  }

  players.forEach((p, i) => {
    const wr = p.GamesPlayed > 0
      ? Math.round((p.Wins / p.GamesPlayed) * 100)
      : 0;

    const isMe = currentUser && (currentUser.username === p.Username || currentUser.Username === p.Username);
    const rankClass = i === 0 ? 'rank-1' : i === 1 ? 'rank-2' : i === 2 ? 'rank-3' : '';
    const rankIcon = i + 1;

      const tr = document.createElement('tr');
      tr.innerHTML = `
      <td class="rank ${rankClass}">${rankIcon}</td>
      <td class="${isMe ? 'me' : ''}">${p.DisplayName || p.Username}${isMe ? ' (ти)' : ''}</td>
      <td class="score">${p.TotalScore ?? 0}</td>
      <td class="wins">${p.Wins}</td>
      <td class="losses">${p.Losses}</td>
      <td>${p.GamesPlayed}</td>
      <td>${wr}%</td>
    `;
    tbody.appendChild(tr);
  });

  table.classList.remove('hidden');
}

//ІСТОРІЯ МАТЧІВ

async function loadHistory() {
  const historyLoading = document.getElementById('historyLoading');
  const historyTable = document.getElementById('historyTable');
  const historyEmpty = document.getElementById('historyEmpty');
  const tbody = document.getElementById('historyBody');

  historyLoading.classList.remove('hidden');
  historyTable.classList.add('hidden');
  historyEmpty.classList.add('hidden');

  try {
    const id = currentUser.Id || currentUser.id;
    const res = await fetch(`${API}/Rating/history/${id}`);
    if (!res.ok) throw new Error('Помилка');
    const matches = await res.json();

    historyLoading.classList.add('hidden');

    if (matches.length === 0) {
      historyEmpty.classList.remove('hidden');
      return;
    }

    tbody.innerHTML = '';
    matches.forEach(m => {
      const resultLabel = m.Result === 'win' ? 'Перемога'
                        : m.Result === 'loss' ? 'Поразка'
                        : 'Нічия';

      const resultClass = m.Result === 'win' ? 'wins'
                        : m.Result === 'loss' ? 'losses'
                        : '';

      const date = new Date(m.PlayedAt).toLocaleString('uk-UA', {
        day: '2-digit', month: '2-digit', year: 'numeric',
        hour: '2-digit', minute: '2-digit'
      });

      const tr = document.createElement('tr');
      tr.innerHTML = `
        <td class="${resultClass}">${resultLabel}</td>
        <td>${m.Score}</td>
        <td style="color:#888;font-size:0.85rem;">${date}</td>
      `;
      tbody.appendChild(tr);
    });

    historyTable.classList.remove('hidden');
  } catch (e) {
    historyLoading.classList.add('hidden');
    historyEmpty.textContent = 'Не вдалось завантажити історію матчів.';
    historyEmpty.classList.remove('hidden');
  }
}

//АВТОРИЗАЦІЯ

async function doLogin() {
  const username = document.getElementById('loginUsername').value.trim();
  const password = document.getElementById('loginPassword').value;
  const msg = document.getElementById('loginMsg');

  if (!username || !password) {
    showMsg(msg, 'Заповни всі поля', 'error');
    return;
  }

  try {
    const res = await fetch(`${API}/Player/login`, {
      method: 'POST',
      headers: { 'Content-Type': 'application/json' },
      body: JSON.stringify({ username, password })
    });

    if (res.ok) {
      currentUser = await res.json();
        showMsg(msg, `Вітаємо, ${currentUser.displayName}!`, 'success');
      setTimeout(() => {
        closeModal();
        updateNav();
        loadRating();
        if (currentTab === 'matches') showTab('matches');
      }, 800);
    } else if (res.status === 401) {
      showMsg(msg, 'Невірний логін або пароль', 'error');
    } else {
      showMsg(msg, `Помилка: ${res.status}`, 'error');
    }
  } catch (e) {
    showMsg(msg, 'Сервер недоступний', 'error');
  }
}

async function doRegister() {
  const username = document.getElementById('regUsername').value.trim();
  const displayName = document.getElementById('regDisplayName').value.trim();
  const password = document.getElementById('regPassword').value;
  const msg = document.getElementById('regMsg');

  if (!username || !password || !displayName) {
    showMsg(msg, 'Заповни всі поля', 'error');
    return;
  }

  try {
    const res = await fetch(`${API}/Player/register`, {
      method: 'POST',
      headers: { 'Content-Type': 'application/json' },
      body: JSON.stringify({ username, password, displayName })
    });

    if (res.status === 201) {
      showMsg(msg, 'Акаунт створено! Тепер увійди.', 'success');
      setTimeout(() => switchTab('login'), 1200);
    } else if (res.status === 409) {
      showMsg(msg, 'Цей логін вже зайнятий', 'error');
    } else {
      showMsg(msg, `Помилка: ${res.status}`, 'error');
    }
  } catch (e) {
    showMsg(msg, 'Сервер недоступний', 'error');
  }
}

function logout() {
  currentUser = null;
  updateNav();
  loadRating();
  if (currentTab === 'matches') showTab('matches');
}

//UI

function updateNav() {
  const nav = document.getElementById('navBtns');
  if (currentUser) {
    const name = currentUser.displayName;
    nav.innerHTML = `
      <span class="user-info">Привіт, <strong>${name}</strong></span>
      <button onclick="logout()">Вийти</button>
    `;
  } else {
    nav.innerHTML = `
      <button onclick="openModal('login')">Увійти</button>
      <button onclick="openModal('register')">Реєстрація</button>
    `;
  }
}

function openModal(tab) {
  document.getElementById('modalOverlay').classList.remove('hidden');
  switchTab(tab);
}

function closeModal() {
  document.getElementById('modalOverlay').classList.add('hidden');
  document.getElementById('loginMsg').classList.add('hidden');
  document.getElementById('regMsg').classList.add('hidden');
}

function closeModalOutside(e) {
  if (e.target === document.getElementById('modalOverlay')) closeModal();
}

function switchTab(tab) {
  const isLogin = tab === 'login';
  document.getElementById('formLogin').classList.toggle('hidden', !isLogin);
  document.getElementById('formRegister').classList.toggle('hidden', isLogin);
  document.getElementById('tabLogin').classList.toggle('active', isLogin);
  document.getElementById('tabRegister').classList.toggle('active', !isLogin);
}

function showMsg(el, text, type) {
  el.textContent = text;
  el.className = 'msg ' + type;
  el.classList.remove('hidden');
}

document.addEventListener('keydown', e => {
  if (e.key === 'Escape') closeModal();
});

loadRating();
