--/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
-- Script title:	Telephonization_Create_Table
-- Script type:		create
-- Author:			Alexey Kulikov <alexeyqu@gmail.com>, gr. 376, DCAM MIPT
--/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/




--*****************************************************************************
------------------------------------TABLES-------------------------------------
--*****************************************************************************

-- Table: Cities
CREATE TABLE Cities 
(
    Ciid	int				NOT NULL IDENTITY (1, 1) ,
    Ciname	nvarchar(15)	NOT NULL,

    CONSTRAINT Cities_pk PRIMARY KEY (Ciid ASC)
);

-- Table: Equipment
CREATE TABLE Equipment 
(
	Eid			bigint			NOT NULL IDENTITY (1, 1),
    Ename		nvarchar(100)	NOT NULL,
    Eproducer	nvarchar(100)	NOT NULL,

    CONSTRAINT Equipment_pk PRIMARY KEY (Eid ASC)
);

-- Table: OpStations
CREATE TABLE OpStations 
(
    [Sid]	int  NOT NULL,
    Opid	int  NOT NULL,

    CONSTRAINT OpStations_pk PRIMARY KEY 
	(
		[Sid] ASC,
		Opid  ASC
	)
);

-- Table: Operators
CREATE TABLE Operators 
(
    Opid	int				NOT NULL IDENTITY (1, 1) ,
    Opname	nvarchar(15)	NOT NULL,

    CONSTRAINT Operators_pk PRIMARY KEY (Opid ASC)
);

-- Table: StEquip
CREATE TABLE StEquip 
(
    [Sid]	int		NOT NULL,
    Eid		bigint  NOT NULL,
    SEdate	date	NOT NULL CONSTRAINT SEdate_def DEFAULT (getdate()),
    SEquant int		NOT NULL,
	
    CONSTRAINT StEquip_pk PRIMARY KEY 
	(
		[Sid]	ASC,
		Eid		ASC
	)
);

-- Table: Stations
CREATE TABLE Stations 
(
    [Sid]	int  NOT NULL IDENTITY (1, 1) ,
    Scity	int  NOT NULL,
    Stechid int  NOT NULL,
    
	CONSTRAINT Stations_pk PRIMARY KEY ([Sid] ASC)
);

-- Table: Technicians
CREATE TABLE Technicians 
(
    Tpassport	int				NOT NULL IDENTITY (1, 1) ,
    Tname		nvarchar(15)	NOT NULL,
    Tcity		int				NOT	NULL DEFAULT 1,
    Taddress	nvarchar(100)	NOT NULL,
    Tphone		varchar(15)		NOT	NULL,

    CONSTRAINT Technicians_pk PRIMARY KEY (Tpassport ASC)
);

--*****************************************************************************
------------------------------------ALTERS-------------------------------------
--*****************************************************************************

CREATE UNIQUE INDEX OP_NAME ON Operators
(
	 Opname ASC
);

CREATE UNIQUE INDEX PROD_NAME ON Equipment
(
	 Eproducer ASC
);

CREATE UNIQUE INDEX CITY_NAME ON Cities
(
	 Ciid ASC
);

-- Reference:  Equipment_StEquip (table: StEquip)
ALTER TABLE StEquip 
	ADD CONSTRAINT Equipment_StEquip FOREIGN KEY (Eid)
		REFERENCES Equipment (Eid);

-- Reference:  OpStations_Stations (table: OpStations)
ALTER TABLE OpStations 
	ADD CONSTRAINT OpStations_Stations FOREIGN KEY ([Sid])
		REFERENCES Stations ([Sid]);

-- Reference:  Operators_OpStations (table: OpStations)
ALTER TABLE OpStations 
	ADD CONSTRAINT Operators_OpStations FOREIGN KEY (Opid)
		REFERENCES Operators (Opid);

-- Reference:  StEquip_Stations (table: StEquip)
ALTER TABLE StEquip 
	ADD CONSTRAINT StEquip_Stations FOREIGN KEY ([Sid])
		REFERENCES Stations ([Sid]);

-- Reference:  Stations_Cities (table: Stations)
ALTER TABLE Stations 
	ADD CONSTRAINT Stations_Cities FOREIGN KEY (Scity)
		REFERENCES Cities (Ciid);

-- Reference:  Stations_Technicians (table: Stations)
ALTER TABLE Stations 
	ADD CONSTRAINT Stations_Technicians FOREIGN KEY (Stechid)
		REFERENCES Technicians (Tpassport);

-- Reference:  TelSubscribers_Cities (table: Technicians)
ALTER TABLE Technicians 
	ADD CONSTRAINT TelSubscribers_Cities FOREIGN KEY (Tcity)
		REFERENCES Cities (Ciid);

-- Constraint: SEdate (table: StEquip)
ALTER TABLE StEquip
	ADD CONSTRAINT SEdate_OK
		CHECK ((SEdate > '1970-01-01') AND (SEdate <= getdate()));

-- Constraint: SEquant (table: StEquip)
ALTER TABLE StEquip
	ADD CONSTRAINT SEquant_OK
		CHECK (SEquant >= 0);

--*****************************************************************************
------------------------------------THEEND-------------------------------------
--*****************************************************************************