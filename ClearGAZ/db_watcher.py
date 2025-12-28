# db_watcher.py
import time
import socket
import mysql.connector
import sys
sys.stdout.reconfigure(encoding='utf-8')



HOST = '127.0.0.1'
PORT = 65432

# All tables in the cleargaz database
tables_to_watch = [
    "Gaz",
    "Régions",
    "Villes",
    "Type_Gaz",
    "Agences",
    "Personnel",
    "Admins",
    "Techniques",
    "Chefs",
    "Capteurs",
    "Rapports",
    "Données"

]

def get_counts():
    counts = {}
    try:
        # New connection on each call to avoid cached state
        with mysql.connector.connect(
            host='localhost',
            user='root',
            password='@2007',
            database='cleargaz'
        ) as conn:
            with conn.cursor() as cursor:
                for table in tables_to_watch:
                    try:
                        cursor.execute(f"SELECT COUNT(*) FROM `{table}`")
                        result = cursor.fetchone()
                        counts[table] = result[0] if result else 0
                    except mysql.connector.Error as e:
                        print(f"⚠️ Error querying table '{table}': {e}")
                        counts[table] = -1
    except mysql.connector.Error as e:
        print(f"❌ Database connection error: {e}")
        for table in tables_to_watch:
            counts[table] = -1
    return counts

# Set up TCP socket server
server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
server.bind((HOST, PORT))
server.listen(1)
print("✅ Waiting for Qt to connect...")
client_socket, addr = server.accept()
print(f"✅ Qt connected from {addr}")

# Start monitoring
last_counts = get_counts()

while True:
    time.sleep(2)
    current_counts = get_counts()

    if current_counts != last_counts:
        print(f"🔄 Change detected: {current_counts}")
        client_socket.sendall(b"DB_CHANGED\n")
        last_counts = current_counts
