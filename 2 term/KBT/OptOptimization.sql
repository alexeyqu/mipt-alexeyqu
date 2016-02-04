--/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
-- Script title:	Telephonization_Optimize2
-- Script type:		optimize
-- Author:			Alexey Kulikov <alexeyqu@gmail.com>, gr. 376, DCAM MIPT
--/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
	
DELETE FROM Products;
DELETE FROM OrderDetails;
DELETE FROM Orders;
DELETE FROM Customers;
DELETE FROM OptProducts;
DELETE FROM OptOrderDetails;
DELETE FROM OptOrders;
DELETE FROM OptCustomers;

DROP TABLE Products;
DROP TABLE OrderDetails;
DROP TABLE Orders;
DROP TABLE Customers;
DROP TABLE OptProducts;
DROP TABLE OptOrderDetails;
DROP TABLE OptOrders;
DROP TABLE OptCustomers;

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

SELECT COUNT(*) FROM Products;
SELECT COUNT(*) FROM OrderDetails;
SELECT COUNT(*) FROM Orders;
SELECT COUNT(*) FROM Customers;

SELECT * INTO OptCustomers FROM Northwind..Customers
DECLARE @count INT
SELECT @count = 10
WHILE @count >= 0
  BEGIN
    INSERT INTO OptCustomers SELECT * FROM Northwind..Customers
    SELECT @count = @count - 1
  END

SELECT * INTO OptOrders FROM Northwind..Orders
SELECT * INTO OptOrderDetails FROM Northwind.."Order Details"

SELECT * INTO OptProducts FROM Northwind..Products
SELECT @count = 10
WHILE @count >= 0
  BEGIN
    INSERT INTO OptProducts (ProductName,SupplierID,CategoryID,QuantityPerUnit,
                                          UnitPrice,UnitsInStock,UnitsOnOrder,
                                                       ReorderLevel,Discontinued)
    SELECT ProductName,SupplierID,CategoryID,
             QuantityPerUnit,UnitPrice,UnitsInStock,
                UnitsOnOrder,ReorderLevel,Discontinued 
    FROM Northwind..Products
    SELECT @count = @count - 1
  END

SELECT COUNT(*) FROM OptProducts;
SELECT COUNT(*) FROM OptOrderDetails;
SELECT COUNT(*) FROM OptOrders;
SELECT COUNT(*) FROM OptCustomers;

--Query to CUSTOMERS

SET STATISTICS TIME ON
SET STATISTICS IO ON

CHECKPOINT
GO
DBCC DROPCLEANBUFFERS
GO

SET SHOWPLAN_TEXT ON
GO
SET SHOWPLAN_TEXT OFF
GO
CREATE NONCLUSTERED INDEX CustomersIndex ON OptCustomers
(
	Country
);
GO
DROP INDEX CustomersIndex ON OptCustomers;

SELECT CompanyName
FROM Customers
WHERE Country = 'USA'
	AND City != 'Portland'
ORDER BY CompanyName DESC
GO
SELECT CompanyName
FROM OptCustomers
--WITH (INDEX (CustomersIndex))
WITH (INDEX (CustomersIndex2))
WHERE Country = 'USA'
	AND City != 'Portland'
GO
SELECT CompanyName
FROM OptCustomers
--WITH (INDEX (CustomersIndex))
WITH (INDEX (CustomersIndex2))
WHERE Country = 'USA'
	AND City != 'Portland'
ORDER BY CompanyName DESC

DROP INDEX CustomersIndex2 ON OptCustomers
GO
CREATE NONCLUSTERED INDEX CustomersIndex2 ON OptCustomers
(
Country,
City,
CompanyName
)
INCLUDE (CustomerID,
City)

