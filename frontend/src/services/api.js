const API_BASE_URL = 'http://localhost:5000/api';

class ApiService {
  constructor() {
    this.baseURL = API_BASE_URL;
  }

  async request(endpoint, options = {}) {
    const url = `${this.baseURL}${endpoint}`;
    
    const config = {
      headers: {
        'Content-Type': 'application/json',
        ...options.headers,
      },
      ...options,
    };

    try {
      const response = await fetch(url, config);
      
      if (!response.ok) {
        const errorData = await response.json().catch(() => ({}));
        throw new Error(errorData.message || `HTTP error! status: ${response.status}`);
      }
      
      return await response.json();
    } catch (error) {
      console.error('API request failed:', error);
      throw error;
    }
  }

  // Auth endpoints
  async login(username, password) {
    return this.request('/auth/login', {
      method: 'POST',
      body: JSON.stringify({ username, password }),
    });
  }

  async register(username, password) {
    return this.request('/auth/register', {
      method: 'POST',
      body: JSON.stringify({ username, password }),
    });
  }

  // Train endpoints
  async getTrains() {
    return this.request('/trains');
  }

  async searchTrains(from, to, date) {
    const params = new URLSearchParams({ source: from, destination: to, date });
    return this.request(`/trains/search?${params}`);
  }

  async addTrain(trainData) {
    return this.request('/trains', {
      method: 'POST',
      body: JSON.stringify(trainData),
    });
  }

  async updateTrain(trainId, trainData) {
    return this.request(`/trains/${trainId}`, {
      method: 'PUT',
      body: JSON.stringify(trainData),
    });
  }

  async deleteTrain(trainId) {
    return this.request(`/trains/${trainId}`, {
      method: 'DELETE',
    });
  }

  // Booking endpoints
  async createBooking(bookingData) {
    return this.request('/bookings', {
      method: 'POST',
      body: JSON.stringify(bookingData),
    });
  }

  async getBookings(username) {
    return this.request(`/bookings/history/${username}`);
  }

  async cancelBooking(ticketId, username) {
    return this.request(`/bookings/${ticketId}`, {
      method: 'DELETE',
      body: JSON.stringify({ username }),
    });
  }

  async getAllBookings() {
    return this.request('/bookings');
  }

  // Report endpoints
  async getReports() {
    return this.request('/reports');
  }

  async getBookingStats() {
    return this.request('/reports/revenue');
  }

  // Health check
  async healthCheck() {
    return this.request('/health');
  }
}

export const apiService = new ApiService();

// Auth service for easier authentication handling
export const authService = {
  async login(username, password) {
    const response = await apiService.login(username, password);
    return response;
  },

  async register(username, password) {
    const response = await apiService.register(username, password);
    return response;
  },

  logout() {
    localStorage.removeItem('user');
  },

  getCurrentUser() {
    const userData = localStorage.getItem('user');
    return userData ? JSON.parse(userData) : null;
  },

  isAuthenticated() {
    return !!localStorage.getItem('user');
  },
};
