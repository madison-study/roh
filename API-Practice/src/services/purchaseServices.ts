import pool from "../db/db";
import * as userModel from '../models/userModel';
import * as itemModel from '../models/itemModel';
import * as depositModel from '../models/depositModel';
import * as authModel from '../models/authModel'

// --> pass client as parameter so that model manages query
export const purchaseItem = async (userId: string, itemId: string) => {
    const client = await pool.connect();

    try {
        await client.query('BEGIN')

        const itemRes = await client.query('SELECT price, stock, owner FROM items WHERE id = $1 FOR UPDATE', [itemId]);
        const item = itemRes.rows[0];

        if (!item || item.stock <= 0) throw new Error('Item not available');

        const sellerRes = await client.query('SELECT balance, id FROM users WHERE email = $1 FOR UPDATE', [item.owner]);
        const seller = sellerRes.rows[0];
        if (!seller) throw new Error('Seller not valid');

        const userRes = await client.query('SELECT balance FROM users WHERE id = $1 FOR UPDATE', [userId]);
        const user = userRes.rows[0];
        if (!user || user.balance < item.price) throw new Error('Insufficient balance');

        await client.query('UPDATE items SET stock = stock - 1 WHERE id = $1', [itemId]);
        await client.query('UPDATE users SET balance = balance - $1 WHERE id = $2', [item.price, userId]);

        await client.query('UPDATE users SET balance = balance + $1 WHERE id = $2', [item.price, seller.id])

        await client.query(
            'INSERT INTO purchases (user_id, item_id, price, purchased_at) VALUES ($1, $2, $3, NOW())',
            [userId, itemId, item.price]
        );
        await client.query('COMMIT');
        return { success: true };   
    } catch (err) {
        await client.query('ROLLBACK');
        throw err;
    } finally {
        client.release();
    }
}