/* Plan without index:  
   |--Sort(ORDER BY:([master].[dbo].[Customers].[CompanyName] DESC))
       |--Table Scan(OBJECT:([master].[dbo].[Customers]), WHERE:([master].[dbo].[Customers].[Country]=CONVERT_IMPLICIT(nvarchar(4000),[@1],0) AND [master].[dbo].[Customers].[City]<>CONVERT_IMPLICIT(nvarchar(4000),[@2],0)))
 
 Plan with index:
   |--Sort(ORDER BY:([master].[dbo].[OptCustomers].[CompanyName] DESC))
       |--Nested Loops(Inner Join, OUTER REFERENCES:([Bmk1000], [Expr1004]) WITH UNORDERED PREFETCH)
            |--Index Seek(OBJECT:([master].[dbo].[OptCustomers].[CustomersIndex]), SEEK:([master].[dbo].[OptCustomers].[Country]=N'USA') ORDERED FORWARD)
            |--RID Lookup(OBJECT:([master].[dbo].[OptCustomers]), SEEK:([Bmk1000]=[Bmk1000]),  WHERE:([master].[dbo].[OptCustomers].[City]<N'Portland' OR [master].[dbo].[OptCustomers].[City]>N'Portland') LOOKUP ORDERED FORWARD)

Stats:
SQL Server parse and compile time: 
   CPU time = 0 ms, elapsed time = 0 ms.
SQL Server parse and compile time: 
   CPU time = 0 ms, elapsed time = 0 ms.

(132 row(s) affected)
Table 'Customers'. Scan count 1, logical reads 38, physical reads 0, read-ahead reads 38, lob logical reads 0, lob physical reads 0, lob read-ahead reads 0.

 SQL Server Execution Times:
   CPU time = 15 ms,  elapsed time = 141 ms.
SQL Server parse and compile time: 
   CPU time = 0 ms, elapsed time = 0 ms.

(132 row(s) affected)
Table 'OptCustomers'. Scan count 1, logical reads 158, physical reads 6, read-ahead reads 32, lob logical reads 0, lob physical reads 0, lob read-ahead reads 0.

 SQL Server Execution Times:
   CPU time = 15 ms,  elapsed time = 69 ms.
*/

--OK, now 2 tables

SET SHOWPLAN_TEXT ON
GO
SET SHOWPLAN_TEXT OFF

CHECKPOINT
GO
DBCC DROPCLEANBUFFERS
GO

CREATE CLUSTERED INDEX OrdersIndex ON OptOrders
(
OrderId
)
GO 
CREATE NONCLUSTERED INDEX OrdersIndex2 ON OptOrders
(
CustomerId
)
GO
CREATE NONCLUSTERED INDEX CustomersIndex2 ON OptCustomers
(
Country,
City
)
INCLUDE (CustomerID, CompanyName)
GO
DROP INDEX OrdersIndex1 ON OptOrders
GO
DROP INDEX OrdersIndex2 ON OptOrders
GO
DROP INDEX CustomersIndex2 ON OptCustomers

BEGIN
SELECT OrderDate, CompanyName 
FROM Customers, Orders
WHERE Orders.CustomerId = Customers.CustomerId 
	AND Country = 'USA'
	AND City != 'Portland'
	AND ShipCountry = 'USA'
ORDER BY CompanyName, OrderDate DESC

SELECT OrderDate, CompanyName 
FROM OptCustomers
		WITH (INDEX (CustomersIndex2)), 
	OptOrders
--		WITH (INDEX (OrdersIndex, OrdersIndex2))
WHERE OptOrders.CustomerId = OptCustomers.CustomerId 
	AND Country = 'USA'
	AND City != 'Portland'
	AND ShipCountry = 'USA'
ORDER BY CompanyName, OrderDate DESC
END

