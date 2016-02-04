--/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
-- Script title:	Telephonization_Opimize
-- Script type:		optimize
-- Author:			Alexey Kulikov <alexeyqu@gmail.com>, gr. 376, DCAM MIPT
--/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
	
SELECT * INTO Customers FROM Northwind..Customers
DECLARE @count INT
SELECT @count = 10
WHILE @count >= 0
  BEGIN
    INSERT INTO Customers SELECT * FROM Northwind..Customers
    SELECT @count = @count - 1
  END

SELECT * INTO Orders FROM Northwind..Orders
SELECT * INTO OrderDetails FROM Northwind.."Order Details"

SELECT * INTO Products FROM Northwind..Products
SELECT @count = 10
WHILE @count >= 0
  BEGIN
    INSERT INTO Products (ProductName,SupplierID,CategoryID,QuantityPerUnit,
                                          UnitPrice,UnitsInStock,UnitsOnOrder,
                                                       ReorderLevel,Discontinued)
    SELECT ProductName,SupplierID,CategoryID,
             QuantityPerUnit,UnitPrice,UnitsInStock,
                UnitsOnOrder,ReorderLevel,Discontinued 
    FROM Northwind..Products
    SELECT @count = @count - 1
  END

SELECT * FROM Products;
SELECT * FROM OrderDetails;
SELECT * FROM Orders;
SELECT * FROM Customers;

SET SHOWPLAN_TEXT OFF
go
SELECT ShipName 
FROM orders 
WHERE ShippedDate > '01/01/1990' AND ShippedDate < '01/01/1999'
go

--Query to CUSTOMERS

SET STATISTICS TIME ON
SET STATISTICS IO ON

SET SHOWPLAN_TEXT ON
GO
SET SHOWPLAN_TEXT OFF
GO
SELECT CompanyName
FROM Customers
WITH (INDEX (CustomersIndex))
WHERE Country = 'USA'
	AND City != 'Portland'
ORDER BY CompanyName DESC

