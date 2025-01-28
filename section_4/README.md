# Проектирование БД для чата

### Таблицы

##### Users

| поле | тип | комментарий |
| - | - | - |
| id | int | первичный ключ |
| login | varchar(255) | |

##### Passwords

| поле | тип | комментарий |
| - | - | - |
| id | int | |
| user_id | int | внешний ключ |
| hash | varchar(255) | |

##### Messages

| поле | тип | комментарий |
| - | - | - |
| id | int | первичный ключ |
| sender_id | int | внешний ключ |
| receiver_id | int | внешний ключ |
| text | text | |
| datetime | timestamp | |
| condition | int | 0 — не отправлено, 1 — доставлено, 2 — прочитано |


### Команды БД

1. Создание таблиц  

```sql
CREATE TABLE IF NOT EXISTS users ( 
    id          SERIAL PRIMARY KEY, 
    login       VARCHAR(255) NOT NULL
);
```

```sql
CREATE TABLE IF NOT EXISTS passwords ( 
    id          SERIAL PRIMARY KEY, 
    user_id     INT NOT NULL REFERENCES users (id),
    pass_hash   VARCHAR(255) NOT NULL
);
```

```sql
CREATE TABLE IF NOT EXISTS messages ( 
    id          SERIAL PRIMARY KEY, 
    sender_id   INT NOT NULL REFERENCES users (id),
    reciever_id INT NOT NULL REFERENCES users (id),
    content     TEXT NOT NULL, 
    created_at  TIMESTAMP DEFAULT CURRENT_TIMESTAMP, 
    condition   SMALLINT DEFAULT 0   --- статус сообщения
);
```

2. Триггеры

- создание хэша в таблице паролей при добавлении пользователя
<!-- TODO -->
- удаление хэша в таблице паролей при удалении пользователя
<!-- TODO -->

3. Наполнение таблиц тестовыми данными

```sql
INSERT INTO users (login) 
VALUES  ('Ivan'), 
        ('Artem'), 
        ('Anna'), 
        ('Fedor'), 
        ('Lika');
```

```sql
INSERT INTO passwords 
       (user_id, pass_hash) 
VALUES ((SELECT id FROM users WHERE login = 'Ivan'), 'oainc9a8y38gr42yfoaijcdpoaode4'), 
        ((SELECT id FROM users WHERE login = 'Artem'), 'poivn09203e1isd2wo1dp2w1wofmo7'), 
        ((SELECT id FROM users WHERE login = 'Anna'), 'poivn09103e1isd2wo1dp2w1wofmo7'), 
        ((SELECT id FROM users WHERE login = 'Fedor'), '03efrjoeirfnowefdikjpsocdufmm3'), 
        ((SELECT id FROM users WHERE login = 'Lika'), '09837gyuhjvbkfndsl9823uyref657');
```

```sql
INSERT INTO messages 
       (sender_id, reciever_id, content, condition) 
VALUES (
            (SELECT id FROM users WHERE login = 'Ivan'), 
            (SELECT id FROM users WHERE login = 'Artem'), 
            'Hello, Artem!',
            0
        ), 
        (
            (SELECT id FROM users WHERE login = 'Artem'), 
            (SELECT id FROM users WHERE login = 'Ivan'), 
            'Hello, Ivan!',
            0
        ), 
        (
            (SELECT id FROM users WHERE login = 'Fedor'), 
            (SELECT id FROM users WHERE login = 'Lika'), 
            'Hello, Lika! How are you?',
            0
        );
```

4. Корректное отображение данных в таблице сообщений после удаления пользователя

<!-- TODO -->

создание поля для хранения статуса пользователя (1 — нормальный пользователь, 0 — удалён). Также нужно переписать команду удаления из таблицы пользователей так, чтобы данные не удалялись, а просто менялся статус пользователя на 0.


5. Запросы к БД

Запрос, который выведет логин, хэш пароля:  
```sql
SELECT 
    users.login as login,
    passwords.pass_hash as hash
FROM users 
LEFT JOIN passwords 
ON
    users.id=passwords.user_id;
```

Запрос, который выведет пользователей с одинаковым email:  
```sql
--- TODO: проверить
SELECT *
FROM users as u1
INNER JOIN users as u2
ON u1.email=u2.email
WHERE u1.id <> u2.id
```
Запрос, который выведет все отправленные и полученные сообщения конкретного пользователя, отсортированные по времени:  
```sql
--- TODO
```
Список удалённых пользователей:  
```sql
--- TODO
```
Список сообщений, которые пользователь отправил сам себе:  
```sql
SELECT *
FROM messages
WHERE sender_id=reciever_id
```
