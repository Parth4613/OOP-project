const express = require('express');
const router = express.Router();
const { loadUsers, saveUsers, appendLog } = require('../utils/fileParser');
const { hashPassword, verifyPassword } = require('../utils/auth');

router.post('/login', (req, res) => {
  try {
    const { username, password } = req.body;
    if (!username || !password) return res.status(400).json({ error: 'Username and password are required' });
    const users = loadUsers();
    const user = users.find(u => u.username === username);
    if (!user || !verifyPassword(password, user.passwordHash)) {
      appendLog('LOGIN', `Failed login attempt: ${username}`);
      return res.status(401).json({ error: 'Invalid username or password' });
    }
    appendLog('LOGIN', `Successful login: ${username} (${user.role})`);
    res.json({ username: user.username, role: user.role });
  } catch (err) { res.status(500).json({ error: err.message }); }
});

router.post('/register', (req, res) => {
  try {
    const { username, password } = req.body;
    if (!username || !password) return res.status(400).json({ error: 'Username and password are required' });
    const users = loadUsers();
    if (users.find(u => u.username === username))
      return res.status(409).json({ error: `Username '${username}' already exists` });
    users.push({ username, passwordHash: hashPassword(password), role: 'passenger' });
    saveUsers(users);
    appendLog('REGISTER', `New passenger registered: ${username}`);
    res.status(201).json({ message: 'Registration successful', username, role: 'passenger' });
  } catch (err) { res.status(500).json({ error: err.message }); }
});

module.exports = router;
