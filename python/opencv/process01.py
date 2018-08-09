import sys
import os
import tensorflow as tf
import numpy as np
import matplotlib.pyplot as plt

imageDirs = ["김연아", "박지성", "장동건", "전지현", "정우성"]

flags = tf.app.flags

flags.DEFINE_integer

FLAGS = flags.FLAGS
FLAGS.image_width = 96
FLAGS.image_height = 96
FLAGS.image_color = 3
FLAGS.num_classes = len(imageDirs)
FLAGS.batch_size = 100
FLAGS.learning_rate = 0.0001



X_train = []
t_train = []

def get_img(fname):
    imageFilePath = os.path.join(imageDirPath, f)
    image = tf.image.decode_jpeg(tf.read_file(imageFilePath), channels=3)
    # print(imageFilePath)
    image = tf.reshape(image, [96, 96, 3])
    return image

def makeNN_conv2(input_data):
    '''
    conv2_layer_size는 이 conv계층의 뉴런 수임.
    conv2_filter_size는 필터 사이즈
    stride2는 필터 이동 속도
    :param input_data:
    :return:
    '''
    conv2_filter_size = 3
    conv2_layer_size = 32
    stride2 = 1

    with tf.name_scope('conv_2'):
        W_conv2 = tf.Variable(tf.truncated_normal(
            [conv2_filter_size, conv2_filter_size, conv2_layer_size, conv2_layer_size], stddev=0.1))
        b2 = tf.Variable(tf.truncated_normal([conv2_layer_size], stddev=0.1))
        h_conv2 = tf.nn.conv2d(input_data,
                               W_conv2,
                               strides=[1,1,1,1],
                               padding='SAME')
        h_conv2_relu = tf.nn.relu(tf.add(h_conv2, b2))
        h_conv2_maxpool = tf.nn.max_pool(h_conv2_relu,
                                         ksize=[1,2,2,1],
                                         strides=[1,2,2,1],
                                         padding='SAME')

    return h_conv2_maxpool


def makeNN_fc1(input_data):
    conv4_layer_size = 5
    input_layer_size = 6 * 6 *conv4_layer_size

    FLAGS.fc1_layer_size = 512

    with tf.name_scope('fc_1'):
        # 앞에서 입력받은 다차원 텐서를 fcc에 넣기 위해서 1차원으로 피는 작업

        input_data_reshape = tf.reshape(input_data, [-1, input_layer_size])

        W_fc1 = tf.Variable(tf.truncated_normal([input_layer_size, FLAGS.fc1_layer_size], stddev=0.1))

        b_fc1 = tf.Variable(tf.truncated_normal(

            [FLAGS.fc1_layer_size], stddev=0.1))

        h_fc1 = tf.add(tf.matmul(input_data_reshape, W_fc1), b_fc1)  # h_fc1 = input_data*W_fc1 + b_fc1

        h_fc1_relu = tf.nn.relu(h_fc1)

    return h_fc1_relu




dataIndex = 0
for index, imgdir in enumerate(imageDirs):

    print(index, imgdir)
    imageDirPath = os.path.join(imgdir, "cropped")
    files = [f for f in os.listdir(imageDirPath) if os.path.isfile(os.path.join(imageDirPath, f))]
    label = index

    for f in files:
        image = get_img(f)

        '''
        image = tf.image.random_flip_left_right(image)
        image = tf.image.random_brightness(image, max_delta=0.5)
        image = tf.image.random_contrast(image, lower=0.2, upper=2.0)
        image = tf.image.random_hue(image, max_delta=0.08)
        image = tf.image.random_saturation(image, lower=0.2, upper=2.0)

        batch_images = tf.train.batch([image], 5)
        '''

        X_train.append(image)
        t_train.append(label)











