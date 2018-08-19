import sys
import os
import tensorflow as tf
import numpy as np
import matplotlib.pyplot as plt

imageDirs = ["김연아", "박지성", "장동건", "전지현", "정우성"]


##############################################
# FIXME: mnist data check code... delete later
'''
from tensorflow.examples.tutorials.mnist import input_data
mnist = input_data.read_data_sets('./mnist/data', one_hot=True)

xb, yb = mnist.train.next_batch(100)
print(xb.shape)
print(yb.shape)

'''
##############################################


import cv2
import matplotlib.image as mpimg
from PIL import Image
categoryCnt = len(imageDirs)


# X_train = np.empty((1, 96, 96, 3), float)
# t_train = np.empty((0, 1, 5))
#
# X_test = np.empty([])
# t_test = np.empty([])
X_train = np.empty((0,96,96,3))
t_train = np.empty((0, categoryCnt))

X_test = np.empty((0,96,96,3))
t_test = np.empty((0, categoryCnt))

X = tf.placeholder(tf.float32, [None, 96, 96, 3])
Y = tf.placeholder(tf.float32, [None, categoryCnt])
keep_prob = tf.placeholder(tf.float32)

# def resize_img(imageFilePath):
#     image = cv2.imread(imageFilePath)
#     cv2.resize(image, (96, 96))
#     cv2.imshow(imageFilePath, image)
#     cv2.waitKey(0)

def label_to_onehot(label, depth):
    onehot = np.zeros((depth), float)
    onehot[label] = 1
    return onehot

def get_img(imageFilePath):
    image = Image.open(imageFilePath)
    image = image.resize((96,96))
    image = np.asarray(image)

    #print("image.shape=", image.shape)

    return image

def load_data():
    xtrain = []
    ttrain = []
    xtest = []
    ttest = []
    for index, imgdir in enumerate(imageDirs):

        print(index, imgdir)
        imageDirPath = os.path.join(imgdir, "cropped")
        files = [f for f in os.listdir(imageDirPath)
                 if os.path.isfile(os.path.join(imageDirPath, f)) and f.find("jpeg") > 0]
        label = index

        totalCnt = len(files)
        print("label ", label, " cnt=", totalCnt)

        trainCnt = totalCnt * 3 / 4

        cnt = 0


        for f in files:
            imageFilePath = os.path.join(imageDirPath, f)
            image = get_img(imageFilePath)

            #label을 one-hot으로 변경
            onehot = label_to_onehot(label, depth=categoryCnt)
            image = np.reshape(image, [96, 96, 3])

            if cnt < trainCnt:
                xtrain.append(image)
                ttrain.append(onehot)

            else:
                xtest.append(image)
                ttest.append(onehot)

            cnt += 1

            # print("X_train, X_test, t_train ,t_test ", X_train.shape, X_test.shape,
            #       t_train.shape, t_test.shape)

    print("total xtrain = ", len(xtrain))
    global X_train
    global t_train
    global X_test
    global t_test

    X_train = np.asarray(xtrain)
    t_train = np.asarray(ttrain)
    X_test = np.asarray(xtest)
    t_test = np.asarray(ttest)
    print("load_data() :")
    print("        X_train : ", X_train.shape)
    print("        t_train : ", t_train.shape)
    print("        X_test  : ", X_test.shape)
    print("        t_test  : ", t_test.shape)

def build_layer(prev, ksize, pdepth, ndepth):
    W = tf.Variable(tf.random_normal([ksize, ksize, pdepth, ndepth], stddev=0.01))
    L = tf.nn.conv2d(prev, W, strides=[1, 1, 1, 1], padding='SAME')
    L = tf.nn.relu(L)
    return L

