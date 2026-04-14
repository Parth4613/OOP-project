import React, { useState, useEffect } from 'react';
import { Plus, Edit2, Trash2, Train, Users, DollarSign, TrendingUp, Calendar } from 'lucide-react';
import { apiService } from '../services/api';

const AdminDashboard = ({ user }) => {
  const [trains, setTrains] = useState([]);
  const [stats, setStats] = useState({
    totalTrains: 0,
    totalBookings: 0,
    totalRevenue: 0,
    todayBookings: 0,
  });
  const [showAddTrain, setShowAddTrain] = useState(false);
  const [editingTrain, setEditingTrain] = useState(null);
  const [trainForm, setTrainForm] = useState({
    name: '',
    number: '',
    from: '',
    to: '',
    departure: '',
    arrival: '',
    price: '',
    totalSeats: '',
    availableSeats: '',
  });
  const [loading, setLoading] = useState(true);
  const [error, setError] = useState('');

  useEffect(() => {
    fetchAdminData();
  }, []);

  const fetchAdminData = async () => {
    try {
      const [trainsData, statsData] = await Promise.all([
        apiService.getTrains(),
        apiService.getBookingStats()
      ]);
      setTrains(trainsData);
      setStats(statsData);
    } catch (error) {
      console.error('Error fetching admin data:', error);
      setError('Failed to fetch data');
    } finally {
      setLoading(false);
    }
  };

  const handleTrainSubmit = async (e) => {
    e.preventDefault();
    setError('');

    try {
      if (editingTrain) {
        await apiService.updateTrain(editingTrain.id, trainForm);
        setTrains(trains.map(train => 
          train.id === editingTrain.id ? { ...train, ...trainForm } : train
        ));
      } else {
        const newTrain = await apiService.addTrain(trainForm);
        setTrains([...trains, newTrain]);
      }

      resetTrainForm();
    } catch (error) {
      setError(error.message || 'Failed to save train');
    }
  };

  const handleEditTrain = (train) => {
    setEditingTrain(train);
    setTrainForm({
      name: train.name,
      number: train.number,
      from: train.from,
      to: train.to,
      departure: train.departure,
      arrival: train.arrival,
      price: train.price,
      totalSeats: train.totalSeats,
      availableSeats: train.availableSeats,
    });
    setShowAddTrain(true);
  };

  const handleDeleteTrain = async (trainId) => {
    if (!window.confirm('Are you sure you want to delete this train?')) {
      return;
    }

    try {
      await apiService.deleteTrain(trainId);
      setTrains(trains.filter(train => train.id !== trainId));
    } catch (error) {
      alert('Failed to delete train: ' + error.message);
    }
  };

  const resetTrainForm = () => {
    setTrainForm({
      name: '',
      number: '',
      from: '',
      to: '',
      departure: '',
      arrival: '',
      price: '',
      totalSeats: '',
      availableSeats: '',
    });
    setEditingTrain(null);
    setShowAddTrain(false);
  };

  const handleInputChange = (e) => {
    setTrainForm({
      ...trainForm,
      [e.target.name]: e.target.value,
    });
  };

  if (loading) {
    return <div className="loading">Loading admin dashboard...</div>;
  }

  return (
    <div>
      <h1 style={{ fontSize: '32px', fontWeight: '600', marginBottom: '32px' }}>
        Admin Dashboard
      </h1>

      <div className="grid grid-4" style={{ marginBottom: '32px' }}>
        <div className="card">
          <div style={{ display: 'flex', alignItems: 'center', justifyContent: 'space-between' }}>
            <div>
              <p style={{ color: 'rgba(255, 255, 255, 0.7)', fontSize: '14px', marginBottom: '4px' }}>
                Total Trains
              </p>
              <p style={{ fontSize: '24px', fontWeight: '600' }}>{stats.totalTrains}</p>
            </div>
            <Train size={32} color="#667eea" />
          </div>
        </div>

        <div className="card">
          <div style={{ display: 'flex', alignItems: 'center', justifyContent: 'space-between' }}>
            <div>
              <p style={{ color: 'rgba(255, 255, 255, 0.7)', fontSize: '14px', marginBottom: '4px' }}>
                Total Bookings
              </p>
              <p style={{ fontSize: '24px', fontWeight: '600' }}>{stats.totalBookings}</p>
            </div>
            <Calendar size={32} color="#764ba2" />
          </div>
        </div>

        <div className="card">
          <div style={{ display: 'flex', alignItems: 'center', justifyContent: 'space-between' }}>
            <div>
              <p style={{ color: 'rgba(255, 255, 255, 0.7)', fontSize: '14px', marginBottom: '4px' }}>
                Today's Bookings
              </p>
              <p style={{ fontSize: '24px', fontWeight: '600' }}>{stats.todayBookings}</p>
            </div>
            <TrendingUp size={32} color="#51cf66" />
          </div>
        </div>

        <div className="card">
          <div style={{ display: 'flex', alignItems: 'center', justifyContent: 'space-between' }}>
            <div>
              <p style={{ color: 'rgba(255, 255, 255, 0.7)', fontSize: '14px', marginBottom: '4px' }}>
                Total Revenue
              </p>
              <p style={{ fontSize: '24px', fontWeight: '600' }}>${stats.totalRevenue}</p>
            </div>
            <DollarSign size={32} color="#ffd43b" />
          </div>
        </div>
      </div>

      <div className="card" style={{ marginBottom: '32px' }}>
        <div style={{ display: 'flex', justifyContent: 'space-between', alignItems: 'center', marginBottom: '24px' }}>
          <h2 style={{ fontSize: '24px', fontWeight: '500' }}>
            Train Management
          </h2>
          <button 
            className="button"
            onClick={() => setShowAddTrain(true)}
            style={{ display: 'flex', alignItems: 'center', gap: '8px' }}
          >
            <Plus size={16} />
            Add Train
          </button>
        </div>

        {showAddTrain && (
          <div className="card" style={{ marginBottom: '24px', background: 'rgba(255, 255, 255, 0.08)' }}>
            <h3 style={{ marginBottom: '20px' }}>
              {editingTrain ? 'Edit Train' : 'Add New Train'}
            </h3>
            <form onSubmit={handleTrainSubmit}>
              <div className="grid grid-2">
                <div className="form-group">
                  <label htmlFor="name">Train Name</label>
                  <input
                    type="text"
                    id="name"
                    name="name"
                    value={trainForm.name}
                    onChange={handleInputChange}
                    className="input"
                    required
                  />
                </div>

                <div className="form-group">
                  <label htmlFor="number">Train Number</label>
                  <input
                    type="text"
                    id="number"
                    name="number"
                    value={trainForm.number}
                    onChange={handleInputChange}
                    className="input"
                    required
                  />
                </div>

                <div className="form-group">
                  <label htmlFor="from">From</label>
                  <input
                    type="text"
                    id="from"
                    name="from"
                    value={trainForm.from}
                    onChange={handleInputChange}
                    className="input"
                    required
                  />
                </div>

                <div className="form-group">
                  <label htmlFor="to">To</label>
                  <input
                    type="text"
                    id="to"
                    name="to"
                    value={trainForm.to}
                    onChange={handleInputChange}
                    className="input"
                    required
                  />
                </div>

                <div className="form-group">
                  <label htmlFor="departure">Departure Time</label>
                  <input
                    type="time"
                    id="departure"
                    name="departure"
                    value={trainForm.departure}
                    onChange={handleInputChange}
                    className="input"
                    required
                  />
                </div>

                <div className="form-group">
                  <label htmlFor="arrival">Arrival Time</label>
                  <input
                    type="time"
                    id="arrival"
                    name="arrival"
                    value={trainForm.arrival}
                    onChange={handleInputChange}
                    className="input"
                    required
                  />
                </div>

                <div className="form-group">
                  <label htmlFor="price">Price ($)</label>
                  <input
                    type="number"
                    id="price"
                    name="price"
                    value={trainForm.price}
                    onChange={handleInputChange}
                    className="input"
                    min="0"
                    step="0.01"
                    required
                  />
                </div>

                <div className="form-group">
                  <label htmlFor="totalSeats">Total Seats</label>
                  <input
                    type="number"
                    id="totalSeats"
                    name="totalSeats"
                    value={trainForm.totalSeats}
                    onChange={handleInputChange}
                    className="input"
                    min="1"
                    required
                  />
                </div>
              </div>

              <div style={{ display: 'flex', gap: '12px', marginTop: '20px' }}>
                <button type="submit" className="button">
                  {editingTrain ? 'Update Train' : 'Add Train'}
                </button>
                <button 
                  type="button" 
                  className="button" 
                  onClick={resetTrainForm}
                  style={{ background: 'linear-gradient(135deg, #ff6b6b 0%, #ff8e53 100%)' }}
                >
                  Cancel
                </button>
              </div>
            </form>
          </div>
        )}

        {error && <div className="error">{error}</div>}

        {trains.length > 0 ? (
          <div className="grid">
            {trains.map((train) => (
              <div key={train.id} className="card">
                <div style={{ display: 'flex', justifyContent: 'space-between', alignItems: 'flex-start', marginBottom: '16px' }}>
                  <div style={{ display: 'flex', alignItems: 'center', gap: '12px' }}>
                    <Train size={24} color="#667eea" />
                    <div>
                      <h3 style={{ fontSize: '18px', fontWeight: '500', marginBottom: '4px' }}>
                        {train.name}
                      </h3>
                      <p style={{ fontSize: '14px', color: 'rgba(255, 255, 255, 0.7)' }}>
                        #{train.number}
                      </p>
                    </div>
                  </div>
                  
                  <div style={{ display: 'flex', gap: '8px' }}>
                    <button 
                      className="button"
                      onClick={() => handleEditTrain(train)}
                      style={{ padding: '8px', background: 'linear-gradient(135deg, #ffd43b 0%, #fab005 100%)' }}
                    >
                      <Edit2 size={16} />
                    </button>
                    <button 
                      className="button"
                      onClick={() => handleDeleteTrain(train.id)}
                      style={{ padding: '8px', background: 'linear-gradient(135deg, #ff6b6b 0%, #ff8e53 100%)' }}
                    >
                      <Trash2 size={16} />
                    </button>
                  </div>
                </div>

                <div style={{ marginBottom: '16px' }}>
                  <p style={{ marginBottom: '8px' }}>
                    <strong>Route:</strong> {train.from} → {train.to}
                  </p>
                  <p style={{ marginBottom: '8px' }}>
                    <strong>Time:</strong> {train.departure} - {train.arrival}
                  </p>
                  <p style={{ marginBottom: '8px' }}>
                    <strong>Price:</strong> <span style={{ color: '#51cf66' }}>${train.price}</span>
                  </p>
                  <p>
                    <strong>Seats:</strong> {train.availableSeats}/{train.totalSeats} available
                  </p>
                </div>

                <div style={{ 
                  height: '4px', 
                  background: 'rgba(255, 255, 255, 0.1)', 
                  borderRadius: '2px', 
                  overflow: 'hidden' 
                }}>
                  <div 
                    style={{ 
                      height: '100%', 
                      background: train.availableSeats > 20 ? '#51cf66' : train.availableSeats > 10 ? '#ffd43b' : '#ff6b6b',
                      width: `${(train.availableSeats / train.totalSeats) * 100}%`,
                      transition: 'width 0.3s ease'
                    }}
                  />
                </div>
              </div>
            ))}
          </div>
        ) : (
          <div style={{ textAlign: 'center', padding: '40px' }}>
            <Train size={48} color="rgba(255, 255, 255, 0.3)" style={{ marginBottom: '16px' }} />
            <p style={{ color: 'rgba(255, 255, 255, 0.7)' }}>
              No trains available. Add your first train to get started.
            </p>
          </div>
        )}
      </div>
    </div>
  );
};

export default AdminDashboard;
