# Проектирование БД для чата

### Таблицы

##### Users

| поле | тип | комментарий |
| - | - | - |
| id | int | первичный ключ |
| name | varchar(255) | |
| surname | varchar(255) | |
| email | varchar(255) | |

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
    name        VARCHAR(255) NOT NULL, 
    surname     VARCHAR(255) NOT NULL, 
    email       VARCHAR(255) NOT NULL
);
```

```sql
CREATE TABLE IF NOT EXISTS passwords ( 
    id          SERIAL PRIMARY KEY, 
    user_id     INT NOT NULL, --- сделать внешним ключом
    pass_hash   VARCHAR(255) NOT NULL,
    FOREIGN KEY (user_id) REFERENCES users (id)
);
```

```sql
CREATE TABLE IF NOT EXISTS messages ( 
    id          SERIAL PRIMARY KEY, 
    sender_id   INT NOT NULL, --- сделать внешним ключом
    reciever_id INT NOT NULL, --- сделать внешним ключом
    content     TEXT NOT NULL, 
    created_at  TIMESTAMP DEFAULT CURRENT_TIMESTAMP, 
    condition   TINYINT UNSIGNED NOT NULL,   --- статус сообщения
    FOREIGN KEY (sender_id) REFERENCES users (id)
    FOREIGN KEY (reciever_id) REFERENCES users (id)
);
```

2. Триггеры

- создание хэша в таблице паролей при добавлении пользователя
<!-- TODO -->
- удаление хэша в таблице паролей при удалении пользователя
<!-- TODO -->

3. Наполнение таблиц тестовыми данными

```sql
INSERT users 
       (name, surname, email) 
VALUES ('Ivan', 'Ivanov', ivan.ivanov@mail.ru), 
        ('Artem', 'Artemov', artem.artemov@gmail.com), 
        ('Anna', 'Artemova', artem.artemov@gmail.com), 
        ('Fedor', 'Fedorov', fedor.fedorov@ya.ru), 
        ('Lika', 'Likova', lika.likova@rambler.ru);
```

```sql
INSERT passwords 
       (user_id, pass_hash) 
VALUES ((SELECT id FROM users WHERE name = 'Ivan' AND surname = 'Ivanov'), 'oainc9a8y38gr42yfoaijcdpoaode4'), 
        ((SELECT id FROM users WHERE name = 'Artem' AND surname = 'Artemov'), 'poivn09203e1isd2wo1dp2w1wofmo7'), 
        ((SELECT id FROM users WHERE name = 'Anna' AND surname = 'Artemova'), 'poivn09103e1isd2wo1dp2w1wofmo7'), 
        ((SELECT id FROM users WHERE name = 'Fedor' AND surname = 'Fedorov'), '03efrjoeirfnowefdikjpsocdufmm3'), 
        ((SELECT id FROM users WHERE name = 'Lika' AND surname = 'Likova'), '09837gyuhjvbkfndsl9823uyref657');
```

```sql
INSERT messages 
       (sender_id, reciever_id, content, condition) 
VALUES (
            (SELECT id FROM users WHERE name = 'Ivan' AND surname = 'Ivanov'), 
            (SELECT id FROM users WHERE name = 'Artem' AND surname = 'Artemov'), 
            'Hello, Artem!',
            0
        ), 
        (
            (SELECT id FROM users WHERE name = 'Artem' AND surname = 'Artemov'), 
            (SELECT id FROM users WHERE name = 'Ivan' AND surname = 'Ivanov'), 
            'Hello, Ivan!',
            0
        ), 
        (
            (SELECT id FROM users WHERE name = 'Fedor' AND surname = 'Fedorov'), 
            (SELECT id FROM users WHERE name = 'Lika' AND surname = 'Likova'), 
            'Hello, Lika! How are you?',
            0
        );
```

4. Корректное отображение данных в таблице сообщений после удаления пользователя

<!-- TODO -->

создание поля для хранения статуса пользователя (1 — нормальный пользователь, 0 — удалён). Также нужно переписать команду удаления из таблицы пользователей так, чтобы данные не удалялись, а просто менялся статус пользователя на 0.


5. Запросы к БД

Запрос, который выведет имя, фамилию, хэш пароля:  
```sql
SELECT 
    users.name as name,
    users.surname as surname,
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
