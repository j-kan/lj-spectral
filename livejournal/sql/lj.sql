create database lj

use lj;

CREATE TABLE friend_of (
  a varchar(16) NOT NULL,
  b varchar(16) NOT NULL
) ENGINE=MyISAM


CREATE INDEX index_friend_of_a ON friend_of(a);
CREATE INDEX index_friend_of_b ON friend_of(b);


DROP TABLE IF EXISTS out_degree;
CREATE TABLE out_degree (
  a      int NOT NULL,
  degree int NOT NULL 
) ENGINE=MyISAM;


INSERT INTO out_degree(a, degree) 
SELECT a, count(*) AS out_degree FROM is_friend_of GROUP BY a



DROP TABLE IF EXISTS out_degree_histogram;
CREATE TABLE out_degree_histogram (
  degree int     NOT NULL,
  n      int     NOT NULL
) ENGINE=MyISAM;

INSERT INTO out_degree_histogram(degree, n) 
SELECT degree, count(*) AS n FROM out_degree GROUP BY degree;




DROP TABLE IF EXISTS in_degree;
CREATE TABLE in_degree (
  b      int NOT NULL,
  degree int     NOT NULL 
) ENGINE=MyISAM;


INSERT INTO in_degree(b, degree) 
SELECT b, count(*) AS in_degree FROM is_friend_of GROUP BY b;


DROP TABLE IF EXISTS in_degree_histogram;
CREATE TABLE in_degree_histogram (
  degree int     NOT NULL,
  n      int     NOT NULL
) ENGINE=MyISAM;

INSERT INTO in_degree_histogram(degree, n) 
SELECT degree, count(*) AS n FROM in_degree GROUP BY degree










SELECT COUNT(*) FROM out_degree_histogram;
SELECT COUNT(*) FROM in_degree_histogram;

+----------+
| COUNT(*) |
+----------+
|      569 | 
+----------+
+----------+
| COUNT(*) |
+----------+
|      653 | 
+----------+


+----------+
| COUNT(*) |
+----------+
|      683 | 
+----------+
+----------+
| COUNT(*) |
+----------+
|      771 | 
+----------+

+----------+
| COUNT(*) |
+----------+
|      683 | 
+----------+

+----------+
| COUNT(*) |
+----------+
|   613783 | 
+----------+

select count(*) from lj.friend_of

+----------+
| count(*) |
+----------+
|     1737 | 
+----------+

+----------+
| count(*) |
+----------+
|      590 | 
+----------+

+-------+-------------+------+-----+---------+-------+
| Field | Type        | Null | Key | Default | Extra |
+-------+-------------+------+-----+---------+-------+
| a     | varchar(16) | NO   |     |         |       | 
| b     | varchar(16) | NO   |     |         |       | 
+-------+-------------+------+-----+---------+-------+

select count(*) from friend_of where a = b

+----------+
| count(*) |
+----------+
|       66 | 
+----------+
select count(*) from friend_of group by a


+-------+----------------+
| a     | b              |
+-------+----------------+
| deese | deese          | 
| deese | gerneith       | 
| deese | annabelle_blue | 
+-------+----------------+

insert into friend_of values ('deese','deese')

select * from friend_of where a = 'deese'

+-------+----------------+
| a     | b              |
+-------+----------------+
| deese | gerneith       | 
| deese | annabelle_blue | 
+-------+----------------+

+-------+----------------+
| a     | b              |
+-------+----------------+
| deese | deese          | 
| deese | gerneith       | 
| deese | annabelle_blue | 
| deese | deese          | 
+-------+----------------+

select a, b, count(*) n from friend_of group by a, b having n > 1


delete from friend_of where a = 'deese' and a = b


select * from friend_of 

+---------+-----------------+
| a       | b               |
+---------+-----------------+
| craig14 | xbroken__starsx | 
+---------+-----------------+

delete from friend_of



      SELECT foa.b 
        FROM friend_of foa LEFT JOIN friend_of fob ON foa.b = fob.a
       WHERE fob.b IS NULL
    GROUP BY foa.b



SELECT a FROM (SELECT a, count(*) AS a_cnt FROM friend_of group by a having a_cnt > 100);





show databases

+--------------------+
| Database           |
+--------------------+
 use information_schema;
 show tables

