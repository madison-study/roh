import fs from 'fs';
import path from 'path';
import { promisify } from 'util';

const writeFile = promisify(fs.writeFile);

const UPLOAD_DIR = path.join(__dirname, './vault');

export const saveFileLocally = async (filename: string, buffer: Buffer) => {
  const filePath = path.join(UPLOAD_DIR, filename);

  await writeFile(filePath, buffer);
  return filePath;
};