import os
from parse_bin import BinaryFileDeconstructor

dirs = os.listdir("data/binary_files")


for file_ in dirs:
    file_name = file_.split(".txt")[0]
    file_name += '.csv'
    full_path = "data/binary_files/" + file_
    BinaryFileDeconstructor(full_path).process(file_name)
