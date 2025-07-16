import morgan from "morgan";
import path from "path";
import { createStream } from "rotating-file-stream";
import { Request } from "express";
import fs from "fs";

const logDir = path.join(process.cwd(), ".logs");
if (!fs.existsSync(logDir)) {
    fs.mkdirSync(logDir);
}

// ✅ 1. 기본 combined 로그 스트림
const combinedLogStream = createStream("combined.log", {
    interval: "1d",
    path: logDir,
});

// ✅ 2. 디버그용 상세 로그 스트림
const detailedLogStream = createStream("detailed.log", {
    interval: "1d",
    path: logDir,
});;

// 커스텀 토큰 정의
morgan.token("body", (req: Request) => JSON.stringify(req.body || {}));

// ✅ 4. 포맷 정의
const detailedFormat = [
    ":remote-addr",
    ":method",
    ":url",
    ":status",
    ":res[content-length]",
    "- :response-time ms",
    '- ":user-agent"',
    "- body: :body"
].join(" ");

// ✅ 5. 미들웨어 export
export const combinedLogger = morgan("combined", {
    stream: combinedLogStream,
});

export const detailedLogger = morgan(detailedFormat, {
    stream: detailedLogStream,
    // skip: (req, res) => req.method === "GET", // 예시: GET 요청은 상세 로그 제외
});