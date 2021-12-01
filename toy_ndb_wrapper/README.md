# schema 

```sql
create database test_db;

CREATE TABLE `test_table` (                                           
  `ATTR1` int NOT NULL,                                               
  `ATTR2` int NOT NULL,                                               
  PRIMARY KEY (`ATTR1`)                                               
) ENGINE=ndbcluster DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci
```


# build cpp example 

Assumes rondb is installed at /usr/local/mysql. 
TODO...


