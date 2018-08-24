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


imageDirs = ["0", "1", "2", "3", "4"]


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


def prepare_data():
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

    # images = []
    # labels = []
    # if istrain == True:
    #     images = train_imgpaths
    #     labels = train_labels
    # else:
    #     images = test_imgpaths
    #     labels = test_labels

    #print("data ", "images=", len(images), "labels=", len(labels))

    #inputqueue = tf.train.slice_input_producer([images, labels], shuffle=True)


    #return inputqueue

    return train_imgpaths, train_labels, test_imgpaths, test_labels


def read_data(item):
    #print("filepath=", item[0], "label=", item[1])
    filepath = item[0]
    label = item[1]
    image = tf.image.decode_jpeg(tf.read_file(filepath), channels=3)
    return image, label, filepath

def read_data_batch(images, labels, batch_size = 100):
    inputqueue = tf.train.slice_input_producer([images, labels], shuffle=True)
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
    image = tf.image.random_brightness(image,max_delta=0.1)
    image = tf.image.random_contrast(image,lower=0.2,upper=1.0)
    image = tf.image.random_hue(image,max_delta=0.08)
    image = tf.image.random_saturation(image,lower=0.2,upper=1.0)

    batch_image, batch_label, batch_filepath = tf.train.batch([image, label, filepath], batch_size=batch_size)
    batch_filepath = tf.reshape(batch_filepath, [batch_size, 1])

    batch_label_onehot = tf.one_hot(tf.to_int64(batch_label), categoryCnt, on_value=1.0 ,off_value=0.0)
    return batch_image, batch_label_onehot, batch_filepath


def testdata():
    trainImgPaths, trainLabels, testImgPaths, testLabels = prepare_data()

    image_batch, label_batch, filepath_batch = read_data_batch(trainImgPaths, trainLabels, batch_size=100)
    vimage_batch, vlabel_batch, vfilepath_batch = read_data_batch(testImgPaths, testLabels, batch_size=100)
    with tf.Session() as session:

        print("image_batch=", image_batch.shape, "label_batch=", label_batch.shape)

        init_op = tf.global_variables_initializer() # use this for tensorflow 0.12rc0
        init_op2 = tf.local_variables_initializer()
        coord = tf.train.Coordinator()
        threads = tf.train.start_queue_runners(sess=session, coord=coord)
        session.run(init_op)
        session.run(init_op2)
        imgbatch_, labelbatch_, filenames_= session.run([image_batch, label_batch, filepath_batch])
        vimgbatch_, vlabelbatch_, vfilenames_ = session.run([vimage_batch, vlabel_batch, vfilepath_batch])

        print("imgbatch_=", imgbatch_.shape, "labelbatch_=", labelbatch_.shape)
        print("vimgbatch_=", vimgbatch_.shape, "vlabelbatch_=", vlabelbatch_.shape)

        # for i in range(100):
        #     print(imgbatch_[i], labelbatch_[i])


        coord.request_stop()
        coord.join(threads)

        print('test tf.session() run finish')

# testdata()


# print("hello world")



# exit()



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
    # prepare data:
    trainImgPaths, trainLabels, testImgPaths, testLabels = prepare_data()
    image_batch, label_batch, filepath_batch = read_data_batch(trainImgPaths, trainLabels, batch_size=100)
    vimage_batch, vlabel_batch, vfilepath_batch = read_data_batch(testImgPaths, testLabels, batch_size=100)

    X = tf.placeholder(tf.float32, [None, 96, 96, 3])
    T = tf.placeholder(tf.float32, [None, categoryCnt])
    keep_prob = tf.placeholder(tf.float32)

    # select model :
    #model = build_model()
    model = build_model_3(X, keep_prob=keep_prob)

    # 'cost' or 'loss'
    cost = tf.reduce_mean(tf.nn.softmax_cross_entropy_with_logits(logits=model, labels=T))

    #tf.summary.scalar('loss',cost)

    #define optimizer
    optimizer = tf.train.AdamOptimizer(0.0001)
    train = optimizer.minimize(cost)

    # print("X_train.shape : ", X_train.shape)
    # print("t_train.shape : ", t_train.shape)

    T_max = tf.argmax(T, 1)
    prediction_max = tf.argmax(model, 1)
    correct_pred = tf.equal(tf.argmax(model, 1), tf.argmax(T, 1))
    accuracy = tf.reduce_mean(tf.cast(correct_pred, tf.float32))

    tf.summary.scalar('accuracy', accuracy)

    summary = tf.summary.merge_all()


    with tf.Session(config=tf.ConfigProto(allow_soft_placement=True, log_device_placement=True)) as session:
        saver = tf.train.Saver()  # create saver to store training model into file
        summary_writer = tf.summary.FileWriter("./", session.graph)

        initializer = tf.global_variables_initializer()
        coord = tf.train.Coordinator()
        threads = tf.train.start_queue_runners(sess=session, coord=coord)
        session.run(initializer)

        for i in range(10000):
            images_, labels_ = session.run([image_batch, label_batch])
            session.run(train, feed_dict={X:images_, T:labels_, keep_prob:0.7})

            if i % 10 == 0:
                print("## steps ", i)

                rt = session.run([T_max, prediction_max, cost, accuracy],
                                 feed_dict={X:images_, T:labels_, keep_prob:1.0})
                print("Prediction loss:", rt[2], ' accuracy:', rt[3])

                #validation steps
                vimages_, vlabels_ = session.run([vimage_batch, vlabel_batch])
                rv = session.run([T_max, prediction_max, cost, accuracy],
                                 feed_dict={X:vimages_, T:vlabels_, keep_prob:1.0})

                print("Validation loss:", rv[2], 'accuracy:', rv[3])

                if rv[3] > 0.9:
                    break

                #validation accuracy
        coord.request_stop()
        coord.join(threads)
        print("finish!")

    #
    #
    #
    #
    #
    #
    #
    # # 신경망 모델 학습
    # init = tf.global_variables_initializer()
    # session = tf.Session()
    # session.run(init)
    #
    # batch_size = 100
    # total_batch = int(X_train.shape[0] / batch_size)
    # print("total_batch=", total_batch)
    #
    # ###################################
    # total_cost = 0
    #
    # _, cost_val = session.run([optimizer, cost], feed_dict={X: X_train, Y: t_train, keep_prob:0.7})
    #
    # total_cost += cost_val
    #
    # print("avg cost=", total_cost/len(X_train))
    #
    # ###################################
    #
    # print("optimization completed")
    #
    # is_correct = tf.equal(tf.argmax(model, 1), tf.argmax(Y, 1))
    # accuracy = tf.reduce_mean(tf.cast(is_correct, tf.float32))
    # print("정확도:", session.run(accuracy, feed_dict={X:X_test, Y:t_test, keep_prob: 1}))





run()
