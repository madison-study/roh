// src/routes/userRoutes.ts
import { Router } from 'express';
import * as purchaseController from '../controllers/purchaseController';
import { authenticateJWT } from '../middlewares/auth';

const router = Router();

router.post('/', authenticateJWT ,purchaseController.purchase);
// router.post('/', userController.addUser);
// router.put('/:id', userController.editUser);
// router.delete('/:id', userController.removeUser);

export default router;