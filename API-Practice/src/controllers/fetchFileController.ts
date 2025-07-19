import { Request, Response } from 'express';
import * as fileModel from '../models/fileModel';


export const fetchFile = async (req: Request, res: Response) => {
    const files = await fileModel.fileFetch();
    res.json(files);
    // res.download(filePath);
}