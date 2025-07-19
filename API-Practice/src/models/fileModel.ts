import pool from "../db/db";

export const fileUpload = async (path: string, name: string, mime_type: string, size: number, uploader: string, is_public: boolean) => {
    const res = await pool.query(
        `
        INSERT INTO files (
            path, 
            original_name,   
            mime_type, 
            size, 
            uploader, 
            is_public
        ) 
        VALUES ($1, $2, $3, $4, $5, $6)
        `,
        [path, name, mime_type, size, uploader, is_public]
    );

    return res.rows[0];
}

export const fileFetch = async () => {
    const res = await pool.query(
        `
            SELECT * FROM files
        `
    ) 
    return res.rows;
}

export const searchFileById = async (id: string): Promise<boolean> => {
    const res = await pool.query(
        'SELECT EXISTS (SELECT 1 FROM files WHERE id = $1)',
        [id]
    );
    return res.rows[0].exists;
};
