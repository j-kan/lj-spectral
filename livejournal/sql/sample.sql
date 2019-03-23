DROP TABLE IF EXISTS out_degree;
CREATE TABLE out_degree (
  user_id  int NOT NULL,
  degree   int NOT NULL 
) ENGINE=MyISAM;

CREATE INDEX in_degree_user_id ON in_degree(user_id);

INSERT INTO out_degree(user_id, degree) 
SELECT a, count(*) AS degree FROM is_friend_of GROUP BY a


DROP TABLE IF EXISTS in_degree;
CREATE TABLE in_degree (
  user_id int NOT NULL,
  degree  int     NOT NULL 
) ENGINE=MyISAM;

CREATE INDEX out_degree_user_id ON out_degree(user_id);

INSERT INTO in_degree(user_id, degree) 
SELECT b, count(*) AS degree FROM is_friend_of GROUP BY b;


DROP TABLE IF EXISTS user_sample;
CREATE TABLE user_sample (
  id INT NOT NULL AUTO_INCREMENT PRIMARY KEY,
  original_id INT NOT NULL,
  name varchar(16) NOT NULL,
  CONSTRAINT UNIQUE KEY i_user_sample_name (name)
) ENGINE=MyISAM;


CREATE TABLE interest_sample (
  id INT NOT NULL PRIMARY KEY,
  original_id INT NOT NULL,
  name varchar(80) NOT NULL,
  count INT default 0,
  CONSTRAINT UNIQUE KEY i_interest_sample_name (name)
) ENGINE=MyISAM;

DROP TABLE IF EXISTS is_friend_of_sample;
CREATE TABLE is_friend_of_sample (
  a int NOT NULL,
  b int NOT NULL,
  KEY i_is_friend_of_sample_a (a), 
  KEY i_is_friend_of_sample_b (b)
) ENGINE=MyISAM;


CREATE TABLE is_interested_in_sample (
  user_id     int NOT NULL,
  interest_id int NOT NULL,
  KEY i_is_interested_in_sample_u (user_id), 
  KEY i_is_interested_in_sample_i (interest_id)
) ENGINE=MyISAM;




INSERT INTO user_sample(original_id, name)
SELECT u.id, u.name 
       FROM user u
  LEFT JOIN in_degree i  ON u.id = i.user_id
  LEFT JOIN out_degree o ON u.id = o.user_id
      WHERE i.degree >= 10
        AND o.degree >= 10
        AND u.visited = 1;


        
select count(*) from user_sample

+----------+
| count(*) |
+----------+
|   129456 | 
+----------+


select * from user_sample limit 10
select max(id) from user_sample
select * from user_sample where id = 129456

+--------+-------------+------------+
| id     | original_id | name       |
+--------+-------------+------------+
| 129456 |      169488 | artyblue70 | 
+--------+-------------+------------+

+---------+
| max(id) |
+---------+
|  129456 | 
+---------+

+----+-------------+-----------------+
| id | original_id | name            |
+----+-------------+-----------------+
|  1 |           1 | intrepidredhead | 
|  2 |           2 | rothko          | 
|  3 |           3 | rainaraina      | 
|  4 |           4 | quelrod         | 
|  5 |           5 | adranse         | 
|  6 |           6 | 0riet           | 
|  7 |           7 | moonwick        | 
|  8 |           8 | philoticjane    | 
|  9 |           9 | griffjon        | 
| 10 |          10 | djvoid          | 
+----+-------------+-----------------+

select count(*) from user

+----------+
| count(*) |
+----------+
|  1951177 | 
+----------+

select * from user_sample where id = 21925

        

INSERT INTO is_friend_of_sample(a, b)
SELECT ifo.* 
  FROM is_friend_of ifo
  INNER JOIN user_sample ua ON ifo.a = ua.id
  INNER JOIN user_sample ub ON ifo.b = ub.id;

select count(*) from is_friend_of_sample;

select * from is_friend_of_sample limit 10;

+----------+
| count(*) |
+----------+
|  3586274 | 
+----------+

select count(*) from in_degree;
select count(*) from out_degree;
select count(*) from user;


+----------+
| count(*) |
+----------+
|  1614581 | 
+----------+
+----------+
| count(*) |
+----------+
|  1951177 | 
+----------+
+----------+
| count(*) |
+----------+
|  1707416 | 
+----------+


select count(*) from is_friend_of;
select count(*) from is_friend_of_sample;
select count(*) from user;
select count(*) from user_sample;

+----------+
| count(*) |
+----------+
| 16263959 | 
+----------+
+----------+
| count(*) |
+----------+
|   487077 | 
+----------+
+----------+
| count(*) |
+----------+
|  1951177 | 
+----------+
+----------+
| count(*) |
+----------+
|    21925 | 
+----------+

