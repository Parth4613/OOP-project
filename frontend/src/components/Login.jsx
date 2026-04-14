import React, { useState } from 'react';
import { Link } from 'react-router-dom';
import { Train, User, Lock } from 'lucide-react';
import { authService } from '../services/api';

const Login = ({ onLogin }) => {
  const [formData, setFormData] = useState({
    username: '',
    password: '',
  });
  const [loading, setLoading] = useState(false);
  const [error, setError] = useState('');

  const handleChange = (e) => {
    setFormData({
      ...formData,
      [e.target.name]: e.target.value,
    });
  };

  const handleSubmit = async (e) => {
    e.preventDefault();
    setLoading(true);
    setError('');

    try {
      const response = await authService.login(formData.username, formData.password);
      onLogin(response);
    } catch (err) {
      setError(err.message || 'Login failed. Please try again.');
    } finally {
      setLoading(false);
    }
  };

  return (
    <div style={{ display: 'flex', justifyContent: 'center', alignItems: 'center', minHeight: 'calc(100vh - 200px)' }}>
      <div className="card" style={{ width: '100%', maxWidth: '400px' }}>
        <div style={{ textAlign: 'center', marginBottom: '32px' }}>
          <div style={{ display: 'flex', justifyContent: 'center', marginBottom: '16px' }}>
            <Train size={48} color="#667eea" />
          </div>
          <h2 style={{ fontSize: '28px', fontWeight: '600', marginBottom: '8px' }}>
            Welcome Back
          </h2>
          <p style={{ color: 'rgba(255, 255, 255, 0.7)' }}>
            Sign in to your RailReserve account
          </p>
        </div>

        <form onSubmit={handleSubmit}>
          <div className="form-group">
            <label htmlFor="username">Username</label>
            <div style={{ position: 'relative' }}>
              <User 
                size={20} 
                style={{ position: 'absolute', left: '12px', top: '50%', transform: 'translateY(-50%)', color: 'rgba(255, 255, 255, 0.5)' }}
              />
              <input
                type="text"
                id="username"
                name="username"
                value={formData.username}
                onChange={handleChange}
                className="input"
                placeholder="Enter your username"
                required
                style={{ paddingLeft: '44px' }}
              />
            </div>
          </div>

          <div className="form-group">
            <label htmlFor="password">Password</label>
            <div style={{ position: 'relative' }}>
              <Lock 
                size={20} 
                style={{ position: 'absolute', left: '12px', top: '50%', transform: 'translateY(-50%)', color: 'rgba(255, 255, 255, 0.5)' }}
              />
              <input
                type="password"
                id="password"
                name="password"
                value={formData.password}
                onChange={handleChange}
                className="input"
                placeholder="Enter your password"
                required
                style={{ paddingLeft: '44px' }}
              />
            </div>
          </div>

          {error && <div className="error">{error}</div>}

          <button 
            type="submit" 
            className="button" 
            disabled={loading}
            style={{ width: '100%', marginBottom: '16px' }}
          >
            {loading ? 'Signing in...' : 'Sign In'}
          </button>
        </form>

        <div style={{ textAlign: 'center', color: 'rgba(255, 255, 255, 0.7)' }}>
          Don't have an account?{' '}
          <Link 
            to="/register" 
            style={{ color: '#667eea', textDecoration: 'none' }}
          >
            Sign up
          </Link>
        </div>
      </div>
    </div>
  );
};

export default Login;
