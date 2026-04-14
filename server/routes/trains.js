const express = require('express');
const router = express.Router();
const { loadTrains, saveTrains, appendLog } = require('../utils/fileParser');

router.get('/', (req, res) => {
  try { res.json(loadTrains()); }
  catch (err) { res.status(500).json({ error: err.message }); }
});

router.get('/search', (req, res) => {
  try {
    const { source, destination, date } = req.query;
    if (!source || !destination || !date) return res.status(400).json({ error: 'source, destination, and date required' });
    const results = loadTrains().filter(t =>
      t.source.toLowerCase() === source.toLowerCase() &&
      t.destination.toLowerCase() === destination.toLowerCase() &&
      t.departureDate === date);
    res.json(results);
  } catch (err) { res.status(500).json({ error: err.message }); }
});

router.get('/search/advanced', (req, res) => {
  try {
    const { source, destination, date, type, minPrice, maxPrice } = req.query;
    if (!source || !destination || !date) return res.status(400).json({ error: 'source, destination, and date required' });
    let trains = loadTrains().filter(t =>
      t.source.toLowerCase() === source.toLowerCase() &&
      t.destination.toLowerCase() === destination.toLowerCase() &&
      t.departureDate === date);
    if (type && type !== 'ALL') trains = trains.filter(t => t.type === type);
    if (minPrice) trains = trains.filter(t => t.baseFare >= parseFloat(minPrice));
    if (maxPrice) trains = trains.filter(t => t.baseFare <= parseFloat(maxPrice));
    res.json(trains);
  } catch (err) { res.status(500).json({ error: err.message }); }
});

router.post('/', (req, res) => {
  try {
    const { trainNumber, name, source, destination, type, totalSeats, baseFare,
            departureDate, departureTime, arrivalTime } = req.body;
    if (!trainNumber || !name || !source || !destination)
      return res.status(400).json({ error: 'Missing required train fields' });
    const trains = loadTrains();
    if (trains.find(t => t.trainNumber === trainNumber))
      return res.status(409).json({ error: 'Train number already exists' });
    const newTrain = {
      trainNumber, name, source, destination, type: type || 'Sleeper',
      totalSeats: parseInt(totalSeats) || 50, baseFare: parseFloat(baseFare) || 500,
      departureDate, departureTime, arrivalTime, bookedSeats: [],
      availableSeats: parseInt(totalSeats) || 50,
    };
    trains.push(newTrain);
    saveTrains(trains);
    appendLog('ADMIN', `Train ${trainNumber} added`);
    res.status(201).json(newTrain);
  } catch (err) { res.status(500).json({ error: err.message }); }
});

router.delete('/:trainNumber', (req, res) => {
  try {
    const { trainNumber } = req.params;
    let trains = loadTrains();
    const idx = trains.findIndex(t => t.trainNumber === trainNumber);
    if (idx === -1) return res.status(404).json({ error: 'Train not found' });
    trains.splice(idx, 1);
    saveTrains(trains);
    appendLog('ADMIN', `Train ${trainNumber} deleted`);
    res.json({ message: `Train ${trainNumber} deleted` });
  } catch (err) { res.status(500).json({ error: err.message }); }
});

router.put('/:trainNumber/schedule', (req, res) => {
  try {
    const { trainNumber } = req.params;
    const { departureDate, departureTime, arrivalTime } = req.body;
    const trains = loadTrains();
    const train = trains.find(t => t.trainNumber === trainNumber);
    if (!train) return res.status(404).json({ error: 'Train not found' });
    if (departureDate) train.departureDate = departureDate;
    if (departureTime) train.departureTime = departureTime;
    if (arrivalTime) train.arrivalTime = arrivalTime;
    saveTrains(trains);
    appendLog('ADMIN', `Schedule modified for train ${trainNumber}`);
    res.json(train);
  } catch (err) { res.status(500).json({ error: err.message }); }
});

router.put('/:trainNumber/capacity', (req, res) => {
  try {
    const { trainNumber } = req.params;
    const { totalSeats } = req.body;
    const trains = loadTrains();
    const train = trains.find(t => t.trainNumber === trainNumber);
    if (!train) return res.status(404).json({ error: 'Train not found' });
    train.totalSeats = parseInt(totalSeats);
    train.bookedSeats = [];
    train.availableSeats = parseInt(totalSeats);
    saveTrains(trains);
    appendLog('ADMIN', `Capacity updated for train ${trainNumber}`);
    res.json(train);
  } catch (err) { res.status(500).json({ error: err.message }); }
});

module.exports = router;
