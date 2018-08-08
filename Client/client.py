import socket
import picamera
import datetime

host = '192.168.1.4'
port = 5560

def connect():
	s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
	
	try: 
		s.connect((host, port))
		print('Connected to: {}'.format(host))
	except:
		s.close()
		raise
	
	return s
	
def capture(cn):

	with picamera.PiCamera() as camera:
		
		for i in range(0, cn):
				camera.capture("/home/pi/Documents/cpp/image_{}_".format(i) + str(datetime.datetime.now()) + ".jpg")
	

	
	
if __name__=="__main__":
	
	s = connect()
	
	while True:
		cn = s.recv(1024)
		if cn:
			print(cn)
			capture(int(cn))
			s.send(str.encode('Capture Successful!'))
			print('Capture {} image(s)'.format(cn)) 

	
	

