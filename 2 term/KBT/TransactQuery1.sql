--/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
-- Script title:	Telephonization_Transact1
-- Script type:		select/update
-- Author:			Alexey Kulikov <alexeyqu@gmail.com>, gr. 376, DCAM MIPT
--/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
	
/** 1 LOST UPDATE

SET TRANSACTION ISOLATION LEVEL READ UNCOMMITTED

BEGIN TRANSACTION

UPDATE Technicians 
	SET Tcity = Tcity + 1 WHERE Tpassport = 1000000001

COMMIT

SELECT Tcity
FROM Technicians
WHERE Tpassport = 1000000001
	
SELECT @@TRANCOUNT

*/

/** 2 DIRTY READ

SET TRANSACTION ISOLATION LEVEL READ UNCOMMITTED

BEGIN TRANSACTION

SELECT Tcity FROM Technicians WHERE Tpassport = 1000000001

SELECT Tcity FROM Technicians WHERE Tpassport = 1000000001

SELECT Tcity FROM Technicians WHERE Tpassport = 1000000001

COMMIT

*/

/** 3 NON-REPEATABLE READ

SET TRANSACTION ISOLATION LEVEL REPEATABLE READ

BEGIN TRANSACTION

SELECT Tcity FROM Technicians WHERE Tpassport = 1000000001

SELECT Tcity FROM Technicians WHERE Tpassport = 1000000001

COMMIT

*/

/** 4 PHANTOM INSERT

SET TRANSACTION ISOLATION LEVEL SERIALIZABLE

BEGIN TRANSACTION

SELECT * FROM Technicians WITH (TABLOCK) WHERE Tpassport <= 1000000001

SELECT * FROM Technicians WHERE Tpassport <= 1000000001

COMMIT

*/

/** 5 REPEATABLE READ INF WHILES

SET TRANSACTION ISOLATION LEVEL REPEATABLE READ

BEGIN TRANSACTION

SELECT * FROM Technicians WHERE Tpassport = 1000000002

UPDATE Technicians 
	SET Tcity = 8 WHERE Tpassport = 1000000001

COMMIT

*/

/** LOST UPDATE - 2

DROP TABLE tmp;

CREATE TABLE tmp (id int, tmp_value int)
INSERT INTO tmp VALUES (1,0)

GO

SELECT * FROM tmp

SET TRANSACTION ISOLATION LEVEL READ UNCOMMITTED

BEGIN TRANSACTION

UPDATE tmp 
	SET tmp_value = 
		(SELECT Tcity FROM Technicians WHERE Tpassport = 1000000003)
	WHERE id = 1
            
GO

SELECT * FROM tmp
	
UPDATE Technicians 
	SET Tcity = (SELECT tmp_value FROM tmp WHERE id = 1)
    WHERE Tpassport = 1000000003

COMMIT

SELECT *
FROM Technicians
WHERE Tpassport = 1000000003
*/