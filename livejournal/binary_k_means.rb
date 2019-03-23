=begin
 Binary distance metric:  

       |FA^FB| 
   -----------------
   |FA+FB| - |FA^FB|
  
   where FA = { friends_of(A) }, FB = { friends_of(B) }
 


DROP TABLE IF EXISTS shared_in_count;

CREATE TABLE shared_in_count (
  user_x_id int NOT NULL,
  user_y_id int NOT NULL,
  count     int NOT NULL default 0
) ENGINE=MyISAM;



-- everybody is their own friend to make the calculations easier

INSERT INTO is_friend_of 
SELECT id, id FROM user

DELETE FROM is_friend_of 
WHERE a = b



INSERT INTO  shared_in_count
SELECT       x.a AS user_x_id, y.a AS user_y_id, COUNT(*) AS count
FROM         is_friend_of x
INNER JOIN   is_friend_of y  ON x.b = y.b
WHERE        x.a != y.a
AND          5000 <= x.a AND x.a < 10000
GROUP BY     x.a, y.a
HAVING       count > 2


+----+-------------+-------+-------+-----------------------------------+------------------+---------+--------+--------+----------------------------------------------+
| id | select_type | table | type  | possible_keys                     | key              | key_len | ref    | rows   | Extra                                        |
+----+-------------+-------+-------+-----------------------------------+------------------+---------+--------+--------+----------------------------------------------+
|  1 | SIMPLE      | x     | range | i_is_friend_of_a,i_is_friend_of_b | i_is_friend_of_a | 4       | NULL   | 126102 | Using where; Using temporary; Using filesort | 
|  1 | SIMPLE      | y     | ref   | i_is_friend_of_b                  | i_is_friend_of_b | 4       | lj.x.b |    135 | Using where                                  | 
+----+-------------+-------+-------+-----------------------------------+------------------+---------+--------+--------+----------------------------------------------+




desc shared_in_count;

+-----------+---------+------+-----+---------+-------+
| Field     | Type    | Null | Key | Default | Extra |
+-----------+---------+------+-----+---------+-------+
| user_x_id | int(11) | NO   |     |         |       | 
| user_y_id | int(11) | NO   |     |         |       | 
| count     | int(11) | NO   |     | 0       |       | 
+-----------+---------+------+-----+---------+-------+


SELECT count(*) from shared_in_count;




+---+---+------+------+
| a | b | a    | b    |
+---+---+------+------+
| 1 | 1 |    2 |    1 | 
| 1 | 2 |    2 |    2 | 
+---+---+------+------+

+---+-----+------+------+
| a | b   | a    | b    |
+---+-----+------+------+
| 2 |   1 |    3 |    1 | 
| 2 | 172 |    3 |  172 | 
| 2 | 183 |    3 |  183 | 
+---+-----+------+------+

+---+-----+------+------+
| a | b   | a    | b    |
+---+-----+------+------+
| 2 |   1 |    3 |    1 | 
| 2 | 172 |    3 |  172 | 
| 2 | 183 |    3 |  183 | 
+---+-----+------+------+


SELECT * 
FROM      is_friend_of x
WHERE a = 1 AND b = 2
OR    b = 1 AND a = 2

+---+---+
| a | b |
+---+---+
| 1 | 2 | 
| 2 | 1 | 
+---+---+


where a = 1

select * from is_friend_of where a = 1
select * from is_friend_of left join user ON b = id where a = 2
select * from is_friend_of left join user ON b = id where a = 3

=end


require 'mysql-helper.rb'

