import express from 'express';
import userRoutes from './routes/userRoutes';
import itemRoutes from './routes/itemRoutes'
import dotenv from 'dotenv';
import morgan, { Morgan } from 'morgan';
import { RotatingFileStream, createStream } from "rotating-file-stream";
import path from "path";
import { detailedLogger, combinedLogger } from './middlewares/logger';

dotenv.config();
const app = express();

app.use(combinedLogger);
app.use(detailedLogger);

app.use(express.json());
app.use('/users', userRoutes);
app.use('/items', itemRoutes)

app.listen(3000, () => {
  console.log('Server running on port 3000');
});