import sys
import os
import tensorflow as tf
import numpy as np
import matplotlib.pyplot as plt

#pyplot 한글 출력
import matplotlib.font_manager as fm

import matplotlib
print(matplotlib.matplotlib_fname() )

font_name = "NanumGothic"
matplotlib.rc('font', family = font_name)


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

# def test():
#     from tensorflow.examples.tutorials.mnist import input_data
#     mnist = input_data.read_data_sets('./mnist/data', one_hot=True)
#
#     xb, yb = mnist.train.next_batch(100)
#     print(xb[0].shape)
#     print(yb[0].shape, yb[0])
#
# test()



# X_train = np.empty((1, 96, 96, 3), float)
# t_train = np.empty((0, 1, 5))
#
# X_test = np.empty([])
# t_test = np.empty([])
X_train = np.empty((0,96,96,3))
t_train = np.empty((0, categoryCnt))

X_test = np.empty((0,96,96,3))
t_test = np.empty((0, categoryCnt))

# def resize_img(imageFilePath):
#     image = cv2.imread(imageFilePath)
#     cv2.resize(image, (96, 96))
#     cv2.imshow(imageFilePath, image)
#     cv2.waitKey(0)

def label_to_onehot(label, depth):
    onehot = np.zeros((depth), float)
    onehot[label] = 1
    return onehot

def onehot_to_label(onehot):
    return np.argmax(onehot)

def get_img(imageFilePath):
    image = Image.open(imageFilePath)
    image = image.resize((96,96))
    image = np.asarray(image)

    #print("image.shape=", image.shape)

    return image


def data_prepare(istrain):
    train_imgpaths = []
    train_labels = []
    test_imgpaths = []
    test_labels = []

    for index, imgdir in enumerate(imageDirs):
        imageDirPath = os.path.join(imgdir, "cropped")
        files = [f for f in os.listdir(imageDirPath)
                 if os.path.isfile(os.path.join(imageDirPath, f)) and f.find("jpeg") > 0]
        label = index

        totalCnt = len(files)
        #print("label ", label, " cnt=", totalCnt)

        trainCnt = totalCnt * 3 / 4

        cnt = 0

        for f in files:

            imageFilePath = os.path.join(imageDirPath, f)
            #image = get_img(imageFilePath)
            if cnt < trainCnt:
                train_imgpaths.append(imageFilePath)
                train_labels.append(label)

            else:
                test_imgpaths.append(imageFilePath)
                test_labels.append(label)

            cnt += 1


    # fig = plt.figure(figsize=(10, 10))
    # for i in range(100):
    #     fig.add_subplot(10, 10, i + 1)
    #     img = get_img(xtrain[i])
    #     plt.imshow(img)
    #     plt.title(imageDirs[ttrain[i]])
    #
    # plt.show()

    #return xtrain, ttrain, xtest, ttest

    images = []
    labels = []
    if istrain == True:
        images = train_imgpaths
        labels = train_labels
    else:
        images = test_imgpaths
        labels = test_labels

    print("data ", "images=", len(images), "labels=", len(labels))

    inputqueue = tf.train.slice_input_producer([images, labels], shuffle=True)


    return inputqueue


def read_data(item):
    #print("filepath=", item[0], "label=", item[1])
    filepath = item[0]
    label = item[1]
    image = tf.image.decode_jpeg(tf.read_file(filepath), channels=3)

    #image = tf.read_file(filepath) #  good
    #image = filepath # good
    return image, label, filepath

def read_data_batch(batch_size = 100, istrain = True):
    inputqueue = data_prepare(istrain)
    image, label, filepath = read_data(inputqueue)

    '''
    절대 이미지 resize에 아래 reshape쓰지마라. 에러난다...
    
    image = tf.reshape(image,[96, 96 ,3]) # !!!주의!!! 에러남!
    
    에러 형태 : 
        => FIFOQueue is closed and has insufficient elements
    '''

    image = tf.image.resize_images(image, [96, 96])

    # random image
    image = tf.image.random_flip_left_right(image)
    image = tf.image.random_brightness(image,max_delta=0.5)
    image = tf.image.random_contrast(image,lower=0.2,upper=2.0)
    image = tf.image.random_hue(image,max_delta=0.08)
    image = tf.image.random_saturation(image,lower=0.2,upper=2.0)

    batch_image, batch_label, batch_filepath = tf.train.batch([image, label, filepath], batch_size=batch_size)
    batch_filepath = tf.reshape(batch_filepath, [batch_size, 1])

    batch_label_onehot = tf.one_hot(tf.to_int64(batch_label), categoryCnt, on_value=1.0 ,off_value=0.0)
    return batch_image, batch_label_onehot, batch_filepath



