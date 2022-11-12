import struct 

class FileDeconstructor:
    def __init__(self, fp, buffer_size=28, struct_format="iffffff") -> None:
        self.fp = fp
        self.buffer_size=buffer_size
        self.struct_format = struct_format
        
    def process_file(self, fp=None):
        if fp is None:
            fp = self.fp
        data = open(fp, "rb").read()
        start = 0
        end = self.buffer_size
        for _ in range(3):
            buffer = data[start: end]
            print(struct.unpack(self.struct_format, buffer))
            start += self.buffer_size
            end += self.buffer_size



        
if __name__ == '__main__':
    fp =   "data/163145.txt"

    FileDeconstructor(fp).process_file()