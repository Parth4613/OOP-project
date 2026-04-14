const fs = require('fs');
const path = require('path');

const DATA_DIR = path.join(__dirname, '..', '..', 'data');

function ensureDataDir() {
  if (!fs.existsSync(DATA_DIR)) fs.mkdirSync(DATA_DIR, { recursive: true });
}

function readLines(filename) {
  const filePath = path.join(DATA_DIR, filename);
  if (!fs.existsSync(filePath)) return [];
  return fs.readFileSync(filePath, 'utf-8').split('\n').filter(l => l.trim().length > 0);
}

function writeLines(filename, lines) {
  ensureDataDir();
  fs.writeFileSync(path.join(DATA_DIR, filename), lines.join('\n') + '\n', 'utf-8');
}

function appendLine(filename, line) {
  ensureDataDir();
  fs.appendFileSync(path.join(DATA_DIR, filename), line + '\n', 'utf-8');
}

function parseTrain(line) {
  const p = line.split('|');
  if (p.length < 10) return null;
  const bookedSeats = [];
  if (p.length > 10 && p[10]) {
    p[10].split(',').forEach(b => {
      const info = b.split(':');
      if (info.length === 2) bookedSeats.push({ seatNumber: parseInt(info[0]), bookedBy: info[1] });
    });
  }
  return {
    trainNumber: p[0], name: p[1], source: p[2], destination: p[3],
    type: p[4], totalSeats: parseInt(p[5]), baseFare: parseFloat(p[6]),
    departureDate: p[7], departureTime: p[8], arrivalTime: p[9],
    bookedSeats, availableSeats: parseInt(p[5]) - bookedSeats.length,
  };
}

function serializeTrain(t) {
  let line = [t.trainNumber, t.name, t.source, t.destination, t.type, t.totalSeats, t.baseFare,
    t.departureDate, t.departureTime, t.arrivalTime].join('|') + '|';
  if (t.bookedSeats && t.bookedSeats.length > 0)
    line += t.bookedSeats.map(b => `${b.seatNumber}:${b.bookedBy}`).join(',');
  return line;
}

function loadTrains() { return readLines('trains.txt').map(parseTrain).filter(Boolean); }
function saveTrains(trains) { writeLines('trains.txt', trains.map(serializeTrain)); }

function parseBooking(line) {
  const p = line.split('|');
  if (p.length < 13) return null;
  return {
    ticketId: p[0], passengerName: p[1], trainNumber: p[2], trainName: p[3],
    seatNumber: parseInt(p[4]), fare: parseFloat(p[5]), status: p[6],
    bookingTime: parseInt(p[7]), paymentId: p[8], paymentAmount: parseFloat(p[9]),
    paymentMethod: p[10], paymentStatus: p[11], refundAmount: parseFloat(p[12]),
  };
}

function serializeBooking(b) {
  return [b.ticketId, b.passengerName, b.trainNumber, b.trainName, b.seatNumber,
    b.fare, b.status, b.bookingTime, b.paymentId, b.paymentAmount,
    b.paymentMethod, b.paymentStatus, b.refundAmount].join('|');
}

function loadBookings() { return readLines('tickets.txt').map(parseBooking).filter(Boolean); }
function saveBookings(bookings) { writeLines('tickets.txt', bookings.map(serializeBooking)); }

function loadUsers() {
  return readLines('users.txt').map(line => {
    const p = line.split('|');
    return p.length >= 3 ? { username: p[0], passwordHash: p[1], role: p[2] } : null;
  }).filter(Boolean);
}

function saveUsers(users) {
  writeLines('users.txt', users.map(u => `${u.username}|${u.passwordHash}|${u.role}`));
}

function loadLogs() { return readLines('logs.txt'); }

function appendLog(eventType, message) {
  const now = new Date();
  const ts = now.getFullYear() + '-' + String(now.getMonth()+1).padStart(2,'0') + '-' +
    String(now.getDate()).padStart(2,'0') + ' ' + String(now.getHours()).padStart(2,'0') + ':' +
    String(now.getMinutes()).padStart(2,'0') + ':' + String(now.getSeconds()).padStart(2,'0');
  appendLine('logs.txt', `[${ts}] [${eventType}] ${message}`);
}

module.exports = {
  readLines, writeLines, appendLine,
  parseTrain, serializeTrain, loadTrains, saveTrains,
  parseBooking, serializeBooking, loadBookings, saveBookings,
  loadUsers, saveUsers, loadLogs, appendLog,
};
