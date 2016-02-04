--/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
-- Script title:	Telephonization_Triggers
-- Script type:		create
-- Author:			Alexey Kulikov <alexeyqu@gmail.com>, gr. 376, DCAM MIPT
--/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
	
DROP TABLE TechFired;

CREATE TABLE TechFired
(
	TFid		int				NOT NULL IDENTITY (1, 1),
    Tpassport	int				NOT NULL,
    Tname		nvarchar(15)	NOT NULL,
    Tcity		int				NOT	NULL DEFAULT 1,
    Taddress	nvarchar(100)	NOT NULL,
    Tphone		varchar(15)		NOT	NULL,
	FireDate	date			NOT NULL,

    CONSTRAINT TechFired_pk PRIMARY KEY (TFid ASC)
);

DROP TRIGGER TTechFired;

CREATE TRIGGER TTechFired
ON Technicians
AFTER DELETE
AS
BEGIN
	INSERT INTO TechFired (Tpassport, Tname, Tcity, Taddress, Tphone, FireDate)
		SELECT Tpassport, Tname, Tcity, Taddress, Tphone, getdate()
		FROM DELETED
END

SELECT * FROM TechFired;
SELECT * FROM Technicians;

DELETE FROM Technicians WHERE Tpassport = 2;
SELECT * FROM TechFired;
SELECT * FROM Technicians;

begin tran
SELECT count(*) FROM TechFired;
DELETE FROM Technicians WHERE Tpassport < 10;
SET IDENTITY_INSERT Technicians ON;
INSERT INTO Technicians (Tpassport, Tname, Tcity, Taddress, Tphone )
		SELECT 1, '1', 1, '1', '1'
SET IDENTITY_INSERT Technicians OFF;
DELETE FROM Technicians WHERE Tpassport < 10;
SELECT * FROM TechFired;
--SELECT * FROM Technicians;
rollback

SET IDENTITY_INSERT TechFired ON
INSERT INTO TechFired (Tpassport, Tname, Tcity, Taddress, Tphone, FireDate)
	SELECT Tpassport, Tname, Tcity, Taddress, Tphone, getdate()
	FROM Technicians
	WHERE Tpassport = 1
SET IDENTITY_INSERT TechFired OFF
