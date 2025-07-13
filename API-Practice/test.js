const pool = require('./db');

async function testConnection() {
  try {
    const res = await pool.query('SELECT NOW()');
    console.log('DB connected successfully:', res.rows[0]);
  } catch (err) {
    console.error('DB connection failed:', err);
  } finally {
    await pool.end(); // 연결 종료 (서버에서는 보통 닫지 않음)
  }
}

testConnection();
