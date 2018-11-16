import tensorflow as tf
import struct
import numpy as np
import matplotlib.pyplot as plt
import math

#models :
from models.model_simple import build_model_simple

def load_mnists():

    # mnist file names :
    trainSet_label_filename = "train-labels.idx1-ubyte"
    trainSet_image_filename = "train-images.idx3-ubyte"
    testSet_label_filename = "t10k-labels.idx1-ubyte"
    testSet_image_filename = "t10k-images.idx3-ubyte"

    trainSet_labels, trainSet_images = load_mnist_set(trainSet_label_filename, trainSet_image_filename)
    testSet_labels, testSet_images = load_mnist_set(testSet_label_filename, testSet_image_filename)
    return trainSet_images, trainSet_labels, testSet_images, testSet_labels

def one_hot(value, classes):
    return np.eye(classes)[value]

def load_mnist_set(labelfilename, imagefilename):
    labels = []
    images = []
    num_items = 0
    row = 0
    col = 0
    with open(labelfilename, "rb") as f:
        magic_number = struct.unpack('>i', f.read(4))  # 꺽쇠방향은 big/little endian 구분용 < : little, > : big
        # 아래 링크 참조
        # https://stackoverflow.com/questions/3783677/how-to-read-integers-from-a-file-that-are-24bit-and-little-endian-using-python
        magic_number = magic_number[0]
        num_items = struct.unpack('>i', f.read(4))[0]

        for i in range(num_items):
            label = struct.unpack('b', f.read(1))
            label_one_hot = one_hot(label, 10) # one-hot encoding으로 변경
            labels.append(label_one_hot)

        print("magic_number : ", format(magic_number, '08x'))
        print("items : ", num_items)

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

    #return labels, images as numpy array
    return np.array(labels).astype(np.float32), np.array(images).reshape((num_items, col * row)).astype(np.float32)

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

    check_xs = get_batch(trainX, 20, 0)
    check_ys = get_batch(trainY, 20, 0)
    sample_data_check(check_xs, check_ys)

    num_train = trainX.shape[0]
    num_test = testX.shape[0]
    classCnt = testY.shape[1]


    print("num_train : ", num_train, " num_test : ", num_test, "   classCnt : ", classCnt)

    X = tf.placeholder(tf.float32, [None, 28, 28, 1])
    Y = tf.placeholder(tf.float32, [None, classCnt])
    keep_prob = tf.placeholder(tf.float32)

    # select model : FIXME:
    model = build_model_simple(X, keep_prob=keep_prob, labelCnt=classCnt)

    # cost function
    '''
    softmax_cross_entropy_with_logits : computes softmax cross entropy between 'logits' and 'labels'
    '''
    cost = tf.reduce_mean(tf.nn.softmax_cross_entropy_with_logits(logits=model, labels=Y))

    # optimizer
    optimizer = tf.train.AdamOptimizer(0.0001).minimize(cost) #AdamOptimizer() 인자는 Learning Rate
    #RMSPropOptimizer를 사용해보면 어떨까?


    batch_size = 100
    total_batch = int(num_train / batch_size)

    with tf.Session() as session:

        initializer = tf.global_variables_initializer()
        session.run(initializer)

        for epoch in range(5):
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

        isCorrect = tf.equal(tf.argmax(model, 1), tf.argmax(Y, 1))
        accuracy = tf.reduce_mean(tf.cast(isCorrect, tf.float32))
        print("accuracy : ", session.run(accuracy, feed_dict={X:testX.reshape(-1, 28, 28, 1), Y:testY, keep_prob:1.0}))


run()