/* Plan without index:
  |--Sort(ORDER BY:([master].[dbo].[Customers].[CompanyName] ASC, [master].[dbo].[Orders].[OrderDate] DESC))
       |--Hash Match(Inner Join, HASH:([master].[dbo].[Orders].[CustomerID])=([master].[dbo].[Customers].[CustomerID]), RESIDUAL:([master].[dbo].[Orders].[CustomerID]=[master].[dbo].[Customers].[CustomerID]))
            |--Table Scan(OBJECT:([master].[dbo].[Orders]), WHERE:([master].[dbo].[Orders].[ShipCountry]=N'USA'))
            |--Table Scan(OBJECT:([master].[dbo].[Customers]), WHERE:([master].[dbo].[Customers].[Country]=N'USA' AND [master].[dbo].[Customers].[City]<>N'Portland'))

Plan with index:
  |--Sort(ORDER BY:([master].[dbo].[OptCustomers].[CompanyName] ASC, [master].[dbo].[OptOrders].[OrderDate] DESC))
       |--Hash Match(Inner Join, HASH:([master].[dbo].[OptOrders].[CustomerID])=([master].[dbo].[OptCustomers].[CustomerID]), RESIDUAL:([master].[dbo].[OptOrders].[CustomerID]=[master].[dbo].[OptCustomers].[CustomerID]))
            |--Hash Match(Inner Join, HASH:([master].[dbo].[OptOrders].[OrderID], [Uniq1004])=([master].[dbo].[OptOrders].[OrderID], [Uniq1004]), RESIDUAL:([master].[dbo].[OptOrders].[OrderID] = [master].[dbo].[OptOrders].[OrderID] AND [Uniq1004] = [Uniq1004]))
            |    |--Clustered Index Scan(OBJECT:([master].[dbo].[OptOrders].[OrdersIndex]), WHERE:([master].[dbo].[OptOrders].[ShipCountry]=N'USA'))
            |    |--Index Scan(OBJECT:([master].[dbo].[OptOrders].[OrdersIndex2]))
            |--Nested Loops(Inner Join, OUTER REFERENCES:([Bmk1000], [Expr1007]) WITH UNORDERED PREFETCH)
                 |--Index Seek(OBJECT:([master].[dbo].[OptCustomers].[CustomersIndex]), SEEK:([master].[dbo].[OptCustomers].[Country]=N'USA') ORDERED FORWARD)
                 |--RID Lookup(OBJECT:([master].[dbo].[OptCustomers]), SEEK:([Bmk1000]=[Bmk1000]),  WHERE:([master].[dbo].[OptCustomers].[City]<N'Portland' OR [master].[dbo].[OptCustomers].[City]>N'Portland') LOOKUP ORDERED FORWARD)

Stats:
SQL Server parse and compile time: 
   CPU time = 31 ms, elapsed time = 80 ms.

(1320 row(s) affected)
Table 'Workfile'. Scan count 0, logical reads 0, physical reads 0, read-ahead reads 0, lob logical reads 0, lob physical reads 0, lob read-ahead reads 0.
Table 'Worktable'. Scan count 0, logical reads 0, physical reads 0, read-ahead reads 0, lob logical reads 0, lob physical reads 0, lob read-ahead reads 0.
Table 'Customers'. Scan count 1, logical reads 38, physical reads 0, read-ahead reads 38, lob logical reads 0, lob physical reads 0, lob read-ahead reads 0.
Table 'Orders'. Scan count 1, logical reads 22, physical reads 0, read-ahead reads 22, lob logical reads 0, lob physical reads 0, lob read-ahead reads 0.

 SQL Server Execution Times:
   CPU time = 47 ms,  elapsed time = 296 ms.

(1320 row(s) affected)
Table 'Workfile'. Scan count 0, logical reads 0, physical reads 0, read-ahead reads 0, lob logical reads 0, lob physical reads 0, lob read-ahead reads 0.
Table 'Worktable'. Scan count 0, logical reads 0, physical reads 0, read-ahead reads 0, lob logical reads 0, lob physical reads 0, lob read-ahead reads 0.
Table 'OptCustomers'. Scan count 1, logical reads 158, physical reads 2, read-ahead reads 36, lob logical reads 0, lob physical reads 0, lob read-ahead reads 0.
Table 'OptOrders'. Scan count 2, logical reads 28, physical reads 2, read-ahead reads 24, lob logical reads 0, lob physical reads 0, lob read-ahead reads 0.

 SQL Server Execution Times:
   CPU time = 31 ms,  elapsed time = 189 ms.
*/

--Move to all 4 tables!

SET SHOWPLAN_TEXT ON
GO
SET SHOWPLAN_TEXT OFF
GO

CHECKPOINT
GO
DBCC DROPCLEANBUFFERS
GO

CREATE CLUSTERED INDEX OrderDetailsIndex ON OptOrderDetails
(
OrderId
)

CREATE NONCLUSTERED INDEX OrderDetailsIndex2 ON OptOrderDetails
(
UnitPrice
)

CREATE CLUSTERED INDEX ProductsIndex ON OptProducts
(
ProductId
)

DROP INDEX ProductsIndex ON OptProducts
DROP INDEX OrderDetailsIndex ON OptOrderDetails
DROP INDEX OrderDetailsIndex2 ON OptOrderDetails

BEGIN
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

SELECT OrderDate, OptOrderDetails.UnitPrice, Quantity, Discount, ProductName, CompanyName
FROM OptOrders,
--		WITH (INDEX (OrdersIndex, OrdersIndex2)), 
	OptOrderDetails,
