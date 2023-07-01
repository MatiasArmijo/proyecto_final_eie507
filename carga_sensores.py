import serial
import psycopg2
import time

# Configuración de la conexión a la base de datos
db_host = '192.168.0.15'
db_port = '5432'
db_name = 'postgres'
db_user = 'postgres'
db_password = '1234'

# Configuración del puerto serial
serial_port = '/dev/ttyUSB0'
baud_rate = 9600

# Establecer conexión con la base de datos
conn = psycopg2.connect(
    host=db_host,
    port=db_port,
    database=db_name,
    user=db_user,
    password=db_password
)
cursor = conn.cursor()

# Establecer conexión con el puerto serial
ser = serial.Serial(serial_port, baud_rate)

# Variables para controlar el tiempo de carga de los datos
last_data_time = time.time()

while True:
    # Leer una línea del puerto serial
    line = ser.readline().decode().strip()
    
    # Realizar acciones según el contenido de la línea
    if line.startswith('Temp:') and 'Hum:' in line:
        # Formato: Temp:valor_temp,Hum:valor_hum
        temperatura = line.split(',')[0].split(':')[1]
        humedad = line.split(',')[1].split(':')[1]
        
        current_time = time.time()
        elapsed_time = current_time - last_data_time
        
        if elapsed_time >= 60:
            # Insertar los valores en la tabla temperatura_humedad
            query = "INSERT INTO lecturas (temperatura, humedad) VALUES (%s, %s)"
            cursor.execute(query, (float(temperatura), float(humedad)))
            conn.commit()
            
            # Actualizar el último tiempo de carga de datos
            last_data_time = current_time
        
    elif line.endswith('[PERMITIDO]'):
        # Formato: string_alphanumeric,USUARIO numero_usuario [PERMITIDO]
        uid = line.split(',')[0]
        numero_usuario = line.split(' ')[1]
        
        # Insertar los valores en la tabla rfid
        query = "INSERT INTO rfid (uid, nombre_acceso) VALUES (%s, %s)"
        cursor.execute(query, (uid, f"usuario_{numero_usuario}_permitido"))
        conn.commit()
        
    elif line.endswith('USUARIO NO PERMITIDO'):
        # Formato: string_alphanumeric,USUARIO NO PERMITIDO
        uid = line.split(',')[0]
        
        # Insertar los valores en la tabla rfid
        query = "INSERT INTO rfid (uid, nombre_acceso) VALUES (%s, %s)"
        cursor.execute(query, (uid, 'usuario_no_permitido'))
        conn.commit()
        
    elif line == 'PUERTA ABIERTA':
        # Insertar el valor en la tabla control_puerta
        query = "INSERT INTO control_puerta (door_state) VALUES (%s)"
        cursor.execute(query, ('puerta_abierta',))
        conn.commit()
        
    elif line == 'Sensor de Movimiento activo':
        # Insertar el valor en la tabla sensor_movimiento
        query = "INSERT INTO sensor_movimiento (sensor_state) VALUES (%s)"
        cursor.execute(query, ('ACTIVO',))
        conn.commit()
        
    elif line == 'LUZ ACTIVA':
        # Insertar el valor en la tabla luz
        query = "INSERT INTO luz (light_state) VALUES (%s)"
        cursor.execute(query, ('luz_prendida',))
        conn.commit()

# Cerrar la conexión con el puerto serial
ser.close()

# Cerrar la conexión con la base de datos
cursor.close()
conn.close()
