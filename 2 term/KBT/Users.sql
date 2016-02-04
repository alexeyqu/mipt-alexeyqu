--/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
-- Script title:	Telephonization_Users
-- Script type:		users
-- Author:			Alexey Kulikov <alexeyqu@gmail.com>, gr. 376, DCAM MIPT
--/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
	
--CREATE USER

USE Telephonization;

DROP LOGIN Test;
DROP USER TestU1;

CREATE LOGIN Test WITH PASSWORD = 'qwerty';

CREATE USER TestU1 FOR LOGIN Test;

--GRANT HIM SOME RIGHTS

GRANT SELECT, INSERT, UPDATE ON Technicians TO TestU1;
GRANT SELECT, INSERT, UPDATE ON Stations TO TestU1;

GRANT SELECT (Opname), UPDATE (Opname) ON Operators TO TestU1;
REVOKE SELECT ON Operators TO TestU1;

GRANT SELECT ON Equipment TO TestU1;

--TRY THEM NOW ON REGULAR TABLES

--HERE HE HAS RIGHTS TO

EXECUTE AS USER = 'TestU1'
GO
	SELECT TOP (1) WITH TIES T.Tpassport, T.Tname, COUNT (S.[Sid]) AS NofStations
	FROM Technicians AS T JOIN Stations AS S
		ON T.Tpassport = S.Stechid
	GROUP BY T.Tpassport, T.Tname
	ORDER BY NofStations DESC;
GO
REVERT

EXECUTE AS USER = 'TestU1'
GO
	SELECT Opid FROM Operators WHERE Opid = 2;
GO
	UPDATE Operators SET Opname = 'MTC' WHERE Opid = 2;
GO
REVERT

EXECUTE AS USER = 'TestU1'
GO
	SELECT * FROM Equipment;
GO
	UPDATE Equipment SET Eproducer = 'MTC' WHERE Eid = 2; -- FAILURE HERE :(
GO
REVERT

--AND HERE DOESN'T :(

EXECUTE AS USER = 'TestU1'
GO
	DELETE FROM Operators 
		WHERE Operators.Opname = 'MTC';
GO
REVERT

EXECUTE AS USER = 'TestU1';
GO
	SELECT TOP (1) WITH TIES O.Opid, O.Opname, COUNT (OS.[Sid]) AS NofStations
	FROM Operators AS O JOIN OpStations AS OS
		ON O.Opid = OS.Opid
	GROUP BY O.Opid, O.Opname
	ORDER BY NofStations DESC;
GO
REVERT

--NOW TRY VIEWS

IF EXISTS
(
	SELECT * FROM sys.views
	WHERE [name] = 'OpStats' AND
		schema_id = SCHEMA_ID ('dbo')
)
DROP VIEW OpStats
GO
CREATE VIEW OpStats AS
(
	SELECT O.Opname, COUNT (S.[Sid]) AS Nofstations, C.ciname, C.ciid
	FROM Operators AS O JOIN OpStations AS OS
		ON O.Opid = OS.Opid JOIN Stations AS S
			ON OS.[Sid] = S.[Sid] JOIN Cities AS C
				ON S.Scity = C.Ciid
	GROUP BY C.Ciid, C.Ciname, O.Opname
)

GRANT SELECT ON Opstats TO TestU1;
REVOKE SELECT ON Opstats TO TestU1;

EXECUTE AS USER = 'TestU1';
GO
SELECT D.ciid, D.ciname, D.opname, MAXTATIONS.Nofstations
FROM 
(
	SELECT ISNULL (MAX (T.Nofstations), 0) AS Nofstations, T.ciid
	FROM OpStats AS T
	GROUP BY T.ciname, T.ciid
) AS MAXTATIONS
JOIN OpStats AS D
	ON MAXTATIONS.ciid = D.ciid
WHERE MAXTATIONS.Nofstations = D.Nofstations
ORDER BY D.ciid;
GO
REVERT

IF EXISTS
(
	SELECT * FROM sys.views
	WHERE [name] = 'EqStats' AND
		schema_id = SCHEMA_ID ('dbo')
)
DROP VIEW EqStats
GO
CREATE VIEW EqStats AS 
(
	SELECT S.Sid AS [Station ID],
		S.Scity AS [Station City], 
		S.Stechid AS [Technician ID],
		E.Eid AS [Equipment ID],
		SE.SEdate AS [Date], 
		SE.SEquant AS Quantity
	FROM Stations AS S 
			LEFT OUTER JOIN StEquip AS SE
					ON S.Sid = SE.Sid 
			LEFT OUTER JOIN Equipment AS E
					ON SE.Eid = E.Eid
	WHERE datediff (day, SE.SEdate, getdate()) > 500 OR
		SE.SEquant < 10 
)
GO

EXECUTE AS USER = 'TestU1';
GO
SELECT * FROM EqStats
GO
REVERT

--OK, NOW LET'S CREATE A NEW ROLE

USE Telephonization

ALTER ROLE TestRole DROP MEMBER TestU1
DROP ROLE TestRole

CREATE ROLE TestRole

ALTER ROLE TestRole ADD MEMBER TestU1; 

REVOKE SELECT ON Technicians TO TestRole;

EXECUTE AS USER = 'TestU1'
GO
	SELECT TOP (1) WITH TIES T.Tpassport, T.Tname, COUNT (S.[Sid]) AS NofStations
	FROM Technicians AS T JOIN Stations AS S
		ON T.Tpassport = S.Stechid
	GROUP BY T.Tpassport, T.Tname
	ORDER BY NofStations DESC;
GO
REVERT

EXECUTE AS USER = 'TestU1'
GO
DELETE FROM Operators 
	WHERE Operators.Opname = 'MTC'; --FAIL
GO
UPDATE Operators SET Opname = 'MTC' WHERE Opid = 2;
GO
REVERT

--TEST

ALTER ROLE TestRole DROP MEMBER TestU1; 

DENY SELECT ON Technicians TO TestRole;
DENY SELECT ON Technicians TO TestU1;
REVOKE SELECT ON Technicians TO TestU1;

ALTER ROLE TestRole ADD MEMBER TestU1; 

GRANT SELECT(Tpassport) ON Technicians TO TestU1;
GRANT SELECT(Tpassport) ON Technicians TO TestRole;

EXECUTE AS USER = 'TestU1'
GO
SELECT Tpassport/*, Tcity -- doesn't work!*/ FROM Technicians
GO
REVERT
