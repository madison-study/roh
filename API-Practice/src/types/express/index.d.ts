import { JwtPayload } from 'jsonwebtoken';

declare global {
  namespace Express {
    interface Request {
      user?: JwtPayload & { userId: string; email: string }; 
      // JwtPayload를 기반으로 userId, email 등 필요한 필드 추가
    }
  }
}