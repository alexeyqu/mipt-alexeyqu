--/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
-- Script title:	Telephonization_Meta
-- Script type:		meta
-- Author:			Alexey Kulikov <alexeyqu@gmail.com>, gr. 376, DCAM MIPT
--/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
	
--TABLES OWNED BY ME
SELECT sysobjects.name AS [Table_name],
	sysobjects.uid,
	sys.schemas.schema_id
FROM sysobjects 
	JOIN sys.database_principals
		ON sysobjects.uid = sys.database_principals.principal_id
	JOIN sys.schemas 
		ON sys.schemas.schema_id = sysobjects.uid
WHERE sysobjects.xtype = 'U' 
	AND sys.database_principals.name = 'TestU2';

SELECT sys.schemas.name, schema_id, principal_id, sysobjects.name
FROM sys.schemas
	JOIN sysobjects
		ON sys.schemas.schema_id = sysobjects.uid

SELECT name, uid FROM sys.sysusers

SELECT name, principal_id, owning_principal_id FROM sys.database_principals

--COLUMNS OWNED BY ME
SELECT sysobjects.name AS Table_name, 
	syscolumns.name AS Column_name,
	syscolumns.xtype AS [Type_id], 
	systypes.name AS [Type_name], 
	systypes.length AS [Type_length], 
	syscolumns.colid AS [Column_ID], 
	syscolumns.isnullable AS [nullable]
FROM syscolumns 
	JOIN sysobjects 
		ON sysobjects.id = syscolumns.id
	JOIN sys.database_principals
		ON sysobjects.uid = sys.database_principals.principal_id
	JOIN systypes
		ON syscolumns.xtype = systypes.xtype
WHERE sysobjects.xtype = 'U' 
	AND sys.database_principals.name = CURRENT_USER;

--INTEGRITY CONSTRAINTS
SELECT SO.name AS [Name],
	SO.xtype AS [Type],
	SO2.name AS [Table_name]
FROM sys.sysobjects AS SO
	JOIN sys.sysobjects AS SO2
		ON SO.parent_obj = SO2.id
	JOIN sys.database_principals
		ON SO.uid = sys.database_principals.principal_id
WHERE sys.database_principals.name = CURRENT_USER
	AND (SO.xtype = 'F' OR
	SO.xtype = 'PK')

--FOREIGN KEYS
SELECT SOK.name AS [Key_name],
	SOF.name AS [Foreign_name],
	SOP.name AS [Parent_name]
FROM sys.sysreferences AS SR 
	JOIN sys.sysobjects AS SOK
		ON SR.constid = SOK.id
	JOIN sys.sysobjects AS SOF
		ON SR.fkeyid = SOF.id
	JOIN sys.sysobjects AS SOP
		ON SR.rkeyid = SOP.id
	JOIN sys.database_principals
		ON SOK.uid = sys.database_principals.principal_id
WHERE sys.database_principals.name = CURRENT_USER

--VIEWS
SELECT SO.name AS [View_name],
	SC.text AS [View_text]
FROM sys.sysobjects AS SO
	JOIN sys.syscomments AS SC
		ON SO.id = SC.id
	JOIN sys.database_principals
		ON SO.uid = sys.database_principals.principal_id
WHERE sys.database_principals.name = CURRENT_USER
	AND SO.xtype = 'V'
	
--TRIGGERS
SELECT SO.name AS [View_name],
	SC.text AS [View_text]
FROM sys.sysobjects AS SO
	JOIN sys.syscomments AS SC
		ON SO.id = SC.id
	JOIN sys.database_principals
		ON SO.uid = sys.database_principals.principal_id
WHERE sys.database_principals.name = CURRENT_USER
	AND SO.xtype = 'TR'

SELECT *
FROM INFORMATION_SCHEMA.TABLES;

SELECT * 
FROM sys.all_objects;

SELECT TOP 1 WITH TIES 
	SO.name AS [Name],
	SO.xtype AS [Type],
	SO2.name AS [Table_name],
	SO.crdate AS [Date]
FROM sys.sysobjects AS SO
	JOIN sys.sysobjects AS SO2
		ON SO.parent_obj = SO2.id
	JOIN sys.database_principals
		ON SO.uid = sys.database_principals.principal_id
WHERE sys.database_principals.name = CURRENT_USER
	AND SO.xtype = 'PK'
ORDER BY SO.crdate DESC

DROP TABLE HisTable1