import express from 'express';
import userRoutes from './routes/userRoutes';
import itemRoutes from './routes/itemRoutes';
import authRoutes from './routes/authRoutes';
import depositRoutes from './routes/depositRoutes'
import dotenv from 'dotenv';
import morgan, { Morgan } from 'morgan';
import { RotatingFileStream, createStream } from "rotating-file-stream";
import path from "path";
import { detailedLogger, combinedLogger } from './middlewares/logger';
import purchaseRoutes from './routes/purchaseRoutes';
import uploadRoutes from './routes/uploadRoutes';
import fetchFilesRoutes from './routes/fetchFilesRoutes';

dotenv.config();
const app = express();

app.use(combinedLogger);
app.use(detailedLogger);

app.use(express.json());
app.use('/users', userRoutes);
app.use('/items', itemRoutes);
app.use('/deposit', depositRoutes);
app.use('/auth', authRoutes);
app.use('/purchase', purchaseRoutes);
app.use('/upload', uploadRoutes);
app.use('/fetch-file', fetchFilesRoutes);

app.listen(3000, () => {
  console.log('Server running on port 3000');
});