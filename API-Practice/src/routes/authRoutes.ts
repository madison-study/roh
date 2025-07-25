
import { Router } from 'express';
import * as authController from '../controllers/authController';

const router = Router();

router.post('/', authController.getJTW);

export default router;