const express = require('express');
const router = express.Router();
const { loadTrains, loadBookings, loadLogs } = require('../utils/fileParser');

router.get('/revenue', (req, res) => {
  try {
    const trains = loadTrains(), bookings = loadBookings();
    let totalRevenue = 0;
    const trainRevenues = trains.map(train => {
      let revenue = 0, bookingCount = 0;
      bookings.forEach(b => {
        if (b.trainNumber === train.trainNumber && b.status === 'CONFIRMED') { revenue += b.fare; bookingCount++; }
      });
      totalRevenue += revenue;
      return { trainNumber: train.trainNumber, trainName: train.name, bookingCount, revenue: Math.round(revenue * 100) / 100 };
    });
    res.json({
      trainRevenues, totalRevenue: Math.round(totalRevenue * 100) / 100,
      totalConfirmed: bookings.filter(b => b.status === 'CONFIRMED').length,
      totalCancelled: bookings.filter(b => b.status === 'CANCELLED').length,
    });
  } catch (err) { res.status(500).json({ error: err.message }); }
});

router.get('/occupancy', (req, res) => {
  try {
    res.json(loadTrains().map(t => ({
      trainNumber: t.trainNumber, trainName: t.name, route: `${t.source} → ${t.destination}`,
      totalSeats: t.totalSeats, bookedSeats: t.bookedSeats.length,
      availableSeats: t.totalSeats - t.bookedSeats.length,
      occupancyPercent: t.totalSeats > 0 ? Math.round((t.bookedSeats.length / t.totalSeats) * 1000) / 10 : 0,
    })));
  } catch (err) { res.status(500).json({ error: err.message }); }
});

router.get('/logs', (req, res) => {
  try { res.json(loadLogs().reverse()); }
  catch (err) { res.status(500).json({ error: err.message }); }
});

module.exports = router;
