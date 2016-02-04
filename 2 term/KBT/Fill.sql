--/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
-- Script title:	Telephonization_Fill
-- Script type:		fill
-- Author:			Alexey Kulikov <alexeyqu@gmail.com>, gr. 376, DCAM MIPT
--/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
	
DELETE FROM OpStations
DELETE FROM StEquip
DELETE FROM Operators
DELETE FROM Equipment
DELETE FROM Stations
DELETE FROM Technicians
DELETE FROM Cities

SET IDENTITY_INSERT Cities ON
INSERT Cities (Ciid, Ciname) VALUES (1,  'Moscow')
INSERT Cities (Ciid, Ciname) VALUES (2,  'StPetersburg')
INSERT Cities (Ciid, Ciname) VALUES (3,  'Taganrog')
INSERT Cities (Ciid, Ciname) VALUES (4,  'Kostroma')
INSERT Cities (Ciid, Ciname) VALUES (5,  'NNovgorod')
INSERT Cities (Ciid, Ciname) VALUES (6,  'VNovgorod')
INSERT Cities (Ciid, Ciname) VALUES (7,  'Ufa')
INSERT Cities (Ciid, Ciname) VALUES (8,  'Chita')
INSERT Cities (Ciid, Ciname) VALUES (9,  'Rostov')
INSERT Cities (Ciid, Ciname) VALUES (10, 'Kursk')
SET IDENTITY_INSERT Cities OFF

SET IDENTITY_INSERT Technicians ON
INSERT Technicians (Tpassport, Tname, Tcity, Taddress, Tphone) 
	VALUES (1000000001, 'Ivan Petrov', 1, 'Lenina 41', '9290000001')
INSERT Technicians (Tpassport, Tname, Tcity, Taddress, Tphone) 
	VALUES (1000000002, 'Ivan Perov',  2, 'Lenina 42', '9290000002')
INSERT Technicians (Tpassport, Tname, Tcity, Taddress, Tphone) 
	VALUES (1000000003, 'Ivan Petov',  3, 'Lenina 41', '9290000003')
INSERT Technicians (Tpassport, Tname, Tcity, Taddress, Tphone) 
	VALUES (1000000004, 'Jan Petrov',  4, 'Lenina 42', '9290000004')
INSERT Technicians (Tpassport, Tname, Tcity, Taddress, Tphone) 
	VALUES (1000000005, 'Alex Petrov', 5, 'Lenina 41', '9290000005')
INSERT Technicians (Tpassport, Tname, Tcity, Taddress, Tphone) 
	VALUES (1000000006, 'Alex Petov',  6, 'Lenina 42', '9290000006')
INSERT Technicians (Tpassport, Tname, Tcity, Taddress, Tphone) 
	VALUES (1000000007, 'Alex Perov',  7, 'Lenina 41', '9290000007')
INSERT Technicians (Tpassport, Tname, Tcity, Taddress, Tphone) 
	VALUES (1000000008, 'Alex Metrov', 8, 'Lenina 42', '9290000008')
INSERT Technicians (Tpassport, Tname, Tcity, Taddress, Tphone) 
	VALUES (1000000009, 'Ivan Petrov', 9, 'Lenina 41', '9290000009')
INSERT Technicians (Tpassport, Tname, Taddress, Tphone) 
	VALUES (1000000010, 'Dan Petrov', 'Lenina 42', '9290000010')
INSERT Technicians (Tpassport, Tname, Taddress, Tphone) 
	VALUES (1, 'Dan Petrov', 'Lenina 42', '9290000010')
INSERT Technicians (Tpassport, Tname, Taddress, Tphone) 
	VALUES (2, 'Dan Petrov', 'Lenina 42', '9290000010')
INSERT Technicians (Tpassport, Tname, Taddress, Tphone) 
	VALUES (3, 'Dan Petrov', 'Lenina 42', '9290000010')
INSERT Technicians (Tpassport, Tname, Taddress, Tphone) 
	VALUES (4, 'Dan Petrov', 'Lenina 42', '9290000010')
SET IDENTITY_INSERT Technicians OFF

SET IDENTITY_INSERT Stations ON
INSERT Stations ([Sid], Scity, Stechid) VALUES (1, 1, 1000000001)
INSERT Stations ([Sid], Scity, Stechid) VALUES (2, 2, 1000000002)
INSERT Stations ([Sid], Scity, Stechid) VALUES (3, 3, 1000000003)
INSERT Stations ([Sid], Scity, Stechid) VALUES (4, 4, 1000000004)
INSERT Stations ([Sid], Scity, Stechid) VALUES (5, 5, 1000000005)
INSERT Stations ([Sid], Scity, Stechid) VALUES (6, 6, 1000000006)
INSERT Stations ([Sid], Scity, Stechid) VALUES (7, 1, 1000000007)
INSERT Stations ([Sid], Scity, Stechid) VALUES (8, 8, 1000000008)
INSERT Stations ([Sid], Scity, Stechid) VALUES (9, 9, 1000000009)
INSERT Stations ([Sid], Scity, Stechid) VALUES (10, 10, 1000000001)
SET IDENTITY_INSERT Stations OFF

