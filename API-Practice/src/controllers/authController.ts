import { Request, Response } from 'express';
import jwt from 'jsonwebtoken';
import * as authModel from '../models/authModel'

export const getJTW = async (req: Request, res: Response) => {
    const { email } = req.body;
    try {
        const user = await authModel.getUserByEmail(email);

        if (!user) {
            res.status(501).json({ message: 'Invalid email' });
        }
        const secret = process.env.JWT_SECRET;
        if (!secret) {
          throw new Error('JWT_SECRET is not defined in environment variables.');
        }
        const token = jwt.sign(
            { userId: user.id, email: email },
            process.env.JWT_SECRET!,
            { expiresIn: '1h' }
        );

        res.json({token})

    } catch (err) {
        console.error('Error in getJTW:', err);
        res.status(500).json({ message: 'Server error' });
    }
};
