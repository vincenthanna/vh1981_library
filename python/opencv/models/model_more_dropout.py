#!/usr/bin/env python
# -*- coding: UTF-8 -*-

'''
model_jodaehyub 에서 dropout이 의심되어 현재 마지막 layer에만 적용되어 있는 dropout을 fc 전체에 추가하였음.
'''

import sys
import os
import tensorflow as tf
import numpy as np

# convolutional network 1
def conv1(input_data):
    with tf.name_scope('conv_1'):
        W_conv1 = tf.Variable(tf.truncated_normal([3, 3, 3, 16], stddev=0.1)) #필터 (w, h, colorspace(input filter count), out_filter_count)
        b1 = tf.Variable(tf.truncated_normal([16], stddev=0.1))
        h_conv1 = tf.nn.conv2d(input_data, W_conv1, strides=[1, 1, 1, 1], padding='SAME')
        h_conv1_relu = tf.nn.relu(tf.add(h_conv1, b1))
        h_conv1_maxpool = tf.nn.max_pool(h_conv1_relu
                                         , ksize=[1, 2, 2, 1]
                                         , strides=[1, 2, 2, 1], padding='SAME')

    return h_conv1_maxpool


# convolutional network layer 2
def conv2(input_data):
    with tf.name_scope('conv_2'):
        W_conv2 = tf.Variable(tf.truncated_normal([3, 3, 16, 32],stddev=0.1))
        b2 = tf.Variable(tf.truncated_normal([32], stddev=0.1))
        h_conv2 = tf.nn.conv2d(input_data, W_conv2, strides=[1, 1, 1, 1], padding='SAME')
        h_conv2_relu = tf.nn.relu(tf.add(h_conv2, b2))
        h_conv2_maxpool = tf.nn.max_pool(h_conv2_relu
                                         , ksize=[1, 2, 2, 1]
                                         , strides=[1, 2, 2, 1], padding='SAME')

    return h_conv2_maxpool


# convolutional network layer 3
def conv3(input_data):
    with tf.name_scope('conv_3'):
        W_conv3 = tf.Variable(tf.truncated_normal([3, 3, 32, 64],stddev=0.1))
        b3 = tf.Variable(tf.truncated_normal([64], stddev=0.1))
        h_conv3 = tf.nn.conv2d(input_data, W_conv3, strides=[1, 1, 1, 1], padding='SAME')
        h_conv3_relu = tf.nn.relu(tf.add(h_conv3, b3))
        h_conv3_maxpool = tf.nn.max_pool(h_conv3_relu
                                         , ksize=[1, 2, 2, 1]
                                         , strides=[1, 2, 2, 1], padding='SAME')

    return h_conv3_maxpool


# convolutional network layer 3
def conv4(input_data):
    with tf.name_scope('conv_4'):
        W_conv4 = tf.Variable(tf.truncated_normal(
            [5, 5, 64, 128],
            stddev=0.1))
        b4 = tf.Variable(tf.truncated_normal(
            [128], stddev=0.1))
        h_conv4 = tf.nn.conv2d(input_data, W_conv4, strides=[1, 1, 1, 1], padding='SAME')
        h_conv4_relu = tf.nn.relu(tf.add(h_conv4, b4))
        h_conv4_maxpool = tf.nn.max_pool(h_conv4_relu
                                         , ksize=[1, 2, 2, 1]
                                         , strides=[1, 2, 2, 1], padding='SAME')

    return h_conv4_maxpool


# fully connected layer 1
def fc1(input_data):
    with tf.name_scope('fc_1'):
        # 앞에서 입력받은 다차원 텐서를 fcc에 넣기 위해서 1차원으로 피는 작업
        input_data_reshape = tf.reshape(input_data, [-1, 6 * 6 * 128])
        W_fc1 = tf.Variable(tf.truncated_normal([6 * 6 * 128, 512], stddev=0.1))
        b_fc1 = tf.Variable(tf.truncated_normal([512], stddev=0.1))
        h_fc1 = tf.add(tf.matmul(input_data_reshape, W_fc1), b_fc1)  # h_fc1 = input_data*W_fc1 + b_fc1
        h_fc1_relu = tf.nn.relu(h_fc1)

    return h_fc1_relu


# fully connected layer 2
def fc2(input_data):
    with tf.name_scope('fc_2'):
        W_fc2 = tf.Variable(tf.truncated_normal([512, 256], stddev=0.1))
        b_fc2 = tf.Variable(tf.truncated_normal([256], stddev=0.1))
        h_fc2 = tf.add(tf.matmul(input_data, W_fc2), b_fc2)  # h_fc1 = input_data*W_fc1 + b_fc1
        h_fc2_relu = tf.nn.relu(h_fc2)

    return h_fc2_relu


# final layer
def final_out(input_data, labelCnt):
    with tf.name_scope('final_out'):
        W_fo = tf.Variable(tf.truncated_normal([256, labelCnt], stddev=0.1))
        b_fo = tf.Variable(tf.truncated_normal([labelCnt], stddev=0.1))
        h_fo = tf.add(tf.matmul(input_data, W_fo), b_fo)  # h_fc1 = input_data*W_fc1 + b_fc1

    # 최종 레이어에 softmax 함수는 적용하지 않았다.

    return h_fo


# build cnn_graph
def build_model_more_dropout(images, keep_prob, labelCnt):
    # define CNN network graph
    # output shape will be (*,48,48,16)
    r_cnn1 = conv1(images)  # convolutional layer 1
    print("shape after cnn1 ", r_cnn1.get_shape())

    r_dropout_cnn1 = tf.nn.dropout(r_cnn1, keep_prob)

    # output shape will be (*,24,24,32)
    r_cnn2 = conv2(r_dropout_cnn1)  # convolutional layer 2
    print("shape after cnn2 :", r_cnn2.get_shape())

    r_dropout_cnn2 = tf.nn.dropout(r_cnn2, keep_prob)

    # output shape will be (*,12,12,64)
    r_cnn3 = conv3(r_dropout_cnn2)  # convolutional layer 3
    print("shape after cnn3 :", r_cnn3.get_shape())

    r_dropout_cnn3 = tf.nn.dropout(r_cnn3, keep_prob)

    # output shape will be (*,6,6,128)
    r_cnn4 = conv4(r_dropout_cnn3)  # convolutional layer 4
    print("shape after cnn4 :", r_cnn4.get_shape())

    r_dropout_cnn4 = tf.nn.dropout(r_cnn4, keep_prob)

    # fully connected layer 1
    r_fc1 = fc1(r_dropout_cnn4)
    print("shape after fc1 :", r_fc1.get_shape())

    # dropout r_fc1
    r_dropout_fc1 = tf.nn.dropout(r_fc1, keep_prob)

    # fully connected layer2
    r_fc2 = fc2(r_dropout_fc1)
    print("shape after fc2 :", r_fc2.get_shape())

    ## drop out
    # 참고 http://stackoverflow.com/questions/34597316/why-input-is-scaled-in-tf-nn-dropout-in-tensorflow
    # 트레이닝시에는 keep_prob < 1.0 , Test 시에는 1.0으로 한다.
    r_dropout = tf.nn.dropout(r_fc2, keep_prob)
    print("shape after dropout :", r_dropout.get_shape())

    # final layer
    r_out = final_out(r_dropout, labelCnt=labelCnt)
    print("shape after final layer :", r_out.get_shape())

    return r_out