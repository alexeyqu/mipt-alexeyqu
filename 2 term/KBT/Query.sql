--/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
-- Script title:	Telephonization_Query
-- Script type:		select/modify
-- Author:			Alexey Kulikov <alexeyqu@gmail.com>, gr. 376, DCAM MIPT
--/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
	
--SELECTS

/** Operator that uses max of stations
SELECT TOP (1) WITH TIES O.Opid, O.Opname, COUNT (OS.[Sid]) AS NofStations
FROM Operators AS O JOIN OpStations AS OS
	ON O.Opid = OS.Opid
GROUP BY O.Opid, O.Opname
ORDER BY NofStations DESC;
*/

/** Most overloaded techinician
SELECT TOP (1) WITH TIES T.Tpassport, T.Tname, COUNT (S.[Sid]) AS NofStations
FROM Technicians AS T JOIN Stations AS S
	ON T.Tpassport = S.Stechid
GROUP BY T.Tpassport, T.Tname
ORDER BY NofStations DESC;
*/

/** City with lessmost num of stations
SELECT TOP (1) WITH TIES C.ciname AS city, COUNT (S.[Sid])
FROM Cities AS C LEFT OUTER JOIN Stations AS S
	ON S.Scity = C.Ciid
GROUP BY C.ciid, C.Ciname
ORDER BY COUNT (S.[Sid]) ASC;
*/

/** Technicians that live and work in different cities
SELECT T.Tpassport, T.Tname, T.Tcity AS Lives, S.Scity AS Works
FROM Technicians AS T JOIN Stations AS S
	ON S.Stechid = T.Tpassport JOIN Cities AS C
		ON C.Ciid = S.Scity
WHERE T.Tcity <> S.Scity;
*/

/** Equipment that manques or old
SELECT E.Eid, E.Ename, S.[Sid], SE.SEdate, SE.SEquant
FROM Equipment AS E JOIN StEquip AS SE
	ON SE.Eid = E.Eid JOIN Stations AS S
		ON SE.[Sid] = S.[Sid]
WHERE SE.SEquant < 10 
	OR datediff (day, SE.SEdate, getdate()) > 500;
*/

/** The coolest operator for each city
SELECT D.ciid, D.ciname, D.opn2ame, MAXTATIONS.Nofstations
FROM 
(
	SELECT ISNULL (MAX (T.Nofstations), 0) AS Nofstations, T.ciid
	FROM 
	(
		SELECT O.Opname, COUNT (S.[Sid]) AS Nofstations, C.ciname, C.ciid
		FROM Operators AS O JOIN OpStations AS OS
			ON O.Opid = OS.Opid JOIN Stations AS S
				ON OS.[Sid] = S.[Sid] JOIN Cities AS C
					ON S.Scity = C.Ciid
		GROUP BY C.Ciid, C.Ciname, O.Opname
	) AS T
	GROUP BY T.ciname, T.ciid
) AS MAXTATIONS
JOIN
(
	SELECT O.Opname, COUNT (S.[Sid]) AS Nofstations, C.ciname, C.ciid
	FROM Operators AS O JOIN OpStations AS OS
		ON O.Opid = OS.Opid JOIN Stations AS S
			ON OS.[Sid] = S.[Sid] JOIN Cities AS C
				ON S.Scity = C.Ciid
	GROUP BY C.Ciid, C.Ciname, O.Opname
) AS D
	ON MAXTATIONS.ciid = D.ciid
WHERE MAXTATIONS.Nofstations = D.Nofstations
ORDER BY D.ciid;
*/

/** Total apple equipment
SELECT ISNULL (SUM (SE.SEquant), 0) AS [Apple equipment]
FROM Equipment AS E JOIN StEquip AS SE
	ON E.Eid = SE.Eid
WHERE E.Eproducer = 'OOO AAApple';
*/

--UPDATES

/** Delete unemployed techs
DELETE FROM Technicians 
	WHERE Technicians.Tpassport NOT IN 
	(
		SELECT T.Tpassport
		FROM Technicians AS T JOIN Stations AS S
		ON T.Tpassport = S.Stechid
	);
*/

/** Use First 5 letters of producer name
UPDATE Equipment SET
	Eproducer = SUBSTRING (Eproducer, 1, 5);

SELECT * FROM Equipment;
*/

--CONSTRAINT VIOLATIONS

/** Eq date
UPDATE StEquip SET 
	SEdate = '1721-1-1' WHERE Eid = 1 AND Sid = 2;
	
UPDATE StEquip SET 
	SEdate = '3213-12-31' WHERE Eid = 1 AND Sid = 2;

SELECT * FROM StEquip;
*/

/** Unique opname
UPDATE Operators SET Opname = 'MTC' WHERE Opid = 2;

SELECT * FROM Operators;
*/

/** Delete foreign key
DELETE FROM Operators 
	WHERE Operators.Opname = 'MTC';
*/

--EXTRA

/**
SELECT * FROM Technicians;
*/