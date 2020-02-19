#!/usr/bin/env python

"""sql.py: This library has an basic API for reading/writing & creating sqlite database 

__author__      = "Nitin Mohan
__copyright__   = "Copy Right 2018. NM Technologies
"""

#---------------------------------------------------#
#                   System Imports                  #
#---------------------------------------------------#
import sqlite3

#---------------------------------------------------#
#                   Start of Program                #
#---------------------------------------------------#
class SQL(object):
    def __init__(self, dbfile, tables, log):
        self.db_file = dbfile
        self.tables = tables
        self.log = log
        self._createDB()

    def _createDB(self):
        #\Input: N/A
        #\Function: Create a database and tables using the Table Config
        #\Output: N/A
        
        conn = sqlite3.connect(self.db_file)
        for table in self.tables:
            self.log.debug("Creating Table {}".format(table))
            conn.cursor().execute("CREATE TABLE IF NOT EXISTS %s (%s)"%(table["TableName"], table["TableCol"]))
            conn.commit()
            self.log.debug("Created Table: {}".format(table["TableCol"]))
        conn.close()
        self.log.debug("Database creation successful")
        
    def _insert_input_struct(self, sqlstruct, table, tb_columns):
        #\Input: input values, input columns, and table name
        #\Function: Construct input structure for inserting a line into the database
        #\Output: Input structure

        struct_0 = "INSERT INTO %s("%table
        struct_1 = ""
        struct_2 = ") VALUES("
        for column in tb_columns:
            struct_1 += "%s, "%column

        struct_1 = struct_1[:-2]
        struct = struct_0 + struct_1 + struct_2

        for item in sqlstruct:
            struct += "?, "

        final_struct = struct[:len(struct)-2] + ")"
        self.log.debug("Created input_struct {}".format(final_struct))
        return final_struct 
                 
    def _update_input_struct(self, columns, table, update_arg):
        #\Input: columns to be updated, table name and search criteria
        #\Function: Construct the sql argument for updating a table based on inputs
        #\Output: Skeleton structure and values

        struct_1 = "UPDATE %s set "%table
        struct_2 = " WHERE %s = ?"%(update_arg[0])
        update_struct = []

        for col in columns:
            struct_1 += "%s = ?, "%col[0]
            update_struct.append(col[1])

        struct_1 = struct_1[:-2]
        skel_struct = struct_1 + struct_2
        update_struct.append(update_arg[1])

        final_struct  = skel_struct, tuple(update_struct)
        self.log.debug("Update sturct created {}".format(final_struct))
        return final_struct
         
    def insertDB(self, sqlstruct, db_file, table, tb_columns):
        #\Input: Input Data, Table Name
        #\Function: Insert a line into the table given the input
        #\Output: N/A

        conn = sqlite3.connect(db_file)
        input_struct = self._insert_input_struct(sqlstruct, table, tb_columns)
        conn.cursor().execute(input_struct, sqlstruct)
        conn.commit()
        conn.close()
        self.log.debug("Data: {} Written to Table: {} into database"
                    .format(input_struct, table))

    def removeDB(self, column, arg, db_file, table):
        #\Input column name, value , table name
        #\Function: Remove line from database given input
        #\Output: N/A
        
        conn = sqlite3.connect(db_file)
        input_struct = "DELETE FROM %s WHERE %s = ?"%(table, column)
        self.log.debug("Executing deletion from database {} {}"
                    .format(input_struct, arg))
        conn.cursor().execute(input_struct, arg)
        conn.commit()
        conn.close()

    def updateDB(self, columns, update_arg, db_file, table):
        #\Input Columns to be updated, search criteria, table name
        #\Function: Update a line in a table given the input criteria
        #\Output: N/A

        conn = sqlite3.connect(db_file)
        skel_struct, update_struct = self._update_input_struct(columns, table, update_arg)
        conn.cursor().execute(skel_struct, update_struct)
        conn.commit()
        conn.close()
        self.log.debug("Updated >> Table: {},  Data: {}".format(table, update_struct))
