import pkg from 'pg';
const { Pool } = pkg;

const pool = new Pool({
  max: 20,           // 동시 연결 제한
  idleTimeoutMillis: 30000,
  connectionTimeoutMillis: 2000,
  user: process.env.DB_USER,
  host: process.env.DB_HOST,
  database: process.env.DB_NAME,
  password: process.env.DB_PASSWORD,
  port: Number(process.env.DB_PORT),
});

export default pool;