def testdata():
    image_batch, label_batch, filepath_batch = read_data_batch(istrain=False)
    with tf.Session() as session:

        print("image_batch=", image_batch.shape, "label_batch=", label_batch.shape)

        init_op = tf.global_variables_initializer() # use this for tensorflow 0.12rc0
        init_op2 = tf.local_variables_initializer()
        coord = tf.train.Coordinator()
        threads = tf.train.start_queue_runners(sess=session, coord=coord)
        session.run(init_op)
        session.run(init_op2)
        imgbatch_, labelbatch_, filenames_= session.run([image_batch, label_batch, filepath_batch])

        print("imgbatch_=", imgbatch_.shape, "labelbatch_=", labelbatch_.shape)

        # for i in range(100):
        #     print(imgbatch_[i], labelbatch_[i])


        coord.request_stop()
        coord.join(threads)

        print('test tf.session() run finish')

testdata()


print("hello world")



exit()





def load_data():
    xtrain = []
    ttrain = []
    xtest = []
    ttest = []

    #fig = plt.figure(figsize=(10, 10))

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

            # if (cnt < 100):
            #     fig.add_subplot(10, 10, cnt + 1)
            #     plt.imshow(image)

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

    p = np.random.permutation(len(X_train))
    X_train = X_train[p]
    t_train = t_train[p]

    fig = plt.figure(figsize=(10, 10))
    for i in range(100):
        fig.add_subplot(10, 10, i + 1)
        plt.imshow(X_train[i])
        plt.title(imageDirs[onehot_to_label(t_train[i])])


    print("t_train[0]", t_train[0].shape, t_train[0])

    plt.show()



def build_layer(prev, ksize, pdepth, ndepth):
    W = tf.Variable(tf.random_normal([ksize, ksize, pdepth, ndepth], stddev=0.01))
    L = tf.nn.conv2d(prev, W, strides=[1, 1, 1, 1], padding='SAME')
    L = tf.nn.relu(L)
    return L

def build_model_2():
    global keep_prob

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

def build_fclayer(prevLayer, inputsize, outputsize):
    global keep_prob
    W = tf.Variable(tf.random_normal([inputsize, outputsize], stddev=0.01))
    L = tf.reshape(prevLayer, [-1, inputsize])
    L = tf.matmul(L, W)
    L = tf.nn.relu(L)
    L = tf.nn.dropout(L, keep_prob)
    return L

def build_model():
    global keep_prob

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
    # W3 = tf.Variable(tf.random_normal([16 * 16 * 64, 256], stddev=0.01))
    # L3 = tf.reshape(L2, [-1, 16 * 16 * 64])
    # L3 = tf.matmul(L3, W3)
    # L3 = tf.nn.relu(L3)
    # L3 = tf.nn.dropout(L3, keep_prob)
    L3 = build_fclayer(L2, 16 * 16 * 64, 256)
    L4 = build_fclayer(L3, 256, 128)
    L5 = build_fclayer(L4, 128, 64)
    L6 = build_fclayer(L5, 64, 32)

    # W4 = tf.Variable(tf.random_normal([256, 128], stddev=0.01))
    # L4 = tf.reshape(L3, [-1, 256])
    # L4 = tf.matmul(L4, W4)
    # L4 = tf.nn.relu(L4)
    # L4 = tf.nn.dropout(L4, keep_prob)

    lastW = tf.Variable(tf.random_normal([32, categoryCnt], stddev=0.01))
    model = tf.matmul(L6, lastW)

    return model


# convolutional network layer 1
def conv1(input_data):
    # layer 1 (convolutional layer)
    # FLAGS.conv1_filter_size = 3
    # FLAGS.conv1_layer_size = 16
    # FLAGS.stride1 = 1

    with tf.name_scope('conv_1'):
        W_conv1 = tf.Variable(tf.truncated_normal([3, 3, 3, 16], stddev=0.1))
        b1 = tf.Variable(tf.truncated_normal([16], stddev=0.1))
        h_conv1 = tf.nn.conv2d(input_data, W_conv1, strides=[1, 1, 1, 1], padding='SAME')
        h_conv1_relu = tf.nn.relu(tf.add(h_conv1, b1))
        h_conv1_maxpool = tf.nn.max_pool(h_conv1_relu
                                         , ksize=[1, 2, 2, 1]
                                         , strides=[1, 2, 2, 1], padding='SAME')

    return h_conv1_maxpool


