import RPi.GPIO as GPIO
bounce_time = 200
PIN_A = 11
a_value = 0

def inputLow(channel):
        if channel == PIN_A:
            a_value = 1
            print(a_value)
		
GPIO.setmode(GPIO.BOARD)
GPIO.setup(PIN_A, GPIO.IN, pull_up_down=GPIO.PUD_DOWN)
while 1:
    if GPIO.input(PIN_A) == 0:
        print ("Leer")
    else:
        print ("besetzt")

