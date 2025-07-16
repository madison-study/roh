// src/routes/userRoutes.ts
import { Router } from 'express';
import * as deposit from '../controllers/depositController';

const router = Router();

router.post('/', deposit.depositUser);

export default router;