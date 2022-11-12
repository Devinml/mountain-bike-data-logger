import struct
import pandas as pd


class FileDeconstructor:
    def __init__(
        self,
        fp,
        buffer_size=28,
        struct_format="iffffff",
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
            except IndexError as e:
                print("reached the end of file")
                end_of_file = True  # don't think I need this
                return data_list

            start += self.buffer_size
            end += self.buffer_size
            for col, data_point in zip(self.columns, data_collection):
                record[col] = data_point
            data_list.append(record)

    def convert_to_csv(
        self, records, file_name, folder_destination="data/parsed_bin_files/"
    ) -> None:
        df = pd.DataFrame.from_records(records)
        df = df.set_index("time")
        df.to_csv(f"{folder_destination}{file_name}")


if __name__ == "__main__":
    fp = "data/161038.txt"
    FileDeconstructor(fp).process_file()
