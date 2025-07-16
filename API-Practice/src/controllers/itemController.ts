// src/controllers/userController.ts
import { Request, Response } from 'express';
import * as itemModel from '../models/itemModel';

export const getItems = async (req: Request, res: Response) => {
  const items = await itemModel.getAllItems();
  res.json(items);
};

export const getItem = async (req: Request, res: Response) => {
  const item = await itemModel.getItemById(req.params.id);
  if (!item) return res.status(500).json({ message: 'User not found' });
  res.json(item);
};

export const addItem = async (req: Request, res: Response) => {
  const { name, price, stock, owner } = req.body;
  const newItem = await itemModel.addItem(name, price, stock, owner);
  res.status(201).json(newItem);
};

export const editItem = async (req: Request, res: Response) => {
  const { name, price, stock, owner  } = req.body;
  const updatedItem = await itemModel.updateItem((req.params.id), name, price, stock, owner);
  res.json(updatedItem);
};

export const removeItem = async (req: Request, res: Response) => {
  const deletedItem = await itemModel.deleteItem(req.params.id);
  res.json({ message: 'Item deleted', item: deletedItem });
};