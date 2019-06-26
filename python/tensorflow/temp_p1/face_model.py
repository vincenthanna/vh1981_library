import numpy as np
import os
import tensorflow as tf
from tensorflow.layers import max_pooling2d, conv2d, batch_normalization, flatten, dense
from tensorflow.nn import relu

def all_diffs(a, b):
    # Returns a tensor of all combinations of a - b
    print("shapes:", a.shape, b.shape)
    return tf.expand_dims(a, axis=1) - tf.expand_dims(b, axis=0)


def euclidean_dist(embed1, embed2):
    # Measures the euclidean dist between all samples in embed1 and embed2

    diffs = all_diffs(embed1, embed2)  # get a square matrix of all diffs
    print("diffs.shape=", diffs.shape)
    return tf.sqrt(tf.reduce_sum(tf.square(diffs), axis=-1) + 1e-12)


TL_MARGIN = 0.2  # The minimum distance margin


def triplet_loss(dists, labels):
    # Defines the "batch hard" triplet loss function.
    print("labels.shape", labels)
    same_identity_mask = tf.equal(tf.expand_dims(labels, axis=1),
                                  tf.expand_dims(labels, axis=0))
    print("same_identity_mastf.shape=", same_identity_mask.shape)

    aaa = tf.eye(tf.shape(labels)[0], dtype=tf.bool)
    print("tf.eye.shape=", aaa.shape)

    negative_mask = tf.logical_not(same_identity_mask)
    positive_mask = tf.logical_xor(same_identity_mask,
                                   tf.eye(tf.shape(labels)[0], dtype=tf.bool))

    furthest_positive = tf.reduce_max(dists * tf.cast(positive_mask, tf.float32), axis=1)
    closest_negative = tf.map_fn(lambda x: tf.reduce_min(tf.boolean_mask(x[0], x[1])),
                                 (dists, negative_mask), tf.float32)

    diff = furthest_positive - closest_negative

    return tf.maximum(diff + TL_MARGIN, 0.0)


def face_recognition_model(images):
    """Face Recognition Model 생성

    image를 입력받아 (None, 128)의 vector로 출력한다.

    Args:
        images -- image 배열 혹은 tensor

    Returns:
        tensor layer object
    """

    h = conv2d(images, filters=32, kernel_size=(3,3), strides=(2, 2), activation=relu)
    h = max_pooling2d(h, pool_size=(2, 2), strides=(1,1))
    h = batch_normalization(h, name='bn1')

    h = conv2d(h, filters=64, kernel_size=(3, 3), strides=(2, 2), activation='relu')
    h = batch_normalization(h, name='bn2')

    h = conv2d(h, filters=128, kernel_size=(3, 3), strides=(2, 2), activation='relu')
    h = batch_normalization(h, name='bn3')

    h = conv2d(h, filters=256, kernel_size=(3, 3), strides=(2, 2), activation='relu')
    h = batch_normalization(h, name='bn4')

    h = flatten(h)

    h = dense(h, 512, activation='relu')
    h = dense(h, 256, activation='relu')

    hypothesis = dense(h, 128, activation='softmax', name='prediction')

    return hypothesis