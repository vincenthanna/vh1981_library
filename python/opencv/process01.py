import sys
import os
import tensorflow as tf
import numpy as np
import matplotlib.pyplot as plt

#pyplot 한글 출력
import matplotlib.font_manager as fm

import matplotlib
print(matplotlib.matplotlib_fname() )

'''
pyplot에 기본 설정된 폰트가 한글을 지원하지 않아서 title의 한글 등이 깨지므로
한글을 지원하는 폰트 명으로 교체한다.
'''
font_name = "NanumGothic"
matplotlib.rc('font', family = font_name)


imageDirs = ["0", "1", "2", "3", "4"]

categoryCnt = len(imageDirs)

import matplotlib.image as mpimg
from PIL import Image


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
        trainCnt = totalCnt * 3 / 4

        cnt = 0

        for f in files:
            imageFilePath = os.path.join(imageDirPath, f)
            if cnt < trainCnt:
                train_imgpaths.append(imageFilePath)
                train_labels.append(label)

            else:
                test_imgpaths.append(imageFilePath)
                test_labels.append(label)

            cnt += 1

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
    이미지 변형 관련 오류가 있을 경우 FIFOQueue관련 오류가 나기 쉽다.
    tf.image를 다음과 같이 직접 reshape할 경우 오류를 발생시킨다.
    image = tf.reshape(image,[96, 96 ,3]) # !!!주의!!! 에러남!
    
    에러 형태 : 
        => FIFOQueue is closed and has insufficient elements
    '''

    image = tf.image.resize_images(image, [96, 96])

    # 랜덤하게 flip/밝기/컨트라스트 등을 적용해서 달라진 이미지가 되게 한다.
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
# exit()

# 모델 코드는 외부로 분리함.
from models.model_jodaehypb import build_model_jodaehyub


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
    model = build_model_jodaehyub(X, keep_prob=keep_prob, labelCnt=categoryCnt)

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

    # tf.summary.scalar('accuracy', accuracy)
    #
    # summary = tf.summary.merge_all()


    with tf.Session(config=tf.ConfigProto(allow_soft_placement=True, log_device_placement=True)) as session:
        # saver = tf.train.Saver()  # create saver to store training model into file
        # summary_writer = tf.summary.FileWriter("./", session.graph)

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


'''
run main program :
'''
if __name__ == '__main__':
    run()
