import pool from "../db/db";

export const depositUser = async (email: string, deposit: number) => {
    const res = await pool.query(
        `UPDATE users 
       SET balance = balance + $1 
       WHERE email = $2 
       RETURNING *`,
        [deposit, email]
    );
    return res.rows[0];
}