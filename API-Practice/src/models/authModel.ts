// src/models/userModel.ts
import pool from "../db/db";

export const validateEmail = async (email: string): Promise<boolean> => {
    const res = await pool.query(
        'SELECT EXISTS (SELECT 1 FROM users WHERE email = $1)',
        [email]
    );
    return res.rows[0].exists;
};

export const getUserByEmail = async (email: string) => {
    const res = await pool.query(
        'SELECT * FROM users WHERE email=  $1', [email]
    );

    return res.rows[0];
}

