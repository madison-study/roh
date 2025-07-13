// src/routes/userRoutes.ts
import { Router } from 'express';
import * as itemController from '../controllers/itemController';

const router = Router();

router.get('/', itemController.getItems);
router.get('/:id', itemController.getItem);
router.post('/', itemController.addItem);
router.put('/:id', itemController.editItem);
router.delete('/:id', itemController.removeItem);

export default router;