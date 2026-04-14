const XOR_KEY = 0x5A;

function hashPassword(password) {
  let hex = '';
  for (let i = 0; i < password.length; i++) {
    hex += (password.charCodeAt(i) ^ XOR_KEY).toString(16).padStart(2, '0');
  }
  return hex;
}

function verifyPassword(password, storedHash) {
  return hashPassword(password) === storedHash;
}

module.exports = { hashPassword, verifyPassword };
