import psycopg2
import asyncio
import websockets

# Configuración de la conexión a la base de datos
db_host = '192.168.0.15'
db_port = '5432'
db_name = 'postgres'
db_user = 'postgres'
db_password = '1234'

# Configuración del servidor WebSocket
websocket_server = 'ws://192.168.0.8:3007'

# Establecer conexión con la base de datos
conn = psycopg2.connect(
    host=db_host,
    port=db_port,
    database=db_name,
    user=db_user,
    password=db_password
)
cursor = conn.cursor()

# Función para enviar los datos a través del WebSocket
async def send_data():
    async with websockets.connect(websocket_server) as websocket:
        while True:
            # Obtener los datos de la última fila de la tabla lecturas
            query = "SELECT temperatura, humedad, fecha_lecturas FROM lecturas ORDER BY fecha_lecturas DESC LIMIT 1"
            cursor.execute(query)
            result = cursor.fetchone()
            temperatura, humedad, fecha_lecturas = result
            
            # Obtener los datos de la última fila de la tabla luz
            query = "SELECT fecha_lightstate FROM luz ORDER BY fecha_lightstate DESC LIMIT 1"
            cursor.execute(query)
            result = cursor.fetchone()
            fecha_lightstate = result[0]
            
            # Obtener los datos de la última fila de la tabla rfid
            query = "SELECT uid, nombre_acceso, fecha_rfid FROM rfid ORDER BY fecha_rfid DESC LIMIT 1"
            cursor.execute(query)
            result = cursor.fetchone()
            uid, nombre_acceso, fecha_rfid = result
            
            # Obtener los datos de la última fila de la tabla sensor_movimiento
            query = "SELECT fecha_sensormovimiento FROM sensor_movimiento ORDER BY fecha_sensormovimiento DESC LIMIT 1"
            cursor.execute(query)
            result = cursor.fetchone()
            fecha_sensormovimiento = result[0]
            
            # Crear un diccionario con los datos a enviar
            data = {
                'lecturas': {
                    'temperatura': temperatura,
                    'humedad': humedad,
                    'fecha_lecturas': str(fecha_lecturas)
                },
                'luz': {
                    'fecha_lightstate': str(fecha_lightstate)
                },
                'rfid': {
                    'uid': uid,
                    'nombre_acceso': nombre_acceso,
                    'fecha_rfid': str(fecha_rfid)
                },
                'sensor_movimiento': {
                    'fecha_sensormovimiento': str(fecha_sensormovimiento)
                }
            }
            
            # Enviar los datos a través del WebSocket
            await websocket.send(str(data))
            
            # Esperar 10 segundos antes de enviar los siguientes datos
            await asyncio.sleep(10)

# Función auxiliar para iniciar el bucle de eventos
def start():
    asyncio.run(send_data())

# Iniciar el bucle de eventos llamando a la función start
start()
