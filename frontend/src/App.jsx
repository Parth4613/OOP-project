import React, { useState, useEffect } from 'react';
import { BrowserRouter as Router, Routes, Route, Navigate } from 'react-router-dom';
import Navbar from './components/Navbar';
import Login from './components/Login';
import Register from './components/Register';
import Dashboard from './components/Dashboard';
import AdminDashboard from './components/AdminDashboard';
import TrainSearch from './components/TrainSearch';
import Bookings from './components/Bookings';
import { authService } from './services/api';

function App() {
  const [user, setUser] = useState(null);
  const [loading, setLoading] = useState(true);

  useEffect(() => {
    const userData = localStorage.getItem('user');
    
    if (userData) {
      try {
        const parsedUser = JSON.parse(userData);
        setUser(parsedUser);
      } catch (error) {
        console.error('Error parsing user data:', error);
        localStorage.removeItem('user');
      }
    }
    setLoading(false);
  }, []);

  const login = (userData) => {
    setUser(userData);
    localStorage.setItem('user', JSON.stringify(userData));
  };

  const logout = () => {
    setUser(null);
    localStorage.removeItem('user');
  };

  if (loading) {
    return (
      <div className="loading">
        <div>Loading...</div>
      </div>
    );
  }

  return (
    <Router>
      <div className="App">
        {user && <Navbar user={user} onLogout={logout} />}
        <main style={{ flex: 1, padding: '20px 0' }}>
          <div className="container">
            <Routes>
              <Route 
                path="/login" 
                element={!user ? <Login onLogin={login} /> : <Navigate to="/dashboard" />} 
              />
              <Route 
                path="/register" 
                element={!user ? <Register onLogin={login} /> : <Navigate to="/dashboard" />} 
              />
              <Route 
                path="/dashboard" 
                element={user ? <Dashboard user={user} /> : <Navigate to="/login" />} 
              />
              <Route 
                path="/admin" 
                element={user?.role === 'admin' ? <AdminDashboard user={user} /> : <Navigate to="/dashboard" />} 
              />
              <Route 
                path="/trains" 
                element={user ? <TrainSearch /> : <Navigate to="/login" />} 
              />
              <Route 
                path="/bookings" 
                element={user ? <Bookings user={user} /> : <Navigate to="/login" />} 
              />
              <Route path="/" element={<Navigate to={user ? "/dashboard" : "/login"} />} />
            </Routes>
          </div>
        </main>
      </div>
    </Router>
  );
}

export default App;
