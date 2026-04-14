const express = require('express');
const cors = require('cors');
const path = require('path');

const app = express();
const PORT = 5000;

app.use(cors());
app.use(express.json());

app.use('/api/auth', require('./routes/auth'));
app.use('/api/trains', require('./routes/trains'));
app.use('/api/bookings', require('./routes/bookings'));
app.use('/api/reports', require('./routes/reports'));

app.get('/api/health', (req, res) => res.json({ status: 'ok', timestamp: new Date().toISOString() }));

const frontendBuild = path.join(__dirname, '..', 'frontend', 'dist');
app.use(express.static(frontendBuild));
app.get('*', (req, res) => {
  const indexPath = path.join(frontendBuild, 'index.html');
  require('fs').existsSync(indexPath)
    ? res.sendFile(indexPath)
    : res.status(404).json({ error: 'Frontend not built yet' });
});

app.listen(PORT, () => {
  console.log(`\n  ✦ Railway Reservation API running on http://localhost:${PORT}`);
  console.log(`  ✦ API endpoints at http://localhost:${PORT}/api\n`);
});