SET IDENTITY_INSERT Equipment ON
INSERT Equipment (Eid, Ename, Eproducer) VALUES (1, 'BPC-2000', 'FireExting')
INSERT Equipment (Eid, Ename, Eproducer) VALUES (2, 'UPS-1', 'APC')
INSERT Equipment (Eid, Ename, Eproducer) VALUES (3, 'Food', 'McDo')
INSERT Equipment (Eid, Ename, Eproducer) VALUES (4, 'Cisco 71', 'CISCO')
INSERT Equipment (Eid, Ename, Eproducer) VALUES (5, 'Netgear DG834G', 'Netgear')
INSERT Equipment (Eid, Ename, Eproducer) VALUES (6, 'Mac', 'OOO AAApple')
INSERT Equipment (Eid, Ename, Eproducer) VALUES (7, 'BestSofa', 'IKEA')
INSERT Equipment (Eid, Ename, Eproducer) VALUES (8, 'UDecider', 'IntSys')
INSERT Equipment (Eid, Ename, Eproducer) VALUES (9, 'ALittlePony', 'Hasbro')
INSERT Equipment (Eid, Ename, Eproducer) VALUES (10, 'Server', 'Ethernet')
INSERT Equipment (Eid, Ename, Eproducer) VALUES (11, 'Server', 'SuperEthernet')
SET IDENTITY_INSERT Equipment OFF

SET IDENTITY_INSERT Operators ON
INSERT Operators (Opid, Opname) VALUES (1, 'MTC')
INSERT Operators (Opid, Opname) VALUES (2, 'Beeline')
INSERT Operators (Opid, Opname) VALUES (3, 'Megafon')
INSERT Operators (Opid, Opname) VALUES (4, 'Tele2')
INSERT Operators (Opid, Opname) VALUES (5, 'Vodafone')
INSERT Operators (Opid, Opname) VALUES (6, 'CTL')
INSERT Operators (Opid, Opname) VALUES (7, 'Telecom')
INSERT Operators (Opid, Opname) VALUES (8, 'AQTel')
INSERT Operators (Opid, Opname) VALUES (9, 'MPO')
INSERT Operators (Opid, Opname) VALUES (10, 'CTM')
SET IDENTITY_INSERT Operators OFF

INSERT OpStations ([Sid], Opid) VALUES (1, 1)
INSERT OpStations ([Sid], Opid) VALUES (1, 2)
INSERT OpStations ([Sid], Opid) VALUES (1, 3)
INSERT OpStations ([Sid], Opid) VALUES (1, 6)
INSERT OpStations ([Sid], Opid) VALUES (1, 8)
INSERT OpStations ([Sid], Opid) VALUES (4, 6)
INSERT OpStations ([Sid], Opid) VALUES (7, 8)
INSERT OpStations ([Sid], Opid) VALUES (8, 5)
INSERT OpStations ([Sid], Opid) VALUES (9, 9)
INSERT OpStations ([Sid], Opid) VALUES (2, 10)
INSERT OpStations ([Sid], Opid) VALUES (3, 7)
INSERT OpStations ([Sid], Opid) VALUES (10, 8)
INSERT OpStations ([Sid], Opid) VALUES (5, 2)
INSERT OpStations ([Sid], Opid) VALUES (4, 3)
INSERT OpStations ([Sid], Opid) VALUES (3, 3)
INSERT OpStations ([Sid], Opid) VALUES (7, 2)
INSERT OpStations ([Sid], Opid) VALUES (8, 2)
INSERT OpStations ([Sid], Opid) VALUES (4, 2)
INSERT OpStations ([Sid], Opid) VALUES (2, 4)
INSERT OpStations ([Sid], Opid) VALUES (7, 6)

INSERT StEquip ([Sid], Eid, SEdate, SEquant) VALUES (1, 1, '2000-1-2', 1005)
INSERT StEquip ([Sid], Eid, SEdate, SEquant) VALUES (1, 2, '2000-7-2', 1005)
INSERT StEquip ([Sid], Eid, SEdate, SEquant) VALUES (1, 3, '2003-1-2', 1005)
INSERT StEquip ([Sid], Eid, SEdate, SEquant) VALUES (1, 4, '2005-9-2', 1005)
INSERT StEquip ([Sid], Eid, SEdate, SEquant) VALUES (1, 5, '2009-1-2', 5)
INSERT StEquip ([Sid], Eid, SEdate, SEquant) VALUES (1, 6, '2000-1-2', 1005)
INSERT StEquip ([Sid], Eid, SEdate, SEquant) VALUES (1, 7, '2010-1-2', 1)
INSERT StEquip ([Sid], Eid, SEdate, SEquant) VALUES (1, 8, '2011-1-27', 1005)
INSERT StEquip ([Sid], Eid, SEdate, SEquant) VALUES (1, 9, '2010-1-2', 1005)
INSERT StEquip ([Sid], Eid, SEdate, SEquant) VALUES (4, 1, '2010-1-24', 1000)
INSERT StEquip ([Sid], Eid, SEdate, SEquant) VALUES (7, 1, '2013-1-22', 100)
INSERT StEquip ([Sid], Eid, SEdate, SEquant) VALUES (2, 1, '2000-1-5', 10050)
INSERT StEquip ([Sid], Eid, SEdate, SEquant) VALUES (1, 10, '2000-1-2', 1000)
INSERT StEquip ([Sid], Eid, SEdate, SEquant) VALUES (8, 1, '2000-1-22', 100500)
INSERT StEquip ([Sid], Eid, SEdate, SEquant) VALUES (2, 2, '2015-1-2', 10050)
INSERT StEquip ([Sid], Eid, SEdate, SEquant) VALUES (8, 7, '2005-10-2', 100500)
INSERT StEquip ([Sid], Eid, SEdate, SEquant) VALUES (5, 4, '2000-1-2', 100)
INSERT StEquip ([Sid], Eid, SEdate, SEquant) VALUES (2, 7, '2000-1-2', 100500)
INSERT StEquip ([Sid], Eid, SEdate, SEquant) VALUES (5, 6, '2000-1-2', 17)
INSERT StEquip ([Sid], Eid, SEdate, SEquant) VALUES (7, 7, '2000-1-2', 1300)