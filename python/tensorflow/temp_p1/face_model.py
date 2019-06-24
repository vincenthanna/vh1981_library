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

def all_diffs(a, b):
    # Returns a tensor of all combinations of a - b
    print("shapes:", a.shape, b.shape)

    return K.expand_dims(a, axis=1) - K.expand_dims(b, axis=0)


def euclidean_dist(embed1, embed2):
    # Measures the euclidean dist between all samples in embed1 and embed2

    diffs = all_diffs(embed1, embed2)  # get a square matrix of all diffs
    print("diffs.shape=", diffs.shape)
    return K.sqrt(K.reduce_sum(K.square(diffs), axis=-1) + 1e-12)


TL_MARGIN = 0.2  # The minimum distance margin


def triplet_loss(dists, labels):
    # Defines the "batch hard" triplet loss function.
    print("labels.shape", labels)
    same_identity_mask = K.equal(K.expand_dims(labels, axis=1),
                                  K.expand_dims(labels, axis=0))
    print("same_identity_mask.shape=", same_identity_mask.shape)

    aaa = K.eye(K.shape(labels)[0], dtype=K.bool)
    print("K.eye.shape=", aaa.shape)

    negative_mask = K.logical_not(same_identity_mask)
    positive_mask = K.logical_xor(same_identity_mask,
                                   K.eye(K.shape(labels)[0], dtype=K.bool))

    furthest_positive = K.reduce_max(dists * K.cast(positive_mask, K.float32), axis=1)
    closest_negative = K.map_fn(lambda x: K.reduce_min(K.boolean_mask(x[0], x[1])),
                                 (dists, negative_mask), K.float32)

    diff = furthest_positive - closest_negative

    return K.maximum(diff + TL_MARGIN, 0.0)


def face_recognition_model(input_shape, labels):
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

    dists = euclidean_dist(model, model)
    loss = K.mean(triplet_loss(dists, labels))

    Model.compile(loss=loss, optimizer='adam', metrics=['accuracy'])

    return model








