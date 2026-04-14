import React, { useState, useEffect } from 'react';
import { Search, Filter, Train, MapPin, Calendar, Clock, Users } from 'lucide-react';
import { apiService } from '../services/api';

const TrainSearch = () => {
  const [trains, setTrains] = useState([]);
  const [filteredTrains, setFilteredTrains] = useState([]);
  const [searchForm, setSearchForm] = useState({
    from: '',
    to: '',
    date: '',
  });
  const [loading, setLoading] = useState(false);
  const [error, setError] = useState('');

  useEffect(() => {
    fetchTrains();
  }, []);

  const fetchTrains = async () => {
    try {
      const data = await apiService.getTrains();
      setTrains(data);
      setFilteredTrains(data);
    } catch (error) {
      console.error('Error fetching trains:', error);
      setError('Failed to fetch trains');
    }
  };

  const handleSearch = async (e) => {
    e.preventDefault();
    setLoading(true);
    setError('');

    try {
      if (searchForm.from && searchForm.to && searchForm.date) {
        const data = await apiService.searchTrains(searchForm.from, searchForm.to, searchForm.date);
        setFilteredTrains(data);
      } else {
        // Filter locally if not all search criteria are provided
        let filtered = trains;
        
        if (searchForm.from) {
          filtered = filtered.filter(train => 
            train.source.toLowerCase().includes(searchForm.from.toLowerCase())
          );
        }
        
        if (searchForm.to) {
          filtered = filtered.filter(train => 
            train.destination.toLowerCase().includes(searchForm.to.toLowerCase())
          );
        }
        
        setFilteredTrains(filtered);
      }
    } catch (error) {
      setError(error.message || 'Search failed');
    } finally {
      setLoading(false);
    }
  };

  const handleInputChange = (e) => {
    setSearchForm({
      ...searchForm,
      [e.target.name]: e.target.value,
    });
  };

  const handleBookTrain = async (trainNumber) => {
    try {
      const userData = JSON.parse(localStorage.getItem('user'));
      const bookingData = {
        username: userData.username,
        trainNumber,
      };
      
      const booking = await apiService.createBooking(bookingData);
      alert('Booking successful! Ticket ID: ' + booking.ticketId);
      // Refresh trains to update available seats
      fetchTrains();
    } catch (error) {
      alert('Booking failed: ' + error.message);
    }
  };

  return (
    <div>
      <h1 style={{ fontSize: '32px', fontWeight: '600', marginBottom: '32px' }}>
        Search Trains
      </h1>

      <div className="card" style={{ marginBottom: '32px' }}>
        <form onSubmit={handleSearch}>
          <div className="grid grid-3">
            <div className="form-group">
              <label htmlFor="from">From</label>
              <div style={{ position: 'relative' }}>
                <MapPin 
                  size={20} 
                  style={{ position: 'absolute', left: '12px', top: '50%', transform: 'translateY(-50%)', color: 'rgba(255, 255, 255, 0.5)' }}
                />
                <input
                  type="text"
                  id="from"
                  name="from"
                  value={searchForm.from}
                  onChange={handleInputChange}
                  className="input"
                  placeholder="Departure city"
                  style={{ paddingLeft: '44px' }}
                />
              </div>
            </div>

            <div className="form-group">
              <label htmlFor="to">To</label>
              <div style={{ position: 'relative' }}>
                <MapPin 
                  size={20} 
                  style={{ position: 'absolute', left: '12px', top: '50%', transform: 'translateY(-50%)', color: 'rgba(255, 255, 255, 0.5)' }}
                />
                <input
                  type="text"
                  id="to"
                  name="to"
                  value={searchForm.to}
                  onChange={handleInputChange}
                  className="input"
                  placeholder="Arrival city"
                  style={{ paddingLeft: '44px' }}
                />
              </div>
            </div>

            <div className="form-group">
              <label htmlFor="date">Date</label>
              <div style={{ position: 'relative' }}>
                <Calendar 
                  size={20} 
                  style={{ position: 'absolute', left: '12px', top: '50%', transform: 'translateY(-50%)', color: 'rgba(255, 255, 255, 0.5)' }}
                />
                <input
                  type="date"
                  id="date"
                  name="date"
                  value={searchForm.date}
                  onChange={handleInputChange}
                  className="input"
                  style={{ paddingLeft: '44px' }}
                />
              </div>
            </div>
          </div>

          {error && <div className="error">{error}</div>}

          <button 
            type="submit" 
            className="button" 
            disabled={loading}
            style={{ display: 'flex', alignItems: 'center', gap: '8px' }}
          >
            <Search size={16} />
            {loading ? 'Searching...' : 'Search Trains'}
          </button>
        </form>
      </div>

      <div>
        <h2 style={{ fontSize: '24px', fontWeight: '500', marginBottom: '20px' }}>
          Available Trains ({filteredTrains.length})
        </h2>

        {filteredTrains.length > 0 ? (
          <div className="grid grid-2">
            {filteredTrains.map((train) => (
              <div key={train.trainNumber} className="card">
                <div style={{ display: 'flex', alignItems: 'center', gap: '12px', marginBottom: '16px' }}>
                  <Train size={24} color="#667eea" />
                  <div>
                    <h3 style={{ fontSize: '18px', fontWeight: '500', marginBottom: '4px' }}>
                      {train.name}
                    </h3>
                    <p style={{ fontSize: '14px', color: 'rgba(255, 255, 255, 0.7)' }}>
                      Train #{train.trainNumber}
                    </p>
                  </div>
                </div>

                <div style={{ marginBottom: '16px' }}>
                  <div style={{ display: 'flex', justifyContent: 'space-between', alignItems: 'center', marginBottom: '8px' }}>
                    <div style={{ display: 'flex', alignItems: 'center', gap: '8px' }}>
                      <MapPin size={16} color="#667eea" />
                      <span>{train.source}</span>
                    </div>
                    <div style={{ display: 'flex', alignItems: 'center', gap: '8px' }}>
                      <MapPin size={16} color="#764ba2" />
                      <span>{train.destination}</span>
                    </div>
                  </div>
                  
                  <div style={{ display: 'flex', justifyContent: 'space-between', alignItems: 'center' }}>
                    <div style={{ display: 'flex', alignItems: 'center', gap: '8px' }}>
                      <Clock size={16} color="rgba(255, 255, 255, 0.5)" />
                      <span style={{ fontSize: '14px' }}>{train.departureTime} - {train.arrivalTime}</span>
                    </div>
                    <div style={{ display: 'flex', alignItems: 'center', gap: '8px' }}>
                      <Users size={16} color="rgba(255, 255, 255, 0.5)" />
                      <span style={{ fontSize: '14px' }}>{train.availableSeats} seats</span>
                    </div>
                  </div>
                </div>

                <div style={{ display: 'flex', justifyContent: 'space-between', alignItems: 'center' }}>
                  <div>
                    <p style={{ fontSize: '20px', fontWeight: '600', color: '#51cf66' }}>
                      ${train.baseFare}
                    </p>
                    <p style={{ fontSize: '12px', color: 'rgba(255, 255, 255, 0.7)' }}>
                      per person
                    </p>
                  </div>
                  
                  <button 
                    className="button"
                    onClick={() => handleBookTrain(train.trainNumber)}
                    disabled={train.availableSeats === 0}
                    style={{ padding: '8px 16px', fontSize: '14px' }}
                  >
                    {train.availableSeats === 0 ? 'Sold Out' : 'Book Now'}
                  </button>
                </div>
              </div>
            ))}
          </div>
        ) : (
          <div className="card text-center">
            <Train size={48} color="rgba(255, 255, 255, 0.3)" style={{ marginBottom: '16px' }} />
            <p style={{ color: 'rgba(255, 255, 255, 0.7)' }}>
              No trains found matching your criteria
            </p>
          </div>
        )}
      </div>
    </div>
  );
};

export default TrainSearch;
