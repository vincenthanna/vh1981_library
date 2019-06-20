import numpy as np
import os
from numpy import genfromtxt
from keras import backend as K
from keras import models, layers
from keras.layers import Conv2D, ZeroPadding2D, Activation, Input, concatenate
from keras.models import Model
from keras.layers.normalization import BatchNormalization
from keras.layers.pooling import MaxPooling2D, AveragePooling2D
from keras.layers.core import Lambda, Flatten, Dense

def face_recognition_model(input_shape):
    """Face Recognition Model 생성

    input_shape의 이미지들을 입력받아서 (None, 128)의 vector로 출력한다.

    Args:
        input_shape -- shape of the images of the dataset

    Returns:
        model -- a Model() instance in Keras
    """
    # Input 생성
    input = Input(input_shape)

    h = Conv2D(filters=32, kernel_size=(3,3), strides=(2,2), activation='relu', input_shape=input_shape)(input)
    h = MaxPooling2D(pool_size=(2, 2))(h)
    h = BatchNormalization(axis=1, name='bn1')(h)

    h = Conv2D(filters=64, kernel_size=(3, 3), strides=(2, 2), activation='relu', input_shape=input_shape)(h)
    h = BatchNormalization(axis=1, name='bn2')(h)

    h = Conv2D(filters=128, kernel_size=(3, 3), strides=(2, 2), activation='relu', input_shape=input_shape)(h)
    h = BatchNormalization(axis=1, name='bn3')(h)

    h = Conv2D(filters=256, kernel_size=(3, 3), strides=(2, 2), activation='relu', input_shape=input_shape)(h)
    h = BatchNormalization(axis=1, name='bn4')(h)

    h = Flatten()(h)

    h = Dense(512, activation='relu')(h)
    h = Dense(256, activation='relu')(h)
    h = Dense(128, activation='relu')(h)

    h = Dense(128, activation='softmax', name='prediction')(h)

    model = Model(inputs=input, outputs=h, name='FaceRecognitionModel')

    return model