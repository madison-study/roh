// src/models/userModel.ts
import pool from "../db/db";

export const getAllItems = async () => {
    const res = await pool.query('SELECT * FROM items');
    return res.rows;
};

export const getItemById = async (id: string) => {
    const res = await pool.query('SELECT * FROM items WHERE id = $1', [id]);
    return res.rows[0];
};

export const addItem = async (name: string, price: number, stock: number, owner: string) => {
    const res = await pool.query(
        'INSERT INTO items (name, price, stock, owner) VALUES ($1, $2, $3, $4) RETURNING *',
        [name, price, stock, owner]
    );
    return res.rows[0];
};

export const updateItem = async (id: string, name: string, price: number, stock: number, owner: string) => {
    const client = await pool.connect();
    try {
        await client.query('BEGIN');

        // 1. row 잠금
        const lock = await client.query('SELECT * FROM items WHERE id = $1 FOR UPDATE', [id]);
        if (lock.rowCount === 0) throw new Error("Item not found");

        // 2. 업데이트
        const update = await client.query(
            'UPDATE items SET name = $1, price = $2, stock = $3, owner = $4 WHERE id = $5 RETURNING *',
            [name, price, stock, owner, id]
        );

        await client.query('COMMIT');
        return update.rows[0];
    } catch (err) {
        await client.query('ROLLBACK');
        throw err;
    } finally {
        client.release();
    }
};

export const deleteItem = async (id: string) => {
    const res = await pool.query('DELETE FROM items WHERE id = $1 RETURNING *', [id]);
    return res.rows[0];
};
