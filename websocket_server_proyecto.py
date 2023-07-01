import asyncio
import websockets

# Función que se ejecuta cuando se recibe un mensaje del cliente
async def receive_data(websocket, path):
    # Realizar las acciones deseadas con los datos recibidos
    while True:
        data = await websocket.recv()
        print("Datos recibidos:", data)

# Iniciar el servidor WebSocket
start_server = websockets.serve(receive_data, '192.168.0.8', 3007)  # Reemplazar 'localhost' por la dirección deseada y 8765 por el puerto deseado

# Ejecutar el servidor en un bucle de eventos
async def run_server():
    await start_server
    await asyncio.Event().wait()

asyncio.get_event_loop().run_until_complete(run_server())