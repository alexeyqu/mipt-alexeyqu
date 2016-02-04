--/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
-- Script title:	Telephonization_Transact2
-- Script type:		select/update
-- Author:			Alexey Kulikov <alexeyqu@gmail.com>, gr. 376, DCAM MIPT
--/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
	/** 1 LOST UPDATE

SET TRANSACTION ISOLATION LEVEL READ UNCOMMITTED

BEGIN TRANSACTION

UPDATE Technicians 
	SET Tcity = Tcity + 1 WHERE Tpassport = 1000000001

COMMIT

*/

/** 2 DIRTY READ

SELECT * FROM Technicians WHERE Tpassport = 1000000001

SET TRANSACTION ISOLATION LEVEL REPEATABLE READ

BEGIN TRANSACTION

UPDATE Technicians 
	SET Tcity += 1 WHERE Tpassport = 1000000001

SAVE TRANSACTION My_Save;

BEGIN TRANSACTION

UPDATE Technicians 
	SET Tcity += 2 WHERE Tpassport = 1000000001

ROLLBACK TRANSACTION My_Save;

UPDATE Technicians 
	SET Tcity += 3 WHERE Tpassport = 1000000001

COMMIT

SELECT * FROM Technicians WHERE Tpassport = 1000000001
*/

/** 3 NON-REPEATABLE READ

SET TRANSACTION ISOLATION LEVEL REPEATABLE READ

BEGIN TRANSACTION

UPDATE Technicians 
	SET Tcity = 1 WHERE Tpassport = 1000000001

COMMIT

*/

/** 4 PHANTOM INSERT

SET TRANSACTION ISOLATION LEVEL SERIALIZABLE

BEGIN TRANSACTION

SET IDENTITY_INSERT Technicians ON

INSERT INTO Technicians (Tpassport, Tname, Tcity, Taddress, Tphone)
	 VALUES (666, 'Lucifer', 3, 'Hell', 12345)

SET IDENTITY_INSERT Technicians OFF

COMMIT

*/

/** 5 REPEATABLE READ INF WHILES

SET TRANSACTION ISOLATION LEVEL REPEATABLE READ

BEGIN TRANSACTION

SELECT * FROM Technicians WHERE Tpassport = 1000000001

UPDATE Technicians 
	SET Tcity = 9 WHERE Tpassport = 1000000002

COMMIT

*/

/** LOST UPDATE - 2

SELECT *
FROM Technicians
WHERE Tpassport = 1000000003

SET TRANSACTION ISOLATION LEVEL READ UNCOMMITTED

BEGIN TRANSACTION

UPDATE Technicians 
	SET Tcity = Tcity + 1 
	WHERE Tpassport = 1000000003

COMMIT

SELECT *
FROM Technicians
WHERE Tpassport = 1000000003