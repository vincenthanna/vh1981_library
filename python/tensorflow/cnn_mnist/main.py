#!/usr/bin/python3
"""Simple CNN example for MNIST dataset.

http://yann.lecun.com/exdb/mnist/ 에서 직접 파일을 다운로드받아서 데이터를 준비한다.
모델에 가공되지 않은 데이터를 넣어야 하는 경우 예제 코드.
"""

import tensorflow as tf
import struct
import numpy as np
import matplotlib.pyplot as plt
import math

#models :
from models.model_simple import build_model_simple

def load_mnists():

    # mnist file names :
    '''
    http://yann.lecun.com/exdb/mnist/ 에서 파일을 다운로드받고 압축을 푼 파일을 사용한다.
    아래 파일명과 비교해서 이름이 다르면 수정해 줄 것.
    '''
    trainSet_label_filename = "train-labels.idx1-ubyte"
    trainSet_image_filename = "train-images.idx3-ubyte"
    testSet_label_filename = "t10k-labels.idx1-ubyte"
    testSet_image_filename = "t10k-images.idx3-ubyte"

    trainSet_labels, trainSet_images = load_mnist_set(trainSet_label_filename, trainSet_image_filename)
    testSet_labels, testSet_images = load_mnist_set(testSet_label_filename, testSet_image_filename)
    return trainSet_images, trainSet_labels, testSet_images, testSet_labels


def one_hot(value, classes):
    '''
    label 값(숫자)를 one-hot numpy 벡터로 변경한다.

    :param value: label 값(0기준)
    :param classes: 식별자 수(0기준)
    :return: one-hot 배열
    '''
    return np.eye(classes)[value]


def load_mnist_set(labelfilename, imagefilename):
    labels = []
    images = []
    num_items = 0
    row = 0
    col = 0
    '''
    label 파일 포맷은 아래와 같다.
    
    [offset] [type]          [value]          [description] 
    0000     32 bit integer  0x00000801(2049) magic number (MSB first) 
    0004     32 bit integer  60000            number of items 
    0008     unsigned byte   ??               label 
    0009     unsigned byte   ??               label 
    ........ 
    xxxx     unsigned byte   ??               label
    
    The labels values are 0 to 9.
    
    처음에서 8바이트 이후 byte단위로 label(0~9) 값이 존재함.
    '''
    with open(labelfilename, "rb") as f:
        # 아래 링크 참조
        # https://stackoverflow.com/questions/3783677/how-to-read-integers-from-a-file-that-are-24bit-and-little-endian-using-python
        magic_number = struct.unpack('>i', f.read(4))  # 꺽쇠방향은 big/little endian 구분용 < : little, > : big
        magic_number = magic_number[0]
        num_items = struct.unpack('>i', f.read(4))[0]

        for i in range(num_items):
            label = struct.unpack('b', f.read(1))
            label_one_hot = one_hot(label, 10) # one-hot encoding으로 변경
            labels.append(label_one_hot)

        print("magic_number : ", format(magic_number, '08x'))
        print("items : ", num_items)

    '''
    이미지 파일 포맷은 다음과 같다 :
     
    [offset] [type]          [value]          [description] 
    0000     32 bit integer  0x00000803(2051) magic number 
    0004     32 bit integer  60000            number of images 
    0008     32 bit integer  28               number of rows 
    0012     32 bit integer  28               number of columns 
    0016     unsigned byte   ??               pixel 
    0017     unsigned byte   ??               pixel 
    ........ 
    xxxx     unsigned byte   ??               pixel
    
    16바이트 이후에 이미지 데이터가 시작됨.
    이미지 크기는 row*col 로 계산하면 되는데 (28,28)크기임.
    '''
    with open(imagefilename, "rb") as f:
        '''
        image 파일의 경우 magic_number, image count, row, column 값이 4바이트 크기(uint32)로 저장되어 있다.
        16번째 바이트부터는 이미지 데이터임.
        '''
        magic_number = struct.unpack('>i', f.read(4))[0]  # 꺽쇠방향은 big/little endian 구분용 < : little, > : big
        num_items = struct.unpack('>i', f.read(4))[0]
        row = struct.unpack('>i', f.read(4))[0]
        col = struct.unpack('>i', f.read(4))[0]
        print("magic:", magic_number, " items:", num_items, " (", row, ",", col, ")")

        for i in range(num_items):
            image = bytearray(f.read(row * col))
            images.append(image)

        print("images len : ", len(images))

    '''return labels, images as numpy array
    
    label 데이터는 (?, 10) 포맷, 이미지는(?, 784) 포맷임. 
    '''
    #return np.array(labels).astype(np.float32), np.array(images).reshape((num_items, col * row)).astype(np.float32)
    return np.array(labels).astype(np.float32), np.array(images).astype(np.float32)


