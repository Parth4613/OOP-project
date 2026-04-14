import React, { useState, useEffect } from 'react';
import { Link } from 'react-router-dom';
import { Train, Search, Calendar, TrendingUp, Users, DollarSign } from 'lucide-react';
import { apiService } from '../services/api';

const Dashboard = ({ user }) => {
  const [stats, setStats] = useState({
    totalTrains: 0,
    totalBookings: 0,
    todayBookings: 0,
    revenue: 0,
  });
  const [recentBookings, setRecentBookings] = useState([]);
  const [loading, setLoading] = useState(true);

  useEffect(() => {
    fetchDashboardData();
  }, []);

  const fetchDashboardData = async () => {
    try {
      const [statsData, bookingsData] = await Promise.all([
        apiService.getBookingStats(),
        user?.role === 'admin' ? apiService.getAllBookings() : apiService.getBookings(user.username)
      ]);
      
      setStats({
        totalTrains: statsData.trainRevenues?.length || 0,
        totalBookings: statsData.totalConfirmed || 0,
        todayBookings: 0, // Backend doesn't provide this, would need date filtering
        revenue: statsData.totalRevenue || 0,
      });
      setRecentBookings(bookingsData.slice(0, 5));
    } catch (error) {
      console.error('Error fetching dashboard data:', error);
    } finally {
      setLoading(false);
    }
  };

  if (loading) {
    return <div className="loading">Loading dashboard...</div>;
  }

  return (
    <div>
      <div style={{ marginBottom: '32px' }}>
        <h1 style={{ fontSize: '32px', fontWeight: '600', marginBottom: '8px' }}>
          Welcome back, {user?.username}!
        </h1>
        <p style={{ color: 'rgba(255, 255, 255, 0.7)' }}>
          {user?.role === 'admin' ? 'Manage your railway system' : 'Manage your train bookings'}
        </p>
      </div>

      <div className="grid grid-3" style={{ marginBottom: '32px' }}>
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

        {user?.role === 'admin' && (
          <div className="card">
            <div style={{ display: 'flex', alignItems: 'center', justifyContent: 'space-between' }}>
              <div>
                <p style={{ color: 'rgba(255, 255, 255, 0.7)', fontSize: '14px', marginBottom: '4px' }}>
                  Total Revenue
                </p>
              <p style={{ fontSize: '24px', fontWeight: '600' }}>${stats.revenue}</p>
              </div>
              <DollarSign size={32} color="#ffd43b" />
            </div>
          </div>
        )}
      </div>

      <div className="grid grid-2">
        <div className="card">
          <h3 style={{ marginBottom: '20px', fontSize: '18px', fontWeight: '500' }}>
            Quick Actions
          </h3>
          <div style={{ display: 'flex', flexDirection: 'column', gap: '12px' }}>
            <Link to="/trains" className="button" style={{ textDecoration: 'none', display: 'flex', alignItems: 'center', gap: '8px' }}>
              <Search size={16} />
              Search Trains
            </Link>
            <Link to="/bookings" className="button" style={{ textDecoration: 'none', display: 'flex', alignItems: 'center', gap: '8px' }}>
              <Calendar size={16} />
              View Bookings
            </Link>
            {user?.role === 'admin' && (
              <Link to="/admin" className="button" style={{ textDecoration: 'none', display: 'flex', alignItems: 'center', gap: '8px' }}>
                <Users size={16} />
                Admin Panel
              </Link>
            )}
          </div>
        </div>

        <div className="card">
          <h3 style={{ marginBottom: '20px', fontSize: '18px', fontWeight: '500' }}>
            Recent Bookings
          </h3>
          {recentBookings.length > 0 ? (
            <div style={{ display: 'flex', flexDirection: 'column', gap: '12px' }}>
              {recentBookings.map((booking) => (
                <div 
                  key={booking.ticketId} 
                  style={{ 
                    padding: '12px', 
                    background: 'rgba(255, 255, 255, 0.05)', 
                    borderRadius: '8px',
                    border: '1px solid rgba(255, 255, 255, 0.1)'
                  }}
                >
                  <div style={{ display: 'flex', justifyContent: 'space-between', alignItems: 'center' }}>
                    <div>
                      <p style={{ fontWeight: '500', marginBottom: '4px' }}>
                        {booking.trainName}
                      </p>
                      <p style={{ fontSize: '14px', color: 'rgba(255, 255, 255, 0.7)' }}>
                        Seat {booking.seatNumber} • Train #{booking.trainNumber}
                      </p>
                    </div>
                    <div style={{ textAlign: 'right' }}>
                      <p style={{ fontWeight: '500' }}>${booking.fare}</p>
                      <p style={{ fontSize: '12px', color: '#51cf66' }}>
                        {booking.status}
                      </p>
                    </div>
                  </div>
                </div>
              ))}
            </div>
          ) : (
            <p style={{ color: 'rgba(255, 255, 255, 0.7)', textAlign: 'center' }}>
              No recent bookings
            </p>
          )}
        </div>
      </div>
    </div>
  );
};

export default Dashboard;
