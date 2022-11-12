import struct
import pandas as pd
from datetime import datetime
from scale_values import ScaleData

class BinaryFileDeconstructor:
    now = datetime.now()
    def __init__(
        self,
        fp,
        buffer_size=28,
        struct_format="iiiiiii",
        columns=["time", "x1", "y1", "z1", "x2", "y2", "z2"],
    ) -> None:
        self.fp = fp
        self.buffer_size = buffer_size
        self.struct_format = struct_format
        self.columns = columns

    def convert_to_list_records(self, fp=None):
        if fp is None:
            fp = self.fp
        data = open(fp, "rb").read()
        start = 0
        end = self.buffer_size
        data_list = []
        record = dict()
        end_of_file = False
        while not end_of_file:
            try:
                
                buffer = data[start:end]
                data_collection = struct.unpack(self.struct_format, buffer)
                print(data_collection)
            except struct.error as e:
                print("reached the end of file")
                end_of_file = True  # don't think I need this
                return data_list

            start += self.buffer_size
            end += self.buffer_size

            for col, data_point in zip(self.columns, data_collection):
                record[col] = data_point
            
            data_list.append(record.copy())

    def convert_to_csv(
        self, records, file_name, folder_destination="data/parsed_bin_files/"
    ) -> pd.DataFrame:
        df = pd.DataFrame.from_records(records)
        df = ScaleData().process(df)
        df = df.set_index("time")
        df.to_csv(f"{folder_destination}{file_name}")
        return df

    def process(
        self,
        save_file_name=f"{now.month}_{now.day}_{now.year}_{now.hour}_{now.minute}.csv",
    ):
        records = self.convert_to_list_records()
        df = self.convert_to_csv(records, save_file_name)
        return df
        


if __name__ == "__main__":
    fp = "data/binary_files/161038.txt"
    BinaryFileDeconstructor(fp).process()
