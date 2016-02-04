--/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
-- Script title:	Telephonization_Change_Datatypes
-- Script type:		modify
-- Author:			Alexey Kulikov <alexeyqu@gmail.com>, gr. 376, DCAM MIPT
--/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/

ALTER TABLE Technicians
	ALTER COLUMN Tphone varchar(15);

--/////////////////////////////////////////////////////////////////////////////

ALTER TABLE StEquip -- drop foreign key
	DROP CONSTRAINT Equipment_StEquip;
	
ALTER TABLE Equipment -- drop primary key 1
	DROP CONSTRAINT Equipment_pk;

ALTER TABLE StEquip -- drop primary key 2
	DROP CONSTRAINT StEquip_pk;

ALTER TABLE Equipment -- change datatype
	ALTER COLUMN Eid bigint NOT NULL;

ALTER TABLE StEquip 
	ALTER COLUMN Eid bigint NOT NULL;

ALTER TABLE Equipment -- set primary key 1
	ADD CONSTRAINT Equipment_pk PRIMARY KEY (Eid ASC);

ALTER TABLE StEquip -- set primary key 2
	ADD CONSTRAINT StEquip_pk PRIMARY KEY 
	(
		[Sid]	ASC,
		Eid		ASC
	);

ALTER TABLE StEquip -- set foreign key
	ADD CONSTRAINT Equipment_StEquip FOREIGN KEY (Eid)
		REFERENCES Equipment (Eid);