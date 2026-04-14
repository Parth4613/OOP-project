const express = require('express');
const router = express.Router();
const { loadTrains, saveTrains, loadBookings, saveBookings, appendLog } = require('../utils/fileParser');

function calculateFare(baseFare, trainType, isTatkal = false) {
  let fare = baseFare;
  if (trainType === 'AC') fare *= 1.8;
  if (isTatkal) fare *= 1.3;
  return Math.round(fare * 100) / 100;
}

function calculateRefund(fare, depDate, depTime) {
  const departure = new Date(`${depDate}T${depTime}:00`);
  const now = new Date();
  const hoursLeft = (departure - now) / (1000 * 60 * 60);
  let refundPercent = hoursLeft > 24 ? 0.90 : hoursLeft > 12 ? 0.70 : 0.50;
  return {
    refundAmount: Math.round(fare * refundPercent * 100) / 100,
    refundPercent: Math.round(refundPercent * 100),
    hoursLeft: Math.round(hoursLeft * 10) / 10,
  };
}

function generateTicketId() {
  const now = Math.floor(Date.now() / 1000);
  return `TRN${now % 100000000}${Math.floor(Math.random() * 10000)}`;
}

router.post('/', (req, res) => {
  try {
    const { username, trainNumber, isTatkal = false, paymentMethod = 'CARD' } = req.body;
    if (!username || !trainNumber) return res.status(400).json({ error: 'username and trainNumber required' });
    const trains = loadTrains();
    const train = trains.find(t => t.trainNumber === trainNumber);
    if (!train) return res.status(404).json({ error: 'Train not found' });
    const departure = new Date(`${train.departureDate}T${train.departureTime}:00`);
    if (departure <= new Date()) return res.status(400).json({ error: 'Train departure time has already passed' });
    const bookedNums = new Set(train.bookedSeats.map(s => s.seatNumber));
    let seatNum = -1;
    for (let i = 1; i <= train.totalSeats; i++) { if (!bookedNums.has(i)) { seatNum = i; break; } }
    if (seatNum === -1) return res.status(409).json({ error: 'No seats available' });
    const fare = calculateFare(train.baseFare, train.type, isTatkal);
    const ticketId = generateTicketId();
    const now = Math.floor(Date.now() / 1000);
    train.bookedSeats.push({ seatNumber: seatNum, bookedBy: username });
    train.availableSeats = train.totalSeats - train.bookedSeats.length;
    saveTrains(trains);
    const booking = {
      ticketId, passengerName: username, trainNumber, trainName: train.name,
      seatNumber: seatNum, fare, status: 'CONFIRMED', bookingTime: now,
      paymentId: ticketId, paymentAmount: fare, paymentMethod,
      paymentStatus: 'COMPLETED', refundAmount: 0,
    };
    const bookings = loadBookings();
    bookings.push(booking);
    saveBookings(bookings);
    appendLog('BOOKING', `Ticket ${ticketId} booked by ${username} on train ${trainNumber} seat ${seatNum}`);
    res.status(201).json(booking);
  } catch (err) { res.status(500).json({ error: err.message }); }
});

router.delete('/:ticketId', (req, res) => {
  try {
    const { ticketId } = req.params;
    const { username } = req.body;
    if (!username) return res.status(400).json({ error: 'username required' });
    const bookings = loadBookings();
    const booking = bookings.find(b => b.ticketId === ticketId && b.passengerName === username);
    if (!booking) return res.status(404).json({ error: 'Ticket not found or unauthorized' });
    if (booking.status === 'CANCELLED') return res.status(400).json({ error: 'Already cancelled' });
    const trains = loadTrains();
    const train = trains.find(t => t.trainNumber === booking.trainNumber);
    let refundInfo;
    if (train) {
      refundInfo = calculateRefund(booking.fare, train.departureDate, train.departureTime);
      train.bookedSeats = train.bookedSeats.filter(s => s.seatNumber !== booking.seatNumber);
      train.availableSeats = train.totalSeats - train.bookedSeats.length;
      saveTrains(trains);
    } else {
      refundInfo = { refundAmount: booking.fare * 0.50, refundPercent: 50, hoursLeft: 0 };
    }
    booking.status = 'CANCELLED';
    booking.paymentStatus = 'REFUNDED';
    booking.refundAmount = refundInfo.refundAmount;
    saveBookings(bookings);
    appendLog('CANCEL', `Ticket ${ticketId} cancelled by ${username}. Refund: Rs. ${refundInfo.refundAmount}`);
    res.json({ ...booking, ...refundInfo });
  } catch (err) { res.status(500).json({ error: err.message }); }
});

router.get('/history/:username', (req, res) => {
  try { res.json(loadBookings().filter(b => b.passengerName === req.params.username)); }
  catch (err) { res.status(500).json({ error: err.message }); }
});

router.get('/', (req, res) => {
  try { res.json(loadBookings()); }
  catch (err) { res.status(500).json({ error: err.message }); }
});

module.exports = router;
