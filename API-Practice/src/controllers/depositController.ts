import { Request, Response } from 'express';
import * as depositModel from '../models/depositModel';

export const depositUser = async (req: Request, res: Response) => {
    const { email, deposit } = req.body;
    const balance = await depositModel.depositUser(email, deposit);
    res.json({ message: 'Curr balance', balance: balance });
  };