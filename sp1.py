import RPi.GPIO as GPIO# Importa la biblioteca para controlar los pines GPIO del Raspberry Pi
import time# Importa la biblioteca para manejar funciones relacionadas con el tiempo
import serial# Importa la biblioteca para la comunicación serie (UART)
import smtplib #Se utiliza para mandar los mensajes
from email.message import EmailMessage #Se utilliza para configurar los mensajes de email

en = [14,15,27,22] #el orden en para conectar al puente H es EN1, EN4, EN2, EN3
o1 = 13 #Se conecta al ENA en el puente H(motorA)
o2 = 19 #Se conecta al ENB en el puente H(motorB)

trig = 23 # Pin para enviar la señal de activación al sensor ultrasónico
echo = 24 # Pin para recibir la señal de eco del sensor ultrasónico

sense = 0 #Variable para controlar el sentido de giro
mail = 0 #Variable para controlar el envio del mensaje
dist = 0 #Variable para almacenar el valor del sensor de distancia
value = '0' #Valor con el que se controla el moviiento del auto por uart o ssh
m = '0' #Valor para cambiar la recepcion del movimiento por ssh o por uart
s = '0' #Valor para almacenar el modo en el que se reciben los datos 0:uart, 1:ssh

emaildir = "" #Almacena la direccion del correo electronico para enviar el mail
emailpass = "" #La contrasegna tiene que ser la configurada para aplicaciones de terceros del email configurado

GPIO.setwarnings(False) # Desactiva las advertencias de GPIO
GPIO.setmode(GPIO.BCM) # Configura el modo de numeración de pines a BCM

ser = serial.Serial('/dev/ttyACM0', 9600) # Abre el puerto serie para comunicación a 9600 baudios

#Mensajes y encabezados para el mensaje email
messages = ["The car was stopped\nA nearby object was detected", "Car functionaiting"]# Mensajes para enviar
subjects = ["Stopped Car", "Car initializated"]# Asuntos de los correos electrónicos

GPIO.setup(en, GPIO.OUT, initial=0) # Configura los pines de habilitación como salidas, inicializados en 0
GPIO.setup(o1, GPIO.OUT) # Configura el pin o1 como salida
GPIO.setup(o2, GPIO.OUT) # Configura el pin o2 como salida
GPIO.setup(trig, GPIO.OUT, initial = 0)  # Configura el pin trig como salida, inicializado en 0
GPIO.setup(echo, GPIO.IN) # Configura el pin echo como entrada

out1 = GPIO.PWM(o1,100) # Crea un objeto PWM en o1 con frecuencia de 100 Hz
out2 = GPIO.PWM(o2,100) # Crea un objeto PWM en o2 con frecuencia de 100 Hz
out1.start(0)# Inicia el PWM en o1 con ciclo de trabajo 0%
out2.start(0)# Inicia el PWM en o2 con ciclo de trabajo 0%

def movement(val, d):
    global sense# Permite modificar la variable sense
    global mail# Permite modificar la variable mail
    if val == '0':# Si el valor de control es '0'
        if d < 5:# Si la distancia medida es menor a 5 cm
            mail = 1# Indica que se debe enviar un correo
        out1.ChangeDutyCycle(0)# Detiene el motor A
        out2.ChangeDutyCycle(0)# Detiene el motor B
    elif val == '1': # Si el valor de control es '1' (mover hacia adelante)
        sense = 0# Establece el sentido de giro hacia adelante
        if mail == 1:# Si se debe enviar un correo
            mail = 2# Cambia el estado para enviar un correo 
        GPIO.output(en[:2], 1) # Activa los pines de habilitación para motores A y B
        GPIO.output(en[2:], 0)# Desactiva los pines de habilitación restantes
        out1.ChangeDutyCycle(50)# Establece el ciclo de trabajo del motor A al 50%
        out2.ChangeDutyCycle(50)# Establece el ciclo de trabajo del motor B al 50%
    elif val == '2':#atras
        sense = 1
        GPIO.output(en[:2], 0)
        GPIO.output(en[2:], 1)
        out1.ChangeDutyCycle(50)
        out2.ChangeDutyCycle(50)
    elif val == '3':#derecha
        if sense == 1:
            GPIO.output(en, 0)
            GPIO.output(en[3], 1)## Activa el pin de habilitación para el motor B
        else:
            GPIO.output(en, 0)
            GPIO.output(en[0], 1)# Activa el pin de habilitación para el motor A
        out1.ChangeDutyCycle(50)
        out2.ChangeDutyCycle(50)
    elif val == '4':# izquierda
        if sense == 1:
            GPIO.output(en, 0)
            GPIO.output(en[2], 1)# Activa el pin de habilitación para el motor A
        else:
            GPIO.output(en, 0)
            GPIO.output(en[1], 1)# Activa el pin de habilitación para el motor B
        out1.ChangeDutyCycle(50)
        out2.ChangeDutyCycle(50)

