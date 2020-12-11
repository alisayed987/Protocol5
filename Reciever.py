# Receiver.py
import time, socket, sys
import random

print("\nStarting\n")
print("Initialising....\n")
time.sleep(1)

s = socket.socket()
shost = socket.gethostname()
ip = "192.168.1.6"
print(shost, "(", ip, ")\n")
host = input(str("Enter server address: "))

port = 8080
print("\nTrying to connect to ", host, "(", port, ")\n")
time.sleep(1)
s.connect((host, port))
print("Connected...\n")



print("Devices are now connected\nEnter \"Exit\" to exit chat room\n")

while True:

    m=s.recv(1024)
    m=m.decode()
    k=s.recv(1024)
    k=k.decode()
    k=int(k)
    i=0
    a=""
    b=""
    f=random.randint(0,1)
    message=""
    while i!=k:
       
       
       f=random.randint(0,1)
       if(f==0):
          b="ACK Lost"
          message = s.recv(1024)
          message = message.decode()
          s.send(b.encode())
         
       elif(f==1):
          b="ACK "+str(i)
          message = s.recv(1024)
          message = message.decode()
          
          s.send(b.encode())
          a=a+message
          i=i+1
          
       
    
    print("The message received is :", m)