+---------------------------------------+
| Tables_in_information_schema          |
+---------------------------------------+
| CHARACTER_SETS                        | 
| COLLATIONS                            | 
| COLLATION_CHARACTER_SET_APPLICABILITY | 
| COLUMNS                               | 
| COLUMN_PRIVILEGES                     | 
| KEY_COLUMN_USAGE                      | 
| PROFILING                             | 
| ROUTINES                              | 
| SCHEMATA                              | 
| SCHEMA_PRIVILEGES                     | 
| STATISTICS                            | 
| TABLES                                | 
| TABLE_CONSTRAINTS                     | 
| TABLE_PRIVILEGES                      | 
| TRIGGERS                              | 
| USER_PRIVILEGES                       | 
| VIEWS                                 | 
+---------------------------------------+
 
| lj                 | 
| mysql              | 
| nightingale        | 
| test               | 
+--------------------+

desc user

+---------+-------------+------+-----+---------+----------------+
| Field   | Type        | Null | Key | Default | Extra          |
+---------+-------------+------+-----+---------+----------------+
| id      | int(11)     | NO   | PRI | NULL    | auto_increment | 
| name    | varchar(16) | NO   | UNI |         |                | 
| visited | bit(1)      | YES  |     |         |                | 
+---------+-------------+------+-----+---------+----------------+


truncate table user;

insert into user (name,visited) values ('blah',true),('blah2',true) ON DUPLICATE KEY UPDATE name = name;
select * from user where not visited;


+----+-------+---------+
| id | name  | visited |
+----+-------+---------+
|  1 | blah  |        | 
|  3 | blah2 |        | 
+----+-------+---------+


SELECT name FROM is_friend_of LEFT JOIN user ON b = user.id WHERE a = 1

+-------+
| name  |
+-------+
| blah  | 
| blah2 | 
+-------+

+---+
| b |
+---+
| 1 | 
| 3 | 
+---+

desc is_friend_of

+-------+---------+------+-----+---------+-------+
| Field | Type    | Null | Key | Default | Extra |
+-------+---------+------+-----+---------+-------+
| a     | int(11) | NO   | MUL |         |       | 
| b     | int(11) | NO   | MUL |         |       | 
+-------+---------+------+-----+---------+-------+

insert into is_friend_of(a,b) 
select 1, id from user where name in ('blah','blah2')

select * from is_friend_of

insert into user (name) values ('blah') ON DUPLICATE KEY UPDATE name = 'blah';
select * from user;
insert into user (name) values ('blah') ON DUPLICATE KEY UPDATE name = 'blah';
select * from user;

truncate table is_friend_of;
truncate table user;

select * from is_friend_of;
select * from user;


select * from interest


select * from is_interested_in




select * from interest where id not in (select distinct interest_id from is_interested_in)

select * from is_interested_in where user_id = 224

+-----+----------+---------+
| id  | name     | visited |
+-----+----------+---------+
| 224 | uberjude |        | 
+-----+----------+---------+


+-----+-----------+---------+
| id  | name      | visited |
+-----+-----------+---------+
| 222 | zoomardav |        | 
+-----+-----------+---------+


select * from user where id not in (select distinct a from is_friend_of union select distinct b from is_friend_of )

select min(id) from user where id > 57355

select count(*), visited from user group by (visited = true)



CREATE TABLE tmp (
  id INT NOT NULL AUTO_INCREMENT PRIMARY KEY,
  name varchar(16) NOT NULL,
  visited bit default 0,
  CONSTRAINT UNIQUE KEY i_tmp_name (name),
  KEY i_tmp_visited (visited)
) ENGINE=MyISAM;




insert into tmp(name, visited) values('blargh',1),('urgh',0),('eeow',0)

delete from tmp where name like 'urg%'


update tmp set id=2 where id=3;
update tmp set id=3 where id=4;
update tmp set id=4 where id=6


select min(id) from user where id > 3


select * from tmp

+----+--------+---------+
| id | name   | visited |
+----+--------+---------+
|  1 | blah   |        | 
|  2 | eew    |         | 
|  3 | blargh |        | 
|  4 | eeow   |         | 
+----+--------+---------+

+----+--------+---------+
| id | name   | visited |
+----+--------+---------+
|  1 | blah   |        | 
|  2 | eew    |         | 
|  4 | blargh |        | 
|  6 | eeow   |         | 
+----+--------+---------+

+----+--------+---------+
| id | name   | visited |
+----+--------+---------+
|  1 | blah   |        | 
|  3 | eew    |         | 
|  4 | blargh |        | 
|  6 | eeow   |         | 
+----+--------+---------+

+----+--------+---------+
| id | name   | visited |
+----+--------+---------+
|  1 | blah   |        | 
|  2 | urg    |         | 
|  3 | eew    |         | 
|  4 | blargh |        | 
|  5 | urgh   |         | 
|  6 | eeow   |         | 
+----+--------+---------+
