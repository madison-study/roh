import { Request, Response, NextFunction } from 'express';
import { JwtPayload } from '../types/jwtPayload';
import { validateEmail } from '../models/authModel';
import jwt from 'jsonwebtoken';

export const authenticateJWT = async (req: Request, res: Response, next: NextFunction) => {
  const authHeader = req.headers.authorization;

  if (!authHeader || !authHeader.startsWith('Bearer ')) {
    return res.status(401).json({ message: 'Missing token' });
  }

  const token = authHeader.split(' ')[1];
  try {
    const decoded = jwt.verify(token, process.env.JWT_SECRET!) as JwtPayload;
    req.user = decoded; // 타입 확장 필요 시 interface 위에 선언

    const valid = await validateEmail(decoded.email);

    if(!valid) {
        return res.status(502).json({message: "Invalid User"});
    }

    next();
  } catch (err) {
    return res.status(403).json({ message: 'Invalid or expired token' });
  }
};