def distance():
    GPIO.output(trig, GPIO.HIGH)  # Envía una señal alta para activar el sensor ultrasónico
    time.sleep(0.00001)  # Espera 10 microsegundos
    GPIO.output(trig, GPIO.LOW)  # Establece el pin trig a bajo
    while True:  # Espera a que se reciba la señal de eco
        pulso_inicio = time.time()  # Registra el tiempo de inicio
        if GPIO.input(echo) == GPIO.HIGH:  # Si el pin echo recibe señal alta
            break  # Sale del bucle
    while True:  # Espera a que la señal de eco se detenga
        pulso_fin = time.time()  # Registra el tiempo de fin
        if GPIO.input(echo) == GPIO.LOW:  # Si el pin echo recibe señal baja
            break  # Sale del bucle
    duracion = pulso_fin - pulso_inicio  # Calcula la duración del pulso
    distancia = (34300 * duracion) / 2  # Calcula la distancia en centímetros

    return distancia  # Devuelve la distancia medida

def emailSent(mess, sub):
    global mail  # Permite modificar la variable mail
    global emaildir  # Permite modificar la dirección de correo
    global emailpass  # Permite modificar la contraseña del correo
    if mail != 0:  # Si hay un mensaje para enviar
        msg = EmailMessage()  # Crea un nuevo mensaje de correo
        msg['From'] = "Raspberry Pi Car"  # Establece el remitente
        msg['To'] = "emmanuel.idiaquez@ucb.edu.bo"  # Establece el destinatario
        if mail == 1:  # Si el estado es 1, se envía el primer mensaje
            msg['Subject'] = sub[0]  # Establece el asunto del primer mensaje
            msg.set_content(mess[0])  # Establece el contenido del primer mensaje
        elif mail == 2:  # Si el estado es 2, se envía el segundo mensaje
            msg['Subject'] = sub[1]  # Establece el asunto del segundo mensaje
            msg.set_content(mess[1])  # Establece el contenido del segundo mensaje

        
        # Establece una conexión con el servidor SMTP de Gmail en el puerto 587.
        s = smtplib.SMTP('smtp.gmail.com', 587)
        s.starttls()  # Inicia el protocolo TLS para una conexión segura.
        s.login(emaildir, emailpass)  # Inicia sesión en el servidor con las credenciales proporcionadas.
        s.send_message(msg)  # Envía el mensaje previamente configurado.
        s.quit()  # Cierra la conexión con el servidor SMTP.
        mail = 0  # Resetea el estado de 'mail' a 0 después de enviar el correo.
        del msg  # Elimina el objeto 'msg' para liberar memoria.

# Bucle principal que se ejecuta indefinidamente.
while True:
    try:
        file = open("config.txt", 'r')# Abre el archivo 'config.txt' en modo lectura.
        config = file.readlines() # Lee todas las líneas del archivo y las almacena en una lista 'config'.
        m = config[0].rstrip()[-1]# Obtiene el último carácter de la primera línea y lo asigna a 'm'.
        s = config[1].rstrip()[-1]# Obtiene el último carácter de la segunda línea y lo asigna a 's'.
        dist = round(distance(),2)# Llama a la función 'distance' y redondea el resultado a dos decimales.
        if (dist < 5):# Si la distancia medida es menor a 5 cm
            value = '0'# Establece 'value' en '0', indicando que se debe detener el vehículo.
#            emailSent(messages, subjects) # Aquí se puede llamar a la función para enviar un correo.
            print("Email sended")# Imprime un mensaje de que se envió un correo.
        else:# Si la distancia es mayor o igual a 5 cm
            # Verifica el modo de operación.
            if m == '0':# Si el modo es '0' (UART)
                if ser.in_waiting > 0:# Si hay datos disponibles en el puerto serie
                    value = ser.readline().decode('utf-8').rstrip() # Lee y decodifica el dato recibido.
            elif m == '1':# Si el modo es '1' (SSH)
                value = s# Asigna 's' a 'value', que indica el comando a seguir.
            else:
                # Si el modo no es válido, detiene el movimiento.
                out1.ChangeDutyCycle(0)# Establece el ciclo de trabajo a 0 (detiene el motor 1).
                out2.ChangeDutyCycle(0)# Establece el ciclo de trabajo a 0 (detiene el motor 2).
                print('incorrect mode value')# Imprime un mensaje de error.
# Llama a la función 'movement' con los valores 'value' y 'dist'.
        movement(value,dist)
        d = str(dist*100)+'\n'# Convierte la distancia a centímetros y le añade un salto de línea.
        ser.write(d.encode('utf-8'))# Envía la distancia a través del puerto serie en formato UTF-8.
        file.close()# Cierra el archivo 'config.txt' después de leerlo.
    except Exception as e:# Captura cualquier excepción que ocurra durante la ejecución del bloque try.
        print(e)# Imprime el error en la consola.