/* Plan without index:
  |--Sort(DISTINCT ORDER BY:([master].[dbo].[Customers].[CompanyName] DESC))
       |--Table Scan(OBJECT:([master].[dbo].[Customers]), WHERE:([master].[dbo].[Customers].[Country]=N'USA' AND [master].[dbo].[Customers].[City]<>N'Portland'))

Time elapsed without index:
SQL Server parse and compile time: 
   CPU time = 0 ms, elapsed time = 4 ms.
SQL Server parse and compile time: 
   CPU time = 0 ms, elapsed time = 0 ms.

(132 row(s) affected)
Table 'Customers'. Scan count 1, logical reads 38, physical reads 0, read-ahead reads 0, lob logical reads 0, lob physical reads 0, lob read-ahead reads 0.

 SQL Server Execution Times:
   CPU time = 16 ms,  elapsed time = 4 ms.
SQL Server parse and compile time: 
   CPU time = 0 ms, elapsed time = 0 ms.

 SQL Server Execution Times:
   CPU time = 0 ms,  elapsed time = 0 ms.

--Let's add indexes

Plan with index:
  |--Sort(DISTINCT ORDER BY:([master].[dbo].[Customers].[CompanyName] DESC))
       |--Table Scan(OBJECT:([master].[dbo].[Customers]), WHERE:([master].[dbo].[Customers].[Country]=N'USA' AND [master].[dbo].[Customers].[City]<>N'Portland'))

Time elapsed with index:SQL Server parse and compile time: 
   CPU time = 0 ms, elapsed time = 0 ms.
SQL Server parse and compile time: 
   CPU time = 0 ms, elapsed time = 0 ms.
SQL Server parse and compile time: 
   CPU time = 0 ms, elapsed time = 8 ms.
SQL Server parse and compile time: 
   CPU time = 0 ms, elapsed time = 0 ms.

(132 row(s) affected)
Table 'Customers'. Scan count 1, logical reads 38, physical reads 0, read-ahead reads 0, lob logical reads 0, lob physical reads 0, lob read-ahead reads 0.

 SQL Server Execution Times:
   CPU time = 0 ms,  elapsed time = 2 ms.
SQL Server parse and compile time: 
   CPU time = 0 ms, elapsed time = 0 ms.

 SQL Server Execution Times:
   CPU time = 0 ms,  elapsed time = 0 ms.

--NOW FORCED INDEXES

  |--Sort(ORDER BY:([master].[dbo].[Customers].[CompanyName] DESC))
       |--Nested Loops(Inner Join, OUTER REFERENCES:([Bmk1000], [Expr1004]) WITH UNORDERED PREFETCH)
            |--Index Seek(OBJECT:([master].[dbo].[Customers].[CustomersIndex]), SEEK:([master].[dbo].[Customers].[Country]=N'USA') ORDERED FORWARD)
            |--RID Lookup(OBJECT:([master].[dbo].[Customers]), SEEK:([Bmk1000]=[Bmk1000]),  WHERE:([master].[dbo].[Customers].[City]<N'Portland' OR [master].[dbo].[Customers].[City]>N'Portland') LOOKUP ORDERED FORWARD)

SQL Server parse and compile time: 
   CPU time = 0 ms, elapsed time = 0 ms.

 SQL Server Execution Times:
   CPU time = 0 ms,  elapsed time = 0 ms.
SQL Server parse and compile time: 
   CPU time = 0 ms, elapsed time = 0 ms.

(132 row(s) affected)
Table 'Customers'. Scan count 1, logical reads 158, physical reads 0, read-ahead reads 0, lob logical reads 0, lob physical reads 0, lob read-ahead reads 0.

 SQL Server Execution Times:
   CPU time = 0 ms,  elapsed time = 24 ms.

=-=-=-=-=-=-=-=-=
 SQL Server Execution Times:
   CPU time = 0 ms,  elapsed time = 2 ms.
=-=-=-=-=-=-=-=-=
*/
CREATE NONCLUSTERED INDEX CustomersIndex ON Customers
(
	Country
)

DROP INDEX CustomersIndex ON Customers

--OK, now 2 tables

SET SHOWPLAN_TEXT ON
GO
SET SHOWPLAN_TEXT OFF
GO
SELECT OrderDate, CompanyName 
FROM Customers, Orders
WHERE Orders.CustomerId = Customers.CustomerId 
	AND Country = 'USA'
	AND City != 'Portland'
	AND ShipCountry = 'USA'
ORDER BY CompanyName, OrderDate DESC

