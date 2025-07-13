// src/controllers/userController.ts
import { Request, Response } from 'express';
import * as userModel from '../models/userModel';

export const getUsers = async (req: Request, res: Response) => {
  const users = await userModel.getAllUsers();
  res.json(users);
};

export const getUser = async (req: Request, res: Response) => {
  const user = await userModel.getUserById(Number(req.params.id));
  if (!user) return res.status(404).json({ message: 'User not found' });
  res.json(user);
};

export const addUser = async (req: Request, res: Response) => {
  const { name, email, birthday } = req.body;
  const newUser = await userModel.createUser(name, email, birthday);
  res.status(201).json(newUser);
};

export const editUser = async (req: Request, res: Response) => {
  const { name, email, birthday } = req.body;
  const updatedUser = await userModel.updateUser(Number(req.params.id), name, email, birthday);
  res.json(updatedUser);
};

export const removeUser = async (req: Request, res: Response) => {
  const deletedUser = await userModel.deleteUser(Number(req.params.id));
  res.json({ message: 'User deleted', user: deletedUser });
};