# convolutional network layer 2
def conv2(input_data):
    # FLAGS.conv2_filter_size = 3
    # FLAGS.conv2_layer_size = 32
    # FLAGS.stride2 = 1

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
    # FLAGS.conv3_filter_size = 3
    # FLAGS.conv3_layer_size = 64
    # FLAGS.stride3 = 1

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
    # FLAGS.conv4_filter_size = 5
    # FLAGS.conv4_layer_size = 128
    # FLAGS.stride4 = 1

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
    # input_layer_size = 6 * 6 * 128
    # FLAGS.fc1_layer_size = 512

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
    # FLAGS.fc2_layer_size = 256

    with tf.name_scope('fc_2'):
        W_fc2 = tf.Variable(tf.truncated_normal([512, 256], stddev=0.1))
        b_fc2 = tf.Variable(tf.truncated_normal([256], stddev=0.1))
        h_fc2 = tf.add(tf.matmul(input_data, W_fc2), b_fc2)  # h_fc1 = input_data*W_fc1 + b_fc1
        h_fc2_relu = tf.nn.relu(h_fc2)

    return h_fc2_relu


# final layer
def final_out(input_data):
    global categoryCnt
    with tf.name_scope('final_out'):
        W_fo = tf.Variable(tf.truncated_normal([256, categoryCnt], stddev=0.1))
        b_fo = tf.Variable(tf.truncated_normal([categoryCnt], stddev=0.1))
        h_fo = tf.add(tf.matmul(input_data, W_fo), b_fo)  # h_fc1 = input_data*W_fc1 + b_fc1

    # 최종 레이어에 softmax 함수는 적용하지 않았다.

    return h_fo


# build cnn_graph
def build_model_3(images, keep_prob):
    # define CNN network graph
    # output shape will be (*,48,48,16)
    r_cnn1 = conv1(images)  # convolutional layer 1
    print("shape after cnn1 ", r_cnn1.get_shape())

    # output shape will be (*,24,24,32)
    r_cnn2 = conv2(r_cnn1)  # convolutional layer 2
    print("shape after cnn2 :", r_cnn2.get_shape())

    # output shape will be (*,12,12,64)
    r_cnn3 = conv3(r_cnn2)  # convolutional layer 3
    print("shape after cnn3 :", r_cnn3.get_shape())

    # output shape will be (*,6,6,128)
    r_cnn4 = conv4(r_cnn3)  # convolutional layer 4
    print("shape after cnn4 :", r_cnn4.get_shape())

    # fully connected layer 1
    r_fc1 = fc1(r_cnn4)
    print("shape after fc1 :", r_fc1.get_shape())

    # fully connected layer2
    r_fc2 = fc2(r_fc1)
    print("shape after fc2 :", r_fc2.get_shape())

    ## drop out
    # 참고 http://stackoverflow.com/questions/34597316/why-input-is-scaled-in-tf-nn-dropout-in-tensorflow
    # 트레이닝시에는 keep_prob < 1.0 , Test 시에는 1.0으로 한다.
    r_dropout = tf.nn.dropout(r_fc2, keep_prob)
    print("shape after dropout :", r_dropout.get_shape())

    # final layer
    r_out = final_out(r_dropout)
    print("shape after final layer :", r_out.get_shape())

    return r_out


def run():
    image_batch, label_batch, filepath_batch = read_data_batch()

    X = tf.placeholder(tf.float32, [None, 96, 96, 3])
    Y = tf.placeholder(tf.float32, [None, categoryCnt])
    keep_prob = tf.placeholder(tf.float32)

    #model = build_model()
    model = build_model_3(X, keep_prob=keep_prob)

    # 'cost' or 'loss'
    cost = tf.reduce_mean(tf.nn.softmax_cross_entropy_with_logits(logits=model, labels=Y))

    #tf.summary.scalar('loss',cost)

    #define optimizer
    optimizer = tf.train.AdamOptimizer(0.0001)
    train = optimizer.minimize(cost)

    print("X_train.shape : ", X_train.shape)
    print("t_train.shape : ", t_train.shape)

    # for validation:
    val_image_batch, val_label_batch, val_filepath_batch = read_data_batch(istrain=False)



    with tf.Session(config=tf.ConfigProto(allow_soft_placement=True, log_device_placement=True)) as session:
        init = tf.global_variables_initializer()
        coord = tf.train.Coordinator()
        threads = tf.train.start_queue_runners(sess=session, coord=coord)
        session.run(init)

        for i in range(10000):
            pass


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
    print("정확도:", session.run(accuracy, feed_dict={X:X_test, Y:t_test, keep_prob: 1}))





#run()
