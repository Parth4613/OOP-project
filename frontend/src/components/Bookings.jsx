import React, { useState, useEffect } from 'react';
import { Calendar, X, Check, Clock, Train, MapPin } from 'lucide-react';
import { apiService } from '../services/api';

const Bookings = ({ user }) => {
  const [bookings, setBookings] = useState([]);
  const [loading, setLoading] = useState(true);
  const [error, setError] = useState('');

  useEffect(() => {
    fetchBookings();
  }, []);

  const fetchBookings = async () => {
    try {
      const data = user?.role === 'admin' 
        ? await apiService.getAllBookings()
        : await apiService.getBookings(user.username);
      setBookings(data);
    } catch (error) {
      console.error('Error fetching bookings:', error);
      setError('Failed to fetch bookings');
    } finally {
      setLoading(false);
    }
  };

  const handleCancelBooking = async (ticketId) => {
    if (!window.confirm('Are you sure you want to cancel this booking?')) {
      return;
    }

    try {
      await apiService.cancelBooking(ticketId, user.username);
      setBookings(bookings.map(booking => 
        booking.ticketId === ticketId 
          ? { ...booking, status: 'CANCELLED' }
          : booking
      ));
      alert('Booking cancelled successfully');
    } catch (error) {
      alert('Failed to cancel booking: ' + error.message);
    }
  };

  const getStatusColor = (status) => {
    switch (status.toLowerCase()) {
      case 'confirmed':
        return '#51cf66';
      case 'pending':
        return '#ffd43b';
      case 'cancelled':
        return '#ff6b6b';
      default:
        return 'rgba(255, 255, 255, 0.7)';
    }
  };

  const getStatusIcon = (status) => {
    switch (status.toLowerCase()) {
      case 'confirmed':
        return <Check size={16} />;
      case 'pending':
        return <Clock size={16} />;
      case 'cancelled':
        return <X size={16} />;
      default:
        return null;
    }
  };

  if (loading) {
    return <div className="loading">Loading bookings...</div>;
  }

  return (
    <div>
      <h1 style={{ fontSize: '32px', fontWeight: '600', marginBottom: '32px' }}>
        {user?.role === 'admin' ? 'All Bookings' : 'My Bookings'}
      </h1>

      {error && <div className="error">{error}</div>}

      {bookings.length > 0 ? (
        <div className="grid">
          {bookings.map((booking) => (
            <div key={booking.ticketId} className="card">
              <div style={{ display: 'flex', justifyContent: 'space-between', alignItems: 'flex-start', marginBottom: '16px' }}>
                <div style={{ display: 'flex', alignItems: 'center', gap: '12px' }}>
                  <Train size={24} color="#667eea" />
                  <div>
                    <h3 style={{ fontSize: '18px', fontWeight: '500', marginBottom: '4px' }}>
                      {booking.trainName}
                    </h3>
                    <p style={{ fontSize: '14px', color: 'rgba(255, 255, 255, 0.7)' }}>
                      Ticket ID: #{booking.ticketId}
                    </p>
                  </div>
                </div>
                
                <div style={{ display: 'flex', alignItems: 'center', gap: '8px', padding: '4px 12px', background: 'rgba(255, 255, 255, 0.1)', borderRadius: '20px' }}>
                  <span style={{ color: getStatusColor(booking.status) }}>
                    {getStatusIcon(booking.status)}
                  </span>
                  <span style={{ fontSize: '14px', color: getStatusColor(booking.status), fontWeight: '500' }}>
                    {booking.status}
                  </span>
                </div>
              </div>

              <div style={{ marginBottom: '16px' }}>
                <div style={{ display: 'flex', alignItems: 'center', gap: '8px', marginBottom: '8px' }}>
                  <MapPin size={16} color="#667eea" />
                  <span style={{ fontSize: '16px' }}>Train #{booking.trainNumber}</span>
                </div>
                
                <div style={{ display: 'flex', alignItems: 'center', gap: '8px', marginBottom: '8px' }}>
                  <Calendar size={16} color="rgba(255, 255, 255, 0.5)" />
                  <span style={{ fontSize: '14px', color: 'rgba(255, 255, 255, 0.7)' }}>
                    Seat: {booking.seatNumber}
                  </span>
                </div>

                <div style={{ display: 'flex', alignItems: 'center', gap: '8px' }}>
                  <Clock size={16} color="rgba(255, 255, 255, 0.5)" />
                  <span style={{ fontSize: '14px', color: 'rgba(255, 255, 255, 0.7)' }}>
                    Booked: {new Date(booking.bookingTime * 1000).toLocaleString()}
                  </span>
                </div>
              </div>

              <div style={{ display: 'flex', justifyContent: 'space-between', alignItems: 'center' }}>
                <div>
                  <p style={{ fontSize: '20px', fontWeight: '600', color: '#51cf66' }}>
                    ${booking.fare}
                  </p>
                  <p style={{ fontSize: '12px', color: 'rgba(255, 255, 255, 0.7)' }}>
                    {booking.paymentMethod} • {booking.paymentStatus}
                  </p>
                </div>
                
                {booking.status === 'CONFIRMED' && (
                  <button 
                    className="button"
                    onClick={() => handleCancelBooking(booking.ticketId)}
                    style={{ padding: '8px 16px', fontSize: '14px', background: 'linear-gradient(135deg, #ff6b6b 0%, #ff8e53 100%)' }}
                  >
                    Cancel
                  </button>
                )}
              </div>

              {user?.role === 'admin' && (
                <div style={{ marginTop: '16px', paddingTop: '16px', borderTop: '1px solid rgba(255, 255, 255, 0.1)' }}>
                  <p style={{ fontSize: '14px', color: 'rgba(255, 255, 255, 0.7)' }}>
                    Passenger: {booking.passengerName}
                  </p>
                </div>
              )}
            </div>
          ))}
        </div>
      ) : (
        <div className="card text-center">
          <Calendar size={48} color="rgba(255, 255, 255, 0.3)" style={{ marginBottom: '16px' }} />
          <p style={{ color: 'rgba(255, 255, 255, 0.7)' }}>
            {user?.role === 'admin' ? 'No bookings found' : 'You have no bookings yet'}
          </p>
        </div>
      )}
    </div>
  );
};

export default Bookings;
