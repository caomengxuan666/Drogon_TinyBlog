-- database: ./TinyBlog.db

-- 插入 users 表数据
INSERT INTO users (username) VALUES ('user1');
INSERT INTO users (username) VALUES ('user2');

-- 插入 articles 表数据
INSERT INTO articles (title, author_id, content, category, tag, Excerpt) VALUES ('First Article', 1, 'This is the content of the first article.', 'Tech', 'SQL', 'Excerpt of the first article.');
INSERT INTO articles (title, author_id, content, category, tag, Excerpt) VALUES ('Second Article', 2, 'This is the content of the second article.', 'Life', 'Travel', 'Excerpt of the second article.');