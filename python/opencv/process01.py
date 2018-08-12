import sys
import os
import tensorflow as tf
import numpy as np
import matplotlib.pyplot as plt

imageDirs = ["김연아", "박지성", "장동건", "전지현", "정우성"]


from tensorflow.examples.tutorials.mnist import input_data
mnist = input_data.read_data_sets('./mnist/data', one_hot=True)

xb, yb = mnist.train.next_batch(100)
print(xb.shape)
print(yb.shape)

import cv2
import matplotlib.image as mpimg
categoryCnt = len(imageDirs)

X_train = []
t_train = []

X_test = []
t_test = []

X = tf.placeholder(tf.float32, [None, 96, 96, 3])
Y = tf.placeholder(tf.float32, [None, categoryCnt])
keep_prob = tf.placeholder(tf.float32)

def get_img(imageFilePath):
    #imageFilePath = os.path.join(imageDirPath, f)
    image = tf.image.decode_jpeg(tf.read_file(imageFilePath), channels=3)
    # print(imageFilePath)
    image = tf.reshape(image, [96, 96, 3])
    print("shape1=", image.shape)

    #cv2.imshow(image)
    image = image.astype(float)

    image = tf.reshape(image, [96*96, 3])
    print("shape2=", image.shape)

    return image

def load_data():
    dataIndex = 0
    for index, imgdir in enumerate(imageDirs):

        print(index, imgdir)
        imageDirPath = os.path.join(imgdir, "cropped")
        files = [f for f in os.listdir(imageDirPath) if os.path.isfile(os.path.join(imageDirPath, f))]
        label = index

        totalCnt = len(files)
        print("label ", label, " cnt=", totalCnt)

        trainCnt = totalCnt * 3 / 4

        cnt = 0
        for f in files:
            imageFilePath = os.path.join(imageDirPath, f)
            image = get_img(imageFilePath)

            if cnt < trainCnt:
                X_train.append(image)
                t_train.append(label)
            else:
                X_test.append(image)
                t_test.append(label)




def build_model():

    W1 = tf.Variable(tf.random_normal([3, 3, 3, 32], stddev=0.01))
    L1 = tf.nn.conv2d(X, W1, strides=[1,1,1,1], padding='SAME')
    L1 = tf.nn.relu(L1)
    L1 = tf.nn.max_pool(L1, ksize=[1,2,2,1], strides=[1, 2, 2, 1], padding='SAME')

    W2 = tf.Variable(tf.random_normal([3, 3, 32, 64], stddev=0.01))
    L2 = tf.nn.conv2d(L1, W2, strides=[1,1,1,1], padding='SAME')
    L2 = tf.nn.relu(L2)
    L2 = tf.nn.max_pool(L2, ksize=[1,2,2,1], strides=[1,2,2,1], padding='SAME')

    W3 = tf.Variable(tf.random_normal([7 * 7 * 64, 256], stddev=0.01))
    L3 = tf.reshape(L2, [-1, 7 * 7 * 64])
    L3 = tf.matmul(L3, W3)
    L3 = tf.nn.relu(L3)
    L3 = tf.nn.dropout(L3, keep_prob)

    W4 = tf.Variable(tf.random_normal([256, categoryCnt], stddev=0.01))
    model = tf.matmul(L3, W4)

    return model


def run():
    load_data()

    print(t_train)
    #t_train2 = tf.one_hot(t_train, 5)
    t_train2 = tf.one_hot(t_train, 5)
    model = build_model()
    cost = tf.reduce_mean(tf.nn.softmax_cross_entropy_with_logits(logits=model, labels=Y))
    optimizer = tf.train.AdamOptimizer(0.001).minimize(cost)

    # 신경망 모델 학습
    init = tf.global_variables_initializer()
    session = tf.Session()
    session.run(init)

    batch_size = 100
    total_batch = int(len(X_train) / batch_size)
    print("total_batch=", total_batch)

    ###################################
    total_cost = 0

    _, cost_val = session.run([optimizer, cost], feed_dict={X: X_train, Y: t_train, keep_prob:0.7})

    total_cost += cost_val

    print("epoch:", epoch, " avg cost=", total_cost/len(X_train))

    ###################################

    print("optimization completed")

    is_correct = tf.equal(tf.argmax(model, 1), tf.argmax(Y, 1))
    accuracy = tf.reduce_mean(tf.cast(is_correct, tf.float32))
    print("정확도:", session.run(accuracy, feed_dict={X:X_train, Y:t_train, keep_prob: 1}))





run()
