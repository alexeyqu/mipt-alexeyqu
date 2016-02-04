--/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
-- Script title:	Telephonization_Views
-- Script type:		select
-- Author:			Alexey Kulikov <alexeyqu@gmail.com>, gr. 376, DCAM MIPT
--/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
	
/** Station, city, n of techs, n of ops
IF EXISTS
(
	SELECT * FROM sys.views
	WHERE [name] = 'StationStats' AND
		schema_id = SCHEMA_ID ('dbo')
)
DROP VIEW StationStats
GO
CREATE VIEW StationStats AS
select Cities.Ciname, tmp.NofTechnicians
from Cities join
(
	SELECT S.Sid AS [Station ID], 
		C.Ciname AS [City name], 
		ISNULL (COUNT (T.Tpassport), 0) AS NofTechnicians, 
		ISNULL (COUNT (OS.Opid), 0) AS NofOperators
	FROM Stations AS S JOIN Cities AS C
		ON S.Scity = C.Ciid LEFT OUTER JOIN Technicians AS T
			ON T.Tpassport = S.Stechid LEFT OUTER JOIN OpStations AS OS
				ON OS.Sid = S.Sid
	GROUP BY S.Sid, C.Ciid, C.Ciname
) tmp on tmp.[City name] = Cities.Ciname
where tmp.NofTechnicians > 2
WITH CHECK OPTION
GO
SELECT * FROM StationStats
-- IT IS NOT UPDATABLE
GO
INSERT INTO StationStats ([Station ID], [City name], NofTechnicians, NofOperators)
	VALUES (1, 1, 100, 100)
GO
SELECT * FROM StationStats
*/

/** Station, Equipment, Date, Quant 
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

SET IDENTITY_INSERT Stations ON
INSERT INTO EqStats ([Station ID], [Station City], [Technician ID])
	VALUES (100, 1, 1000000001)
SET IDENTITY_INSERT Stations OFF

SELECT * FROM EqStats
GO -- UPDATABLE!!!
UPDATE EqStats 
	SET Date = getdate () WHERE Quantity = 5
GO
SELECT * FROM EqStats
*/

/** Coolest operator
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
*/

/** Cities UPDATABLE
IF EXISTS
(
	SELECT * FROM sys.views
	WHERE [name] = 'CitiesW' AND
		schema_id = SCHEMA_ID ('dbo')
)
DROP VIEW CitiesW
GO
IF EXISTS
(
	SELECT * FROM sys.views
	WHERE [name] = 'CitiesWrap' AND
		schema_id = SCHEMA_ID ('dbo')
)
DROP VIEW CitiesWrap
GO
CREATE VIEW CitiesW AS
(
	SELECT C.Ciid, C.Ciname
	FROM Cities AS C
	WHERE Ciid < 5
)
WITH CHECK OPTION
GO
CREATE VIEW CitiesWrap AS
(
	SELECT *
	FROM CitiesW
)
GO
SET IDENTITY_INSERT Cities ON
INSERT INTO CitiesWrap (Ciid, Ciname)
	VALUES (100, 'TEST')
SET IDENTITY_INSERT Cities OFF
GO
SELECT * FROM Cities
*/

IF EXISTS
(
	SELECT * FROM sys.views
	WHERE [name] = 'Eq' AND
		schema_id = SCHEMA_ID ('dbo')
)
DROP VIEW Eq
GO
CREATE VIEW Eq AS
(
	SELECT E.Eid AS [Eq ID], 
		E.Ename AS [Eq name],
		E.Eproducer AS [Eq prod],
		SE.Eid AS [Eq 2 ID],
		SE.[Sid] AS [Stat ID],
		SE.SEdate AS [Date],
		SE.SEquant AS Quantity
	FROM Equipment AS E
		JOIN StEquip AS SE
				ON E.Eid = SE.Eid
)
GO
SELECT * FROM Eq
GO
SET IDENTITY_INSERT Equipment ON
INSERT INTO Eq ([Eq ID], [Eq name], [Eq prod])
	VALUES (22, 'TEST', 'TESTPROD')
INSERT INTO Eq ([Eq 2 ID], [Stat ID], [Date], [Quantity])
	VALUES (22, 1, getdate(), 100)
SET IDENTITY_INSERT Equipment OFF
GO
SELECT * FROM Eq
SELECT * FROM Equipment
SELECT * FROM StEquip