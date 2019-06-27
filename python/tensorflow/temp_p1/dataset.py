import sys
import os
import tensorflow as tf
import numpy as np
import matplotlib.pyplot as plt
from sklearn.utils import shuffle

from keras.preprocessing.image import ImageDataGenerator

# pyplot 한글 출력
import matplotlib.font_manager as fm

from PIL import Image

import matplotlib
print(matplotlib.matplotlib_fname())

'''
pyplot에 기본 설정된 폰트가 한글을 지원하지 않아서 title의 한글 등이 깨지므로
한글을 지원하는 폰트 명으로 교체한다.
'''
font_name = "NanumGothic"
matplotlib.rc('font', family=font_name)


def prepare_data():
    names = {}
    idx = 0
    for subdir in os.listdir("./PINS"):
        name = subdir[5:]
        names[name] = idx
        idx = idx + 1
    print(names)

    labels = []
    imgpaths = []

    for subdir in os.listdir("./PINS"):
        name = subdir[5:]
        # print(name)
        idx = names[name]
        print(idx)
        cur_dir = "./PINS/" + subdir
        files = [os.path.join(cur_dir, f) for f in os.listdir(cur_dir)
                 if os.path.isfile( os.path.join(cur_dir, f) ) and (f.find("jpg") > 0 or f.find("jpeg") > 0)]
        print("len(files):", len(files))

        labels.extend([idx for i in range(len(files))])
        imgpaths.extend(files)

    # TODO: shuffle을 한번 해줘야 한다.
    labels, imgpaths = shuffle(labels, imgpaths)


        # 실제 이미지는 batch 넘길때 load하면 된다.
        # 이미지 loading은 아래 코드 참조.
        # for file in files:
        #     img = np.array(Image.open(file).resize((200, 200)))
        #     # print("img.shape", img.shape)
        #     # plt.imshow(img)
        #     # plt.show()
        #     images.append(img)

    print(len(labels), len(imgpaths))

    return labels, imgpaths, names


def get_batch(data, batch_size, idx, isImgPath=False):
    """data에서 batch_size * idx ~ batch_size * (idx+1)만큼 잘라서 리턴
    :param data: 전체 데이터 크기
    :param batch_size:batch 크기
    :param idx:batch 데이터 인덱스(몇번째?)
    :return: idx순서의 batch_size의 slice된 데이터
    """
    if isImgPath:
        imgpaths = data[batch_size * idx: batch_size * (idx + 1)]
        images = []
        for imgpath in imgpaths:
            img = np.array(Image.open(imgpath).resize((200, 200)))
            images.append(img)
        return np.array(images)
    else:
        ret = data[batch_size * idx: batch_size * (idx+1)]
        return np.array(ret)