+----------+
| count(*) |
+----------+
| 26591107 |
+----------+
+----------+
| count(*) |
+----------+
|  4176206 |
+----------+
+----------+
| count(*) |
+----------+
|  2665094 |
+----------+
+----------+
| count(*) |
+----------+
|   297987 |
+----------+




CREATE TABLE out_degree_sample (
  user_id     int NOT NULL,
  degree      int NOT NULL,
  KEY i_degree_sample_a_u (user_id), 
  KEY i_degree_sample_a_i (degree)
) ENGINE=MyISAM;


INSERT INTO out_degree_sample(user_id, degree) 
SELECT a as user_id, count(*) as degree 
FROM is_friend_of_sample GROUP BY user_id;

CREATE TABLE out_degree_histogram_sample (
  degree int     NOT NULL,
  n      int     NOT NULL
) ENGINE=MyISAM;

INSERT INTO out_degree_histogram_sample(degree, n) 
SELECT degree, count(*) AS n FROM out_degree_sample GROUP BY degree;




CREATE TABLE in_degree_sample (
  user_id     int NOT NULL,
  degree      int NOT NULL,
  KEY i_degree_sample_b_u (user_id), 
  KEY i_degree_sample_b_i (degree)
) ENGINE=MyISAM;


INSERT INTO in_degree_sample(user_id, degree) 
SELECT b as user_id, count(*) as degree 
FROM is_friend_of_sample GROUP BY user_id;

CREATE TABLE in_degree_histogram_sample (
  degree int     NOT NULL,
  n      int     NOT NULL
) ENGINE=MyISAM;

INSERT INTO in_degree_histogram_sample(degree, n) 
SELECT degree, count(*) AS n FROM in_degree_sample GROUP BY degree


SELECT i.degree, o.degree, count(*) 
  FROM user_sample u
INNER JOIN in_degree_sample i  ON u.id = i.user_id
INNER JOIN out_degree_sample o ON u.id = o.user_id
GROUP BY i.degree, o.degree
HAVING i.degree < 5
  OR   o.degree < 5

+----------+
| count(*) |
+----------+
|    42256 |
+----------+


desc interest_sample

+-------------+-------------+------+-----+---------+-------+
| Field       | Type        | Null | Key | Default | Extra |
+-------------+-------------+------+-----+---------+-------+
| id          | int(11)     | NO   | PRI |         |       | 
| original_id | int(11)     | NO   |     |         |       | 
| name        | varchar(80) | NO   | UNI |         |       | 
| count       | int(11)     | YES  |     | 0       |       | 
+-------------+-------------+------+-----+---------+-------+

select count(*) from interest_sample



DROP TABLE IF EXISTS interest_in_degree;
CREATE TABLE interest_in_degree (
  interest_id     int NOT NULL,
  degree          int NOT NULL 
) ENGINE=MyISAM;


CREATE INDEX interest_in_degree_id ON interest_in_degree(interest_id);
CREATE INDEX interest_in_degree_degree ON interest_in_degree(degree);

INSERT INTO interest_in_degree(interest_id, degree) 
SELECT interest_id, count(*) AS degree FROM is_interested_in GROUP BY interest_id;

DROP TABLE IF EXISTS interest_in_degree_histogram;
CREATE TABLE interest_in_degree_histogram (
  degree int     NOT NULL,
  n      int     NOT NULL
) ENGINE=MyISAM;

INSERT INTO interest_in_degree_histogram(degree, n) 
SELECT degree, count(*) AS n FROM interest_in_degree GROUP BY degree;



select count(*) from interest_in_degree

+----------+
| count(*) |
+----------+
|  1979154 | 
+----------+

select * from interest_in_degree limit 10;

+-------------+--------+
| interest_id | degree |
+-------------+--------+
|           0 |      2 | 
|           1 |   3675 | 
|           2 |   3294 | 
|           3 |   1347 | 
|           4 |   1912 | 
|           5 |   7029 | 
|           6 |   5922 | 
|           7 |     18 | 
|           8 |   3002 | 
|           9 |    946 | 
+-------------+--------+

select count(*) from interest_in_degree_histogram

+----------+
| count(*) |
+----------+
|     1803 | 
+----------+


desc is_interested_in

+-------------+---------+------+-----+---------+-------+
| Field       | Type    | Null | Key | Default | Extra |
+-------------+---------+------+-----+---------+-------+
| user_id     | int(11) | NO   | MUL |         |       | 
| interest_id | int(11) | NO   | MUL |         |       | 
+-------------+---------+------+-----+---------+-------+

show create table interest

+----------+------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
| Table    | Create Table                                                                                                                                                                                                           |
+----------+------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
| interest | CREATE TABLE `interest` (
  `id` int(11) NOT NULL,
  `name` varchar(80) NOT NULL,
  `count` int(11) default '0',
  PRIMARY KEY  (`id`),
  UNIQUE KEY `i_interest_name` (`name`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1 | 
+----------+------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+


select 