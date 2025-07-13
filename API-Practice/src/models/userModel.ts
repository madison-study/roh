// src/models/userModel.ts
import pool from "../db/db";

export const getAllUsers = async () => {
  const res = await pool.query('SELECT * FROM users');
  return res.rows;
};

export const getUserById = async (id: number) => {
  const res = await pool.query('SELECT * FROM users WHERE id = $1', [id]);
  return res.rows[0];
};

export const createUser = async (name: string, email: string, birthday: string) => {
  const res = await pool.query(
    'INSERT INTO users (name, email, birthday) VALUES ($1, $2, $3) RETURNING *',
    [name, email, birthday]
  );
  return res.rows[0];
};

export const updateUser = async (id: number, name: string, email: string, birthday: string) => {
  const res = await pool.query(
    'UPDATE users SET name = $1, email = $2, birthday = $3 WHERE id = $4 RETURNING *',
    [name, email, birthday, id]
  );
  return res.rows[0];
};

export const deleteUser = async (id: number) => {
  const res = await pool.query('DELETE FROM users WHERE id = $1 RETURNING *', [id]);
  return res.rows[0];
};
