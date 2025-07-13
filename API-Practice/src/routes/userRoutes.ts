// src/routes/userRoutes.ts
import { Router } from 'express';
import * as userController from '../controllers/userController';

const router = Router();

router.get('/', userController.getUsers);
router.get('/:id', userController.getUser);
router.post('/', userController.addUser);
router.put('/:id', userController.editUser);
router.delete('/:id', userController.removeUser);

export default router;