import { Request, Response } from 'express';
import * as uploadModel from '../models/fileModel';
import * as userModel from '../models/userModel';

export const uploadFile = async (req: Request, res: Response) => {
    try {
      // 파일 존재 여부 확인
      if (!req.file) {
        return res.status(400).json({ message: 'No file uploaded' });
      }
  
      // 업로더 정보
      const uploader = req.user?.email;
      if (!uploader) {
        return res.status(400).json({ message: 'User not found' });
      }
      
      // req.file에서 정보 추출
      const { path, filename, mimetype, size } = req.file;
  
      // is_public: 기본값 true
      const is_public =
      req.query.is_public !== undefined
        ? req.query.is_public === 'true'
        : true;
  
      // DB 저장
      await uploadModel.fileUpload(path, filename, mimetype, size, uploader, is_public);
      await userModel.newFile(uploader, filename);
      // 응답
      res.status(200).json({ filename });
    } catch (error) {
      console.error('File upload error:', error);
      res.status(500).json({ message: 'Internal server error' });
    }
  };

