import tensorflow as tf
from tensorflow.examples.tutorials.mnist import input_data
import numpy as np
import matplotlib.pyplot as plt
import random

def sample_01():
    session = tf.InteractiveSession()
    image = np.array(
        [
            [
                [
                    [1], [2], [3]
                ],
                [
                    [4], [5], [6]
                ],
                [
                    [7], [8], [9]
                ]
            ]
        ],
        dtype=np.float32
    )
    print("image.shape", image.shape)
    plt.imshow(image.reshape(3,3), cmap='Greys')


    weight = tf.constant([[[[1.]], [[1.]]], [[[1.]], [[1.]]]])
    print("weight.shape", weight.shape)

    conv2d = tf.nn.conv2d(image, weight, strides=[1.,1.,1.,1.], padding='VALID')
    conv2d_img = conv2d.eval()
    print("conv2d_img.shape", conv2d_img.shape)
    conv2d_img = np.swapaxes(conv2d_img, 0, 3)
    for i, one_img in enumerate(conv2d_img):
        print("one_img.reshape(2,2)", one_img.reshape(2,2))
        plt.subplot(1, 2, i+1), plt.imshow(one_img.reshape(2,2), cmap='gray')

    plt.show()



def sample_02():
    session = tf.InteractiveSession()
    image = np.array(
        [
            [
                [
                    [1], [2], [3]
                ],
                [
                    [4], [5], [6]
                ],
                [
                    [7], [8], [9]
                ]
            ]
        ],
        dtype=np.float32
    )
    print("image.shape", image.shape)
    plt.imshow(image.reshape(3, 3), cmap='Greys')

    weight = tf.constant([[[[1., 10., -1.]], [[1., 10., -1.]]], [[[1., 10., -1.]], [[1., 10., -1.]]]])
    print("weight.shape", weight.shape) #(2, 2, 1, 3) 마지막 3은 다음으로 넘어가는 channel 수가 된다.

    conv2d = tf.nn.conv2d(image, weight, strides=[1., 1., 1., 1.], padding='SAME')
    conv2d_img = conv2d.eval()
    print("conv2d_img.shape", conv2d_img.shape)
    conv2d_img = np.swapaxes(conv2d_img, 0, 3)
    for i, one_img in enumerate(conv2d_img):
        print("one_img.reshape(3,3)", one_img.reshape(3, 3))
        plt.subplot(1, 3, i + 1), plt.imshow(one_img.reshape(3, 3), cmap='gray')

    plt.show()

def sample_mnist():
    mnist = input_data.read_data_sets("MINST_data/", one_hot=True)
    session = tf.InteractiveSession()
    img = mnist.test.images[random.randrange(0, 500)]
    img = img.reshape(-1, 28, 28, 1)
    W1 = tf.Variable(tf.random_normal([3, 3, 1, 5], stddev=0.01))
    conv2d = tf.nn.conv2d(img, W1, strides=[1,2,2,1], padding='SAME')
    print(conv2d)
    with tf.Session() as session:
        session.run(tf.global_variables_initializer())
        conv2d_img = conv2d.eval()
        print("conv2d_img", conv2d_img.shape)
        conv2d_img = np.swapaxes(conv2d_img, 0, 3)
        print("conv2d_img.swapaxes(0, 3)", conv2d_img.shape)
        for i, img in enumerate(conv2d_img):
            plt.subplot(1, 5, i+1)
            plt.imshow(img.reshape(14, 14), cmap='gray')
    plt.show()

def sample_maxpool():
    mnist = input_data.read_data_sets("MINST_data/", one_hot=True)
    session = tf.InteractiveSession()
    img = mnist.test.images[random.randrange(0, 500)]
    img = img.reshape(-1, 28, 28, 1)
    W1 = tf.Variable(tf.random_normal([3, 3, 1, 5], stddev=0.01))
    conv2d = tf.nn.conv2d(img, W1, strides=[1, 2, 2, 1], padding='SAME')
    print("conv2d:", conv2d)
    pool = tf.nn.max_pool(conv2d, ksize=[1,2,2,1], strides=[1,2,2,1], padding='SAME')
    print("pool:", pool)
    with tf.Session() as session:
        session.run(tf.global_variables_initializer())
        pool_img = pool.eval()
        pool_img = np.swapaxes(pool_img, 0, 3)
        print("pool_img.swapaxes(0, 3)", pool_img.shape)
        for i, img in enumerate(pool_img):
            plt.subplot(1, 5, i + 1)
            plt.imshow(img.reshape(7, 7), cmap='gray')
    plt.show()

sample_maxpool()