/*
Plan without indexes:
  |--Sort(ORDER BY:([master].[dbo].[Customers].[CompanyName] ASC, [master].[dbo].[Orders].[OrderDate] DESC))
       |--Hash Match(Inner Join, HASH:([master].[dbo].[Orders].[CustomerID])=([master].[dbo].[Customers].[CustomerID]), RESIDUAL:([master].[dbo].[Orders].[CustomerID]=[master].[dbo].[Customers].[CustomerID]))
            |--Table Scan(OBJECT:([master].[dbo].[Orders]), WHERE:([master].[dbo].[Orders].[ShipCountry]=N'USA'))
            |--Table Scan(OBJECT:([master].[dbo].[Customers]), WHERE:([master].[dbo].[Customers].[Country]=N'USA' AND [master].[dbo].[Customers].[City]<>N'Portland'))

SQL Server parse and compile time: 
   CPU time = 0 ms, elapsed time = 0 ms.
SQL Server parse and compile time: 
   CPU time = 14 ms, elapsed time = 14 ms.

(1320 row(s) affected)
Table 'Workfile'. Scan count 0, logical reads 0, physical reads 0, read-ahead reads 0, lob logical reads 0, lob physical reads 0, lob read-ahead reads 0.
Table 'Worktable'. Scan count 0, logical reads 0, physical reads 0, read-ahead reads 0, lob logical reads 0, lob physical reads 0, lob read-ahead reads 0.
Table 'Customers'. Scan count 1, logical reads 38, physical reads 0, read-ahead reads 0, lob logical reads 0, lob physical reads 0, lob read-ahead reads 0.
Table 'Orders'. Scan count 1, logical reads 22, physical reads 0, read-ahead reads 0, lob logical reads 0, lob physical reads 0, lob read-ahead reads 0.

 SQL Server Execution Times:
   CPU time = 31 ms,  elapsed time = 152 ms.
SQL Server parse and compile time: 
   CPU time = 0 ms, elapsed time = 0 ms.

 SQL Server Execution Times:
   CPU time = 0 ms,  elapsed time = 0 ms.

--INDEX

Plan: 
  |--Sort(ORDER BY:([master].[dbo].[Customers].[CompanyName] ASC, [master].[dbo].[Orders].[OrderDate] DESC))
       |--Hash Match(Inner Join, HASH:([master].[dbo].[Orders].[CustomerID])=([master].[dbo].[Customers].[CustomerID]), RESIDUAL:([master].[dbo].[Orders].[CustomerID]=[master].[dbo].[Customers].[CustomerID]))
            |--Clustered Index Scan(OBJECT:([master].[dbo].[Orders].[OrdersIndex]), WHERE:([master].[dbo].[Orders].[ShipCountry]=N'USA'))
            |--Table Scan(OBJECT:([master].[dbo].[Customers]), WHERE:([master].[dbo].[Customers].[Country]=N'USA' AND [master].[dbo].[Customers].[City]<>N'Portland'))

Time:

SQL Server parse and compile time: 
   CPU time = 0 ms, elapsed time = 0 ms.
SQL Server parse and compile time: 
   CPU time = 31 ms, elapsed time = 38 ms.

(1320 row(s) affected)
Table 'Workfile'. Scan count 0, logical reads 0, physical reads 0, read-ahead reads 0, lob logical reads 0, lob physical reads 0, lob read-ahead reads 0.
Table 'Worktable'. Scan count 0, logical reads 0, physical reads 0, read-ahead reads 0, lob logical reads 0, lob physical reads 0, lob read-ahead reads 0.
Table 'Customers'. Scan count 1, logical reads 38, physical reads 0, read-ahead reads 0, lob logical reads 0, lob physical reads 0, lob read-ahead reads 0.
Table 'Orders'. Scan count 1, logical reads 23, physical reads 0, read-ahead reads 0, lob logical reads 0, lob physical reads 0, lob read-ahead reads 0.

 SQL Server Execution Times:
   CPU time = 16 ms,  elapsed time = 138 ms.
SQL Server parse and compile time: 
   CPU time = 0 ms, elapsed time = 0 ms.

 SQL Server Execution Times:
   CPU time = 0 ms,  elapsed time = 0 ms.

*/

CREATE CLUSTERED INDEX OrdersIndex ON Orders
(
OrderId
)

GO 

CREATE NONCLUSTERED INDEX OrdersIndex2 ON Orders
(
CustomerId
)

GO

CREATE CLUSTERED INDEX ProductsIndex ON Products
(
ProductId
)

DROP INDEX OrdersIndex1 ON Orders
DROP INDEX OrdersIndex2 ON Orders
DROP INDEX ProductsIndex ON Products

--Move to all 4 tables!

SET SHOWPLAN_TEXT ON
GO
SET SHOWPLAN_TEXT OFF
GO
SELECT OrderDate, OrderDetails.UnitPrice, Quantity, Discount, ProductName, CompanyName
FROM Orders, OrderDetails, Customers, Products
WHERE Orders.customerId = Customers.customerId
	AND Orders.orderId = OrderDetails.orderId
	AND OrderDetails.productId = Products.productId
	AND Customers.Country = 'USA'
	AND Customers.City != 'Portland'
	AND Orders.ShipCountry = 'USA'
	AND OrderDetails.UnitPrice > 100
	AND Products.ProductId > 2
ORDER BY CompanyName, Orders.orderId DESC

