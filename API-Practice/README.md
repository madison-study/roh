# SQL DBMS

## 1. 📘 데이터베이스란?

- **데이터베이스 (DB)**: 데이터를 구조화된 방식으로 저장하고 검색할 수 있도록 만든 시스템.
- **RDB (Relational Database)**: 테이블(표)의 형태로 데이터를 저장하고, 각 테이블은 관계(Relation)를 가짐.
- SQL 기반 DB는 전통적인 은행, 쇼핑몰, SNS, 업무 시스템 등에 널리 사용됨.

## 2. 🔡 SQL이란?

- **SQL (Structured Query Language)**: RDB에서 데이터를 삽입, 조회, 수정, 삭제하기 위한 표준 언어.

```sql
SELECT name FROM users WHERE age > 30;
```

## 3. 🧱 데이터베이스 구성 요소 

### 📁 데이터베이스 (Database)
- 여러 개의 테이블, 인덱스, 함수 등을 포함하는 큰 단위

### 📄 테이블 (Table)
- 데이터를 행(Row)과 열(Column)로 저장하는 구조

| 용어                     | 설명             |
| ---------------------- | -------------- |
| **Row (행)**            | 데이터 한 건        |
| **Column (열)**         | 속성, 필드         |
| **Primary Key (기본 키)** | 행을 유일하게 식별     |
| **Foreign Key (외래 키)** | 다른 테이블과의 관계 설정 |


## 4. 📂 테이블 생성과 구조 정의

```sql
CREATE TABLE users (
  id SERIAL PRIMARY KEY,
  name TEXT NOT NULL,
  email TEXT UNIQUE,
  age INTEGER,
  created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);
```

TEXT: 문자열
INTEGER: 정수
BOOLEAN: true / false
TIMESTAMP: 날짜 및 시간

## 5. 📥 데이터 조작 (CRUD)

### 🔹 1. INSERT (삽입)

```sql
INSERT INTO users (name, email, age)
VALUES ('Alice', 'alice@example.com', 25);
```

---

### 🔹 2. SELECT (조회)

```sql
SELECT * FROM users;
SELECT name, age FROM users WHERE age > 30;
```

---

### 🔹 3. UPDATE (수정)

```sql
UPDATE users SET age = 26 WHERE name = 'Alice';
```

---

### 🔹 4. DELETE (삭제)

```sql
DELETE FROM users WHERE id = 1;
```

---

### 5. 🔍 WHERE / ORDER BY / LIMIT

```sql
SELECT * FROM users
WHERE age >= 20
ORDER BY age DESC
LIMIT 10;
```

* **WHERE**: Filters rows based on a specified condition.
* **ORDER BY**: Sorts the result set by one or more columns.
* **LIMIT**: Restricts the number of rows returned.

---

### 6. 🔗 테이블 관계 (JOIN)

```sql
SELECT users.name, orders.amount
FROM users
JOIN orders ON users.id = orders.user_id;
```

#### JOIN Types:

* **INNER JOIN**: Returns rows when there is a match in both tables.
* **LEFT JOIN**: Returns all rows from the left table, and the matched rows from the right table.
* **RIGHT JOIN**: Returns all rows from the right table, and the matched rows from the left table.
* **FULL JOIN**: Returns all rows when there is a match in one of the tables.

---

### 7. 🛡️ 제약 조건 (Constraints)

* **PRIMARY KEY**: A unique identifier for each record in a table; cannot be NULL.
* **UNIQUE**: Ensures all values in a column are different.
* **NOT NULL**: Ensures that a column cannot have a NULL value.
* **FOREIGN KEY**: A key used to link two tables together.
* **CHECK**: Ensures that all values in a column satisfy a specific condition.

---

### 8. 📈 인덱스 (Index)

```sql
CREATE INDEX idx_users_email ON users(email);
```

* Indexes are used to **speed up search operations** on a database table.
* They are typically set on columns that are frequently queried.

자주 검색되는 컬럼은 인덱스 적극 활용

자주 조인되는 외래 키에도 인덱스 필요

읽기 많은 시스템에는 인덱스 유리

쓰기 많은 시스템은 인덱스 최소화 고려

* What if there are too many indexes

> INSERT, UPDATE, DELETE 시 모든 인덱스도 동시에 갱신됨
> TPS(초당 트랜잭션 수) 급감 → 대량 로그 수집 시스템에서 병목 발생
> 로그 테이블처럼 쓰기 중심(Write-heavy) 환경에서는 오히려 성능 악화

| 상황            | 인덱스 남용 위험              |
| ------------- | ---------------------- |
| 인덱스가 너무 많음    | 쓰기 성능 저하 (모두 유지해야 하므로) |
| 사용되지 않는 인덱스   | 디스크 낭비 + 유지비 발생        |
| 복합 인덱스 순서 문제  | 일부 쿼리에만 유효             |
| 함수가 포함된 WHERE | 인덱스 무효화                |
| 잦은 업데이트 컬럼    | 인덱스 갱신 비용 ↑            |

* 인덱스는 반드시 쿼리 패턴을 분석하고, 사용 빈도를 고려해 최소화
* EXPLAIN 명령어로 실제 인덱스 사용 여부를 확인할 것
* 쓰기 많은 테이블은 인덱스 최소화가 핵심

---

### 9. 👨‍💻 실무에서의 SQL 활용

* **Integration with Backend Servers**: Used with languages like Node.js, Python, Java, etc.
* **ORM Tools**: Often used with Object-Relational Mapping tools such as Sequelize, Prisma, TypeORM, etc., to interact with databases using object-oriented paradigms.
* **Connecting to REST APIs and Admin Pages**: SQL powers the data retrieval and manipulation for these interfaces.

---

### ✅ Summary

| Category        | Key Concepts                          |
| :-------------- | :------------------------------------ |
| Data Structure  | DB, Table, Row/Column, Key            |
| SQL Commands    | CREATE, INSERT, SELECT, UPDATE, DELETE|
| Search Control  | WHERE, ORDER BY, LIMIT                |
| Relationship    | JOIN, FOREIGN KEY                     |
| Performance     | INDEX                                 |
| Data Integrity  | NOT NULL, UNIQUE, CHECK               |

## Injection Prevention

### ❌ 취약한 예 (Node.js 예시):
W
```sql
const name = req.query.name;
const query = `SELECT * FROM users WHERE name = '${name}'`;
```
### ✅ 안전한 방식: 값은 파라미터로 처리

```sql
const name = req.query.name;
const query = 'SELECT * FROM users WHERE name = $1';
const values = [name];

const result = await pool.query(query, values);
```

## 분리된 아키텍처

| 계층                   | 역할                                        |
| -------------------- | ----------------------------------------- |
| **Controller**       | 요청/응답 처리 (`req`, `res`)                   |
| **Service**          | 비즈니스 로직 (`if`, `try`, `validate`, etc.)   |
| **Repository**       | DB 접근 로직만 (`query`, `insert`, etc.)       |
| **Model (optional)** | Type / Interface 정의 (`User`, `Product` 등) |


# POSTGRESQL

