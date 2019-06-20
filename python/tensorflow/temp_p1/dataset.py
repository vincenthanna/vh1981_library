import sys
import os
import tensorflow as tf
import numpy as np
import matplotlib.pyplot as plt

from keras.preprocessing.image import ImageDataGenerator

#pyplot 한글 출력
import matplotlib.font_manager as fm

import matplotlib
print(matplotlib.matplotlib_fname() )

'''
pyplot에 기본 설정된 폰트가 한글을 지원하지 않아서 title의 한글 등이 깨지므로
한글을 지원하는 폰트 명으로 교체한다.
'''
font_name = "NanumGothic"
matplotlib.rc('font', family = font_name)






def prepare_data():

    for subdir in os.listdir("./PINS"):
        name = subdir[5:]
        print(name)

        train_imgpaths = []

        labels = []

        cur_dir = "./PINS/" + subdir
        files = [os.path.join(cur_dir, f) for f in os.listdir(cur_dir)
                 if os.path.isfile( os.path.join(cur_dir, f) ) and (f.find("jpg") > 0 or f.find("jpeg") > 0)]