def build_model_2():
    L1 = build_layer(X, 7, 3, 32)
    L2 = build_layer(L1, 6, 32, 64)

    # 96 X 96 X 64 => 48 X 48 X 64
    L3 = tf.nn.max_pool(L2, ksize=[1, 2, 2, 1], strides=[1, 2, 2, 1], padding='SAME')

    L4 = build_layer(L3, 5, 64, 128)
    L5 = build_layer(L4, 4, 128, 256)

    # 48 X 48 X 256 => 24 X 24 X 256
    L6 = tf.nn.max_pool(L5, ksize=[1, 2, 2, 1], strides=[1, 2, 2, 1], padding='SAME')

    L7 = build_layer(L6, 3, 256, 512)
    L8 = build_layer(L7, 2, 512, 1024)

    # 24 => 12
    L9 = tf.nn.max_pool(L8, ksize=[1, 2, 2, 1], strides=[1, 2, 2, 1], padding='SAME')
    L10 = build_layer(L9, 1, 1024, 2048)

    # 12 => 6
    L11 = tf.nn.max_pool(L10, ksize=[1, 2, 2, 1], strides=[1, 2, 2, 1], padding='SAME')

    # fully connected layer
    W12 = tf.Variable(tf.random_normal([6 * 6 * 2048, 8192], stddev=0.01))
    L12 = tf.reshape(L11, [-1, 6 * 6 * 2048])
    L12 = tf.matmul(L12, W12)
    L12 = tf.nn.relu(L12)
    L12 = tf.nn.dropout(L12, keep_prob)

    W13 = tf.Variable(tf.random_normal([8192, 2048], stddev=0.01))
    L13 = tf.reshape(L12, [-1, 8192])
    L13 = tf.matmul(L13, W13)
    L13 = tf.nn.relu(L13)
    L13 = tf.nn.dropout(L13, keep_prob)

    W14 = tf.Variable(tf.random_normal([2048, 512], stddev=0.01))
    L14 = tf.reshape(L13, [-1, 2048])
    L14 = tf.matmul(L14, W14)
    L14 = tf.nn.relu(L14)
    L14 = tf.nn.dropout(L14, keep_prob)

    W15 = tf.Variable(tf.random_normal([512, 256], stddev=0.01))
    L15 = tf.reshape(L14, [-1, 512])
    L15 = tf.matmul(L15, W15)
    L15 = tf.nn.relu(L15)
    L15 = tf.nn.dropout(L15, keep_prob)

    W16 = tf.Variable(tf.random_normal([256, categoryCnt], stddev=0.01))
    model = tf.matmul(L15, W16)

    return model



def build_model():

    '''
    1st layer
    input : 96 X 96 X 3
    '''
    W1 = tf.Variable(tf.random_normal([5, 5, 3, 32], stddev=0.01))
    L1 = tf.nn.conv2d(X, W1, strides=[1, 1, 1, 1], padding='SAME')
    L1 = tf.nn.relu(L1)

    '''
    1st pooling layer
    input : 96 X 96 X 32
    output : 48 X 48 X 32
    '''
    L1 = tf.nn.max_pool(L1, ksize=[1, 2, 2, 1], strides=[1, 2, 2, 1], padding='SAME')

    '''
    2nd layer
    input : 48 X 48 X 32
    output : 48 X 48 X 64
    '''
    W2 = tf.Variable(tf.random_normal([3, 3, 32, 64], stddev=0.01))
    L2 = tf.nn.conv2d(L1, W2, strides=[1, 1, 1, 1], padding='SAME')
    L2 = tf.nn.relu(L2)

    '''
    2nd pooling layer
    input : 48 X 48 X 64
    output : 16 X 16 X 64
    '''
    L2 = tf.nn.max_pool(L2, ksize=[1,3,3,1], strides=[1,3,3,1], padding='SAME')

    # fully connected layer
    W3 = tf.Variable(tf.random_normal([16 * 16 * 64, 256], stddev=0.01))
    L3 = tf.reshape(L2, [-1, 16 * 16 * 64])
    L3 = tf.matmul(L3, W3)
    L3 = tf.nn.relu(L3)
    L3 = tf.nn.dropout(L3, keep_prob)

    W4 = tf.Variable(tf.random_normal([256, categoryCnt], stddev=0.01))
    model = tf.matmul(L3, W4)

    return model


def run():
    load_data()

    global X_train
    global t_train
    global X_test
    global t_test

    global X
    global Y

    model = build_model_2()

    cost = tf.reduce_mean(tf.nn.softmax_cross_entropy_with_logits(logits=model, labels=Y))
    optimizer = tf.train.AdamOptimizer(0.001).minimize(cost)

    print(X_train.shape)
    print(t_train.shape)

    # 신경망 모델 학습
    init = tf.global_variables_initializer()
    session = tf.Session()
    session.run(init)

    batch_size = 100
    total_batch = int(X_train.shape[0] / batch_size)
    print("total_batch=", total_batch)

    ###################################
    total_cost = 0


    _, cost_val = session.run([optimizer, cost], feed_dict={X: X_train, Y: t_train, keep_prob:0.7})

    total_cost += cost_val

    print("avg cost=", total_cost/len(X_train))

    ###################################

    print("optimization completed")

    is_correct = tf.equal(tf.argmax(model, 1), tf.argmax(Y, 1))
    accuracy = tf.reduce_mean(tf.cast(is_correct, tf.float32))
    print("정확도:", session.run(accuracy, feed_dict={X:X_train, Y:t_train, keep_prob: 1}))





run()
