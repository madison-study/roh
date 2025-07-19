import { Router } from 'express';
import * as fetchFileController from '../controllers/fetchFileController';

const router = Router();

router.get('/', fetchFileController.fetchFile);

export default router