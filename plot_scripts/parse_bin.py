import struct 
data = open("162947.txt", "rb").read()
buffer = data[0:28]
struct.unpack("iffffff", buffer)