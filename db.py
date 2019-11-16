import psycopg2


class Interface:

    # Constructor
    def __init__(self, dbname, dbusername, dbpassword):
        self.dbName = dbname
        self.dbUser = dbusername
        self.dbPassword = dbpassword
        self.connection = None
        self.cursor = None

    def connect(self):
        self.connection = psycopg2.connect(
            "dbname={} user={} password={}".format(self.dbName, self.dbUser, self.dbPassword))
        self.cursor = self.connection.cursor()
        return

    def insertlocation(self, id, ip, name):
        query = "INSERT INTO location(\"id\", ip, name)" \
                " VALUES ('{}','{}','{}')".format(id, ip, name)

        self.cursor.execute(query)  # Executes the
        self.connection.commit()  # Commit changes

    def insertobject(self, id, ip, name):
        query = "INSERT INTO object(id, ip, name)" \
                " VALUES ('{}','{}','{}')".format(id, ip, name)

        self.cursor.execute(query)  # Executes the
        self.connection.commit()  # Commit changes

    def insertobjectlocation(self, id_object, id_location):
        query = "SELECT count(*) FROM object_location WHERE fk_object ='{}' and fk_location='{}'".format(id_object, id_location)
        self.cursor.execute(query)
        self.connection.commit()
        ret = self.cursor.fetchall()[0][0]
        if ret == 0:
            query = "INSERT INTO object_location(fk_object, fk_location)" \
                 " VALUES ('{}','{}')".format(id_object, id_location)
        self.cursor.execute(query)  # Executes the
        self.connection.commit()  # Commit changes

    def remove_object_location(self, id_object):
        query = "DELETE from object_location where fk_object='{}'".format(id_object)

        self.cursor.execute(query)  # Executes the
        self.connection.commit()  # Commit changes

    def findobject(self, name):
        query = "SELECT id from object where name='{}'".format(name)
        print(query)
        self.cursor.execute(query)
        try:
            id = self.cursor.fetchall()[0][0]
            query = "SELECT name from location where id=(SELECT fk_location FROM object_location as ol WHERE ol.fk_object='{}')".format(
                id)
            print(query)
            self.cursor.execute(query)
            self.connection.commit()
            return self.cursor
        except IndexError:
            return -1

if __name__ == "__main__":
    i = Interface("tools","postgres", "ivan127")
    i.connect()
    i.insertobject("FF:FF", "192.168.0.1", "tool box")
    # i.insertobject("BB:BB", "192.192.0.1", "drill")
    i.insertlocation("AA:AA", "10.10.0.1", "conference room")
    # i.insertlocation("CC:FF", "10.10.0.35", "kitchen")
    i.insertobjectlocation("FF:FF", "AA:AA") #Hammer in kitchen
    # i.insertobjectlocation("BB:BB","CC:AA") #Drill in bathroom
    # i.remove_object_location("AA:BB")

    #ret = i.findobject("AA:BB")
    #print(ret.fetchall()[0][0])