/*
NO INDEX

Plan: 
  |--Nested Loops(Inner Join, OUTER REFERENCES:([master].[dbo].[OrderDetails].[ProductID]))
       |--Sort(ORDER BY:([master].[dbo].[Customers].[CompanyName] ASC, [master].[dbo].[Orders].[OrderID] DESC))
       |    |--Hash Match(Inner Join, HASH:([master].[dbo].[Orders].[CustomerID])=([master].[dbo].[Customers].[CustomerID]), RESIDUAL:([master].[dbo].[Orders].[CustomerID]=[master].[dbo].[Customers].[CustomerID]))
       |         |--Nested Loops(Inner Join, OUTER REFERENCES:([master].[dbo].[OrderDetails].[OrderID], [Expr1012]) WITH UNORDERED PREFETCH)
       |         |    |--Table Scan(OBJECT:([master].[dbo].[OrderDetails]), WHERE:([master].[dbo].[OrderDetails].[ProductID]>(2) AND [master].[dbo].[OrderDetails].[UnitPrice]>($100.0000)))
       |         |    |--Clustered Index Seek(OBJECT:([master].[dbo].[Orders].[OrdersIndex]), SEEK:([master].[dbo].[Orders].[OrderID]=[master].[dbo].[OrderDetails].[OrderID]),  WHERE:([master].[dbo].[Orders].[ShipCountry]=N'USA') ORDERED FORWARD)
       |         |--Table Scan(OBJECT:([master].[dbo].[Customers]), WHERE:([master].[dbo].[Customers].[Country]=N'USA' AND [master].[dbo].[Customers].[City]<>N'Portland'))
       |--Clustered Index Seek(OBJECT:([master].[dbo].[Products].[ProductsIndex]), SEEK:([master].[dbo].[Products].[ProductID]=[master].[dbo].[OrderDetails].[ProductID]),  WHERE:([master].[dbo].[Products].[ProductID]>(2)) ORDERED FORWARD)

SQL Server parse and compile time: 
   CPU time = 0 ms, elapsed time = 0 ms.

(2 row(s) affected)
SQL Server parse and compile time: 
   CPU time = 0 ms, elapsed time = 0 ms.
SQL Server parse and compile time: 
   CPU time = 0 ms, elapsed time = 0 ms.
SQL Server parse and compile time: 
   CPU time = 37 ms, elapsed time = 37 ms.

(120 row(s) affected)
Table 'Products'. Scan count 120, logical reads 240, physical reads 0, read-ahead reads 0, lob logical reads 0, lob physical reads 0, lob read-ahead reads 0.
Table 'Workfile'. Scan count 0, logical reads 0, physical reads 0, read-ahead reads 0, lob logical reads 0, lob physical reads 0, lob read-ahead reads 0.
Table 'Worktable'. Scan count 0, logical reads 0, physical reads 0, read-ahead reads 0, lob logical reads 0, lob physical reads 0, lob read-ahead reads 0.
Table 'Customers'. Scan count 1, logical reads 38, physical reads 0, read-ahead reads 0, lob logical reads 0, lob physical reads 0, lob read-ahead reads 0.
Table 'Orders'. Scan count 46, logical reads 117, physical reads 0, read-ahead reads 0, lob logical reads 0, lob physical reads 0, lob read-ahead reads 0.
Table 'OrderDetails'. Scan count 1, logical reads 13, physical reads 0, read-ahead reads 0, lob logical reads 0, lob physical reads 0, lob read-ahead reads 0.

 SQL Server Execution Times:
   CPU time = 16 ms,  elapsed time = 72 ms.
SQL Server parse and compile time: 
   CPU time = 0 ms, elapsed time = 0 ms.

 SQL Server Execution Times:
   CPU time = 0 ms,  elapsed time = 0 ms.

INDEX

Plan:
  |--Nested Loops(Inner Join, OUTER REFERENCES:([master].[dbo].[OrderDetails].[ProductID]))
       |--Sort(ORDER BY:([master].[dbo].[Customers].[CompanyName] ASC, [master].[dbo].[Orders].[OrderID] DESC))
       |    |--Hash Match(Inner Join, HASH:([master].[dbo].[Orders].[CustomerID])=([master].[dbo].[Customers].[CustomerID]), RESIDUAL:([master].[dbo].[Orders].[CustomerID]=[master].[dbo].[Customers].[CustomerID]))
       |         |--Nested Loops(Inner Join, OUTER REFERENCES:([master].[dbo].[Orders].[OrderID]))
       |         |    |--Clustered Index Scan(OBJECT:([master].[dbo].[Orders].[OrdersIndex]), WHERE:([master].[dbo].[Orders].[ShipCountry]=N'USA'))
       |         |    |--Clustered Index Seek(OBJECT:([master].[dbo].[OrderDetails].[OrderDetailsIndex]), SEEK:([master].[dbo].[OrderDetails].[OrderID]=[master].[dbo].[Orders].[OrderID]),  WHERE:([master].[dbo].[OrderDetails].[ProductID]>(2) AND [master].[dbo].[OrderDetails].[UnitPrice]>($100.0000)) ORDERED FORWARD)
       |         |--Table Scan(OBJECT:([master].[dbo].[Customers]), WHERE:([master].[dbo].[Customers].[Country]=N'USA' AND [master].[dbo].[Customers].[City]<>N'Portland'))
       |--Clustered Index Seek(OBJECT:([master].[dbo].[Products].[ProductsIndex]), SEEK:([master].[dbo].[Products].[ProductID]=[master].[dbo].[OrderDetails].[ProductID]),  WHERE:([master].[dbo].[Products].[ProductID]>(2)) ORDERED FORWARD)

SQL Server parse and compile time: 
   CPU time = 0 ms, elapsed time = 0 ms.

(2 row(s) affected)
SQL Server parse and compile time: 
   CPU time = 0 ms, elapsed time = 0 ms.
SQL Server parse and compile time: 
   CPU time = 0 ms, elapsed time = 0 ms.
SQL Server parse and compile time: 
   CPU time = 32 ms, elapsed time = 39 ms.

(120 row(s) affected)
Table 'Products'. Scan count 120, logical reads 240, physical reads 0, read-ahead reads 0, lob logical reads 0, lob physical reads 0, lob read-ahead reads 0.
Table 'Workfile'. Scan count 0, logical reads 0, physical reads 0, read-ahead reads 0, lob logical reads 0, lob physical reads 0, lob read-ahead reads 0.
Table 'Worktable'. Scan count 0, logical reads 0, physical reads 0, read-ahead reads 0, lob logical reads 0, lob physical reads 0, lob read-ahead reads 0.
Table 'Customers'. Scan count 1, logical reads 38, physical reads 0, read-ahead reads 0, lob logical reads 0, lob physical reads 0, lob read-ahead reads 0.
Table 'OrderDetails'. Scan count 122, logical reads 249, physical reads 0, read-ahead reads 0, lob logical reads 0, lob physical reads 0, lob read-ahead reads 0.
Table 'Orders'. Scan count 1, logical reads 23, physical reads 0, read-ahead reads 0, lob logical reads 0, lob physical reads 0, lob read-ahead reads 0.

 SQL Server Execution Times:
   CPU time = 15 ms,  elapsed time = 131 ms.
SQL Server parse and compile time: 
   CPU time = 0 ms, elapsed time = 0 ms.

 SQL Server Execution Times:
   CPU time = 0 ms,  elapsed time = 0 ms.
*/

	
CREATE CLUSTERED INDEX OrderDetailsIndex ON OrderDetails
(
OrderId
)

GO

CREATE NONCLUSTERED INDEX OrderDetailsIndex2 ON OrderDetails
(
UnitPrice
)

DROP INDEX OrderDetailsIndex ON OrderDetails
DROP INDEX OrderDetailsIndex2 ON OrderDetails
