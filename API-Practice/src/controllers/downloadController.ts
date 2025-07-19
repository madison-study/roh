import { Request, Response } from 'express';
import * as uploadModel from '../models/fileModel';


export const downloadFile = async (req: Request, res: Response) => {
    // const filename = req.params.filename;
    // const filePath = getFilePath(filename);
  
    // const exists = await checkFileExists(filePath);
    // if (!exists) return res.status(404).json({ message: 'File not found' });
  
    // res.download(filePath);
}