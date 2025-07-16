import { Request, Response } from 'express';
import { purchaseItem } from '../services/purchaseServices';

export const purchase = async (req: Request, res: Response) => {

  const itemId = req.body.itemId;
  const userId = req.user?.userId;

  if (!userId) res.status(501).json({ message: 'user not provided' });

  try {
    const result = await purchaseItem(userId as string, itemId);
    res.status(200).json(result);
  } catch (error) {
    res.status(500).json({"error":error});
  }

};