--		WITH (INDEX (OrderDetailsIndex, OrderDetailsIndex2)),
	OptCustomers
		WITH (INDEX (CustomersIndex2)), 
	OptProducts
--		WITH (INDEX (ProductsIndex))
WHERE OptOrders.customerId = OptCustomers.customerId
	AND OptOrders.orderId = OptOrderDetails.orderId
	AND OptOrderDetails.productId = OptProducts.productId
	AND OptCustomers.Country = 'USA'
	AND OptCustomers.City != 'Portland'
	AND OptOrders.ShipCountry = 'USA'
	AND OptOrderDetails.UnitPrice > 100
	AND OptProducts.ProductId > 2
ORDER BY CompanyName, OptOrders.orderId DESC
END

/* Plan withour index:
  |--Sort(ORDER BY:([master].[dbo].[Customers].[CompanyName] ASC, [master].[dbo].[Orders].[OrderID] DESC))
       |--Hash Match(Inner Join, HASH:([master].[dbo].[OrderDetails].[ProductID])=([master].[dbo].[Products].[ProductID]))
            |--Hash Match(Inner Join, HASH:([master].[dbo].[Orders].[CustomerID])=([master].[dbo].[Customers].[CustomerID]), RESIDUAL:([master].[dbo].[Orders].[CustomerID]=[master].[dbo].[Customers].[CustomerID]))
            |    |--Hash Match(Inner Join, HASH:([master].[dbo].[OrderDetails].[OrderID])=([master].[dbo].[Orders].[OrderID]))
            |    |    |--Table Scan(OBJECT:([master].[dbo].[OrderDetails]), WHERE:([master].[dbo].[OrderDetails].[ProductID]>(2) AND [master].[dbo].[OrderDetails].[UnitPrice]>($100.0000)))
            |    |    |--Table Scan(OBJECT:([master].[dbo].[Orders]), WHERE:([master].[dbo].[Orders].[ShipCountry]=N'USA'))
            |    |--Table Scan(OBJECT:([master].[dbo].[Customers]), WHERE:([master].[dbo].[Customers].[Country]=N'USA' AND [master].[dbo].[Customers].[City]<>N'Portland'))
            |--Table Scan(OBJECT:([master].[dbo].[Products]), WHERE:([master].[dbo].[Products].[ProductID]>(2)))

Plan with index:
  |--Nested Loops(Inner Join, OUTER REFERENCES:([master].[dbo].[OptOrderDetails].[ProductID]))
       |--Sort(ORDER BY:([master].[dbo].[OptCustomers].[CompanyName] ASC, [master].[dbo].[OptOrders].[OrderID] DESC))
       |    |--Hash Match(Inner Join, HASH:([master].[dbo].[OptOrders].[CustomerID])=([master].[dbo].[OptCustomers].[CustomerID]), RESIDUAL:([master].[dbo].[OptOrders].[CustomerID]=[master].[dbo].[OptCustomers].[CustomerID]))
       |         |--Hash Match(Inner Join, HASH:([master].[dbo].[OptOrderDetails].[OrderID])=([master].[dbo].[OptOrders].[OrderID]))
       |         |    |--Hash Match(Inner Join, HASH:([master].[dbo].[OptOrderDetails].[OrderID], [Uniq1004])=([master].[dbo].[OptOrderDetails].[OrderID], [Uniq1004]), RESIDUAL:([master].[dbo].[OptOrderDetails].[OrderID] = [master].[dbo].[OptOrderDetails].[OrderID] AND [Uniq1004] = [Uniq1004]))
       |         |    |    |--Clustered Index Scan(OBJECT:([master].[dbo].[OptOrderDetails].[OrderDetailsIndex]), WHERE:([master].[dbo].[OptOrderDetails].[ProductID]>(2) AND [master].[dbo].[OptOrderDetails].[UnitPrice]>($100.0000)))
       |         |    |    |--Index Seek(OBJECT:([master].[dbo].[OptOrderDetails].[OrderDetailsIndex2]), SEEK:([master].[dbo].[OptOrderDetails].[UnitPrice] > ($100.0000)) ORDERED FORWARD)
       |         |    |--Hash Match(Inner Join, HASH:([master].[dbo].[OptOrders].[OrderID], [Uniq1001])=([master].[dbo].[OptOrders].[OrderID], [Uniq1001]), RESIDUAL:([master].[dbo].[OptOrders].[OrderID] = [master].[dbo].[OptOrders].[OrderID] AND [Uniq1001] = [Uniq1001]))
       |         |         |--Clustered Index Scan(OBJECT:([master].[dbo].[OptOrders].[OrdersIndex]), WHERE:([master].[dbo].[OptOrders].[ShipCountry]=N'USA'))
       |         |         |--Index Scan(OBJECT:([master].[dbo].[OptOrders].[OrdersIndex2]))
       |         |--Index Seek(OBJECT:([master].[dbo].[OptCustomers].[CustomersIndex2]), SEEK:([master].[dbo].[OptCustomers].[Country]=N'USA' AND [master].[dbo].[OptCustomers].[City] < N'Portland' OR [master].[dbo].[OptCustomers].[Country]=N'USA' AND [master].[dbo].[OptCustomers].[City] > N'Portland') ORDERED FORWARD)
       |--Clustered Index Seek(OBJECT:([master].[dbo].[OptProducts].[ProductsIndex]), SEEK:([master].[dbo].[OptProducts].[ProductID]=[master].[dbo].[OptOrderDetails].[ProductID]),  WHERE:([master].[dbo].[OptProducts].[ProductID]>(2)) ORDERED FORWARD)

Stats:

SQL Server parse and compile time: 
   CPU time = 0 ms, elapsed time = 0 ms.

 SQL Server Execution Times:
   CPU time = 0 ms,  elapsed time = 0 ms.
SQL Server parse and compile time: 
   CPU time = 62 ms, elapsed time = 176 ms.

(120 row(s) affected)
Table 'Workfile'. Scan count 0, logical reads 0, physical reads 0, read-ahead reads 0, lob logical reads 0, lob physical reads 0, lob read-ahead reads 0.
Table 'Worktable'. Scan count 0, logical reads 0, physical reads 0, read-ahead reads 0, lob logical reads 0, lob physical reads 0, lob read-ahead reads 0.
Table 'Products'. Scan count 1, logical reads 14, physical reads 14, read-ahead reads 0, lob logical reads 0, lob physical reads 0, lob read-ahead reads 0.
Table 'Customers'. Scan count 1, logical reads 38, physical reads 0, read-ahead reads 38, lob logical reads 0, lob physical reads 0, lob read-ahead reads 0.
Table 'Orders'. Scan count 1, logical reads 22, physical reads 0, read-ahead reads 22, lob logical reads 0, lob physical reads 0, lob read-ahead reads 0.
Table 'OrderDetails'. Scan count 1, logical reads 13, physical reads 13, read-ahead reads 0, lob logical reads 0, lob physical reads 0, lob read-ahead reads 0.

(1 row(s) affected)

 SQL Server Execution Times:
   CPU time = 31 ms,  elapsed time = 509 ms.

(120 row(s) affected)
Table 'OptProducts'. Scan count 120, logical reads 240, physical reads 2, read-ahead reads 0, lob logical reads 0, lob physical reads 0, lob read-ahead reads 0.
Table 'Workfile'. Scan count 0, logical reads 0, physical reads 0, read-ahead reads 0, lob logical reads 0, lob physical reads 0, lob read-ahead reads 0.
Table 'Worktable'. Scan count 0, logical reads 0, physical reads 0, read-ahead reads 0, lob logical reads 0, lob physical reads 0, lob read-ahead reads 0.
Table 'OptCustomers'. Scan count 2, logical reads 7, physical reads 1, read-ahead reads 3, lob logical reads 0, lob physical reads 0, lob read-ahead reads 0.
Table 'OptOrders'. Scan count 2, logical reads 28, physical reads 2, read-ahead reads 24, lob logical reads 0, lob physical reads 0, lob read-ahead reads 0.
Table 'OptOrderDetails'. Scan count 2, logical reads 16, physical reads 4, read-ahead reads 10, lob logical reads 0, lob physical reads 0, lob read-ahead reads 0.

(1 row(s) affected)

 SQL Server Execution Times:
   CPU time = 31 ms,  elapsed time = 514 ms.
SQL Server parse and compile time: 
   CPU time = 0 ms, elapsed time = 0 ms.

 SQL Server Execution Times:
   CPU time = 0 ms,  elapsed time = 0 ms.
*/