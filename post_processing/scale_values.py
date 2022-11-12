
class ScaleData:
    def __init__(self, LIS3DH_LSB16_TO_KILO_LSB10=64000, lsb_value = 48):
        # https://github.com/adafruit/Adafruit_LIS3DH/blob/master/Adafruit_LIS3DH.cpp#L217
        # http://adafruit.github.io/Adafruit_LIS3DH/html/_adafruit___l_i_s3_d_h_8h.html
        self.error_value = LIS3DH_LSB16_TO_KILO_LSB10
        self.lsb = lsb_value
    
    def process(self, df):
        for col in df.columns:
            if col != "time":
                df[col] = (df[col] * self.lsb) / self.error_value
                # df[col] = df[col] / self.error_value

        return df 

    def save_and_process(self, df, fp):
        df = self.process(df)
        df.to_csv(fp)
        return df