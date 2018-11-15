#!/usr/bin/env python
# -*- coding: UTF-8 -*-

'''
simple mnist 모델
'''

import sys
import os
import tensorflow as tf
import numpy as np


def conv_same_maxpooling_half(input_data, filter_w, filter_h, input_channel, output_channel):
    '''
    filtering은 크기가 유지되고 pooling으로 layer크기가 반으로 줄어드는 convolution layer

    :param input_data: input 데이터
    :param filter_w: 필터 window width
    :param filter_h: 필터 window height
    :param input_channel: 입력 channel(처음에는 이미지의 color 구성(RGB)수이고 이후에는 이전 단계 out값
    :param output_channel: 다음으로 넘겨줄 channel 수
    :return: layer 오브젝트
    '''
    with tf.name_scope('conv_1'):
        W_conv1 = tf.Variable(tf.truncated_normal([filter_w, filter_h, input_channel, output_channel], stddev=0.1)) #필터 (w, h, colorspace(input filter count), out_filter_count)
        b1 = tf.Variable(tf.truncated_normal([output_channel], stddev=0.1))
        h_conv1 = tf.nn.conv2d(input_data, W_conv1, strides=[1, 1, 1, 1], padding='SAME')
        h_conv1_relu = tf.nn.relu(tf.add(h_conv1, b1))
        h_conv1_maxpool = tf.nn.max_pool(h_conv1_relu
                                         , ksize=[1, 2, 2, 1]
                                         , strides=[1, 2, 2, 1], padding='SAME')

    return h_conv1_maxpool




def fclayer(layername, input, inputsize, outputsize, keep_prob):
    '''
    fully-connected layer 생성을 위한 helper 함수

    :param layername: tensorflow name scope 문자열
    :param input: 입력 layer 오브젝트
    :param inputsize: 입력 layer width
    :param outputsize: 출력 layer widht
    :param keep_prob: dropout ratio.
    :return: layer 오브젝트
    '''
    with tf.name_scope(layername):

        w = tf.Variable(tf.truncated_normal([inputsize, outputsize], stddev=0.1))
        b = tf.Variable(tf.truncated_normal([outputsize], stddev=0.1))
        h = tf.add(tf.matmul(input, w), b)  # h_fc1 = input_data*W_fc1 + b_fc1
        h_relu = tf.nn.relu(h)

        # dropout r_fc1
        r_dropout_fc1 = tf.nn.dropout(h_relu, keep_prob)

    return h_relu


# final layer
def final_out(input_data, input_size, labelCnt):
    with tf.name_scope('final_out'):
        W_fo = tf.Variable(tf.truncated_normal([input_size, labelCnt], stddev=0.1))
        b_fo = tf.Variable(tf.truncated_normal([labelCnt], stddev=0.1))
        h_fo = tf.add(tf.matmul(input_data, W_fo), b_fo)  # h_fc1 = input_data*W_fc1 + b_fc1

    # 최종 레이어에 softmax 함수는 적용하지 않았다.

    return h_fo


# build cnn_graph
def build_model_simple(images, keep_prob, labelCnt):
    # define CNN network graph
    # output shape will be (*,48,48,16)
    r_cnn1 = conv_same_maxpooling_half(images, 3, 3, 1, 32)  # convolutional layer 1
    print("shape after cnn1 ", r_cnn1.get_shape())

    # output shape will be (*,24,24,32)
    r_cnn2 = conv_same_maxpooling_half(r_cnn1, 3, 3, 32, 64)  # convolutional layer 2
    print("shape after cnn2 :", r_cnn2.get_shape())

    # 앞에서 입력받은 다차원 텐서를 fcc에 넣기 위해서 1차원으로 펴는 작업
    conv_out_reshape = tf.reshape(r_cnn2, [-1, 7 * 7 * 64])  # convolution layer 끝난 후 크기를 알아야 한다.

    # fully-connected layers :
    r_fc1 = fclayer('r_fc1', conv_out_reshape, 7 * 7 * 64, 256, keep_prob)
    print("shape after fc1 :", r_fc1.get_shape())

    # final layer
    r_out = final_out(r_fc1, 256, labelCnt=labelCnt)
    print("shape after final layer :", r_out.get_shape())

    return r_out