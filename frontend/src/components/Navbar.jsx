import React from 'react';
import { Link, useNavigate } from 'react-router-dom';
import { Train, LogOut, User, Settings, Home, Search, Calendar } from 'lucide-react';

const Navbar = ({ user, onLogout }) => {
  const navigate = useNavigate();

  const handleLogout = () => {
    onLogout();
    navigate('/login');
  };

  return (
    <nav className="navbar">
      <div className="container">
        <div className="navbar-content">
          <Link to="/dashboard" style={{ textDecoration: 'none', display: 'flex', alignItems: 'center', gap: '8px' }}>
            <Train size={24} color="#667eea" />
            <h1>RailReserve</h1>
          </Link>
          
          <div className="navbar-actions">
            <Link to="/dashboard" style={{ color: '#ffffff', textDecoration: 'none' }}>
              <Home size={20} />
            </Link>
            <Link to="/trains" style={{ color: '#ffffff', textDecoration: 'none' }}>
              <Search size={20} />
            </Link>
            <Link to="/bookings" style={{ color: '#ffffff', textDecoration: 'none' }}>
              <Calendar size={20} />
            </Link>
            
            {user?.role === 'admin' && (
              <Link to="/admin" style={{ color: '#ffffff', textDecoration: 'none' }}>
                <Settings size={20} />
              </Link>
            )}
            
            <div style={{ display: 'flex', alignItems: 'center', gap: '8px', marginLeft: '16px' }}>
              <User size={20} />
              <span style={{ fontSize: '14px' }}>{user?.username}</span>
            </div>
            
            <button 
              onClick={handleLogout}
              className="button"
              style={{ padding: '8px 16px', fontSize: '14px' }}
            >
              <LogOut size={16} style={{ marginRight: '4px' }} />
              Logout
            </button>
          </div>
        </div>
      </div>
    </nav>
  );
};

export default Navbar;
