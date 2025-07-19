// src/routes/uploadRoutes.ts
import { Router } from 'express';
// import * as purchaseController from '../controllers/purchaseController';
import * as uploadController from '../controllers/uploadController';
import { authenticateJWT } from '../middlewares/auth';
import { upload } from '../middlewares/upload';

const router = Router();

router.post('/', authenticateJWT, upload.single('upload') ,uploadController.uploadFile);
// router.post('/', userController.addUser);
// router.put('/:id', userController.editUser);
// router.delete('/:id', userController.removeUser);

export default router;