def sample_data_check(imgs, labels):
    '''
    데이터가 제대로 읽혀졌는지 테스트하는 함수
    :param imgs: [num, 28*28] 형태의 데이터
    :param labels: 이미지 label(one-hot)
    :return: none.
    '''
    print("sample_data_check:", imgs.shape, labels.shape)
    num_imgs = imgs.shape[0]
    imgs = imgs.reshape(-1, 28, 28)
    print("sample_data_check:", imgs.shape, labels.shape)
    cols = rows = int(math.sqrt(num_imgs))
    for i in range(0, rows * cols):
        plt.subplot(rows, cols, i + 1)
        plt.xlabel("")
        plt.title(np.argmax(labels[i]))
        plt.imshow(imgs[i])

    plt.show()


def get_batch(data, batch_size, idx):
    '''
    data에서 batch_size * idx ~ batch_size * (idx+1)만큼 잘라서 리턴
    :param data: 전체 데이터 크기
    :param batch_size:batch 크기
    :param idx:batch 데이터 인덱스(몇번째?)
    :return: idx순서의 batch_size의 slice된 데이터
    '''
    ret = data[batch_size*idx:batch_size*(idx+1),]
    return ret


def run():
    """
    실행 함수
    :return: void
    """

    # 데이터 준비
    trainX, trainY, testX, testY, = load_mnists()
    print("read data : ")
    print("trainX : ", trainX.shape)
    print("trainY : ", trainY.shape)
    print("testX  : ", testX.shape)
    print("testY  : ", testY.shape)

    # 데이터가 제대로 읽혀졌는지 테스트하는 코드.
    # check_xs = get_batch(trainX, 20, 0)
    # check_ys = get_batch(trainY, 20, 0)
    # sample_data_check(check_xs, check_ys)

    num_train = trainX.shape[0] #training set 데이터 수.
    num_test = testX.shape[0] #test set 데이터 수.
    classCnt = testY.shape[1] #식별자 수. MNIST의 경우 0~9 의 10개.

    print("num_train : ", num_train, " num_test : ", num_test, "   classCnt : ", classCnt)

    X = tf.placeholder(tf.float32, [None, 28, 28, 1])
    Y = tf.placeholder(tf.float32, [None, classCnt])
    keep_prob = tf.placeholder(tf.float32)

    # select model : FIXME:
    model = build_model_simple(X, keep_prob=keep_prob, labelCnt=classCnt)

    # cost function
    '''
    softmax_cross_entropy_with_logits : computes softmax cross entropy between 'logits' and 'labels'
    결과값을 softmax 변환하고 cross entropy loss를 계산
    cost는 각 class의 loss의 평균으로 계산(reduce_mean())
    '''
    cost = tf.reduce_mean(tf.nn.softmax_cross_entropy_with_logits(logits=model, labels=Y))

    # optimizer
    '''
    minimize()호출은 gradient를 계산하고 그것을 변수(weight,bias)에 적용하는 것을 동시에 처리한다.
    작업을 따로 하고 싶으면 compute_gradient()/apply_gradient()를 사용.
    '''
    optimizer = tf.train.AdamOptimizer(0.001).minimize(cost) #AdamOptimizer() 인자는 Learning Rate

    batch_size = 100
    total_batch = int(num_train / batch_size)

    num_epochs = 10
    with tf.Session() as session:

        initializer = tf.global_variables_initializer()
        session.run(initializer)

        for epoch in range(num_epochs):
            total_cost = 0

            for i in range(total_batch):
                batch_xs = get_batch(trainX, batch_size, i)
                batch_ys = get_batch(trainY, batch_size, i)
                batch_xs = batch_xs.reshape(-1, 28, 28, 1)
                #print("batch ", str(i), " : ", batch_xs.shape, batch_ys.shape, batch_xs.dtype, batch_ys.dtype)

                _, cost_val = session.run([optimizer, cost], feed_dict={X: batch_xs, Y: batch_ys, keep_prob:0.7})
                total_cost += cost_val

            print("epoch ", epoch, " average cost:", "{:.3f}".format(total_cost / total_batch))

        print("optimization completed")

        '''
        training이 완료된 모델로 test 데이터의 성능을 측정한다.        
        '''
        isCorrect = tf.equal(tf.argmax(model, 1), tf.argmax(Y, 1)) #각각이 정답 레이블과 일치하는지 확인.
        accuracy = tf.reduce_mean(tf.cast(isCorrect, tf.float32)) #전체 결과를 평균(true/false를 float32로 변환).
        # validation단계이므로 dropout은 하지 않음.
        print("accuracy : ", session.run([accuracy], feed_dict={X:testX.reshape(-1, 28, 28, 1), Y:testY, keep_prob:1.0}))

run()