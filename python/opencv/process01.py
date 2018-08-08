import sys
import os
import tensorflow as tf

imageDirs = ["김연아", "박지성", "장동건", "전지현", "정우성"]


X_train = []
t_train = []

for index, imgdir in enumerate(imageDirs):

    print(index, imgdir)
    imageDirPath = os.path.join(imgdir, "cropped")
    files = [f for f in os.listdir(imageDirPath) if os.path.isfile(os.path.join(imageDirPath, f))]

    for f in files:
        imageFilePath = os.path.join(imageDirPath, f)




    X_train.
