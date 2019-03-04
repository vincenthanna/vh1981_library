import tensorflow as tf
import numpy as np

x_data = [[1,2,1,1,], [2,1,3,2], [3,1,3,4], [4,1,5,5], [1,7,5,5], [1,2,5,6], [1,6,6,6], [1,7,7,7]]

y_data = [[0,0,1], [0,0,1], [0,0,1], [0,1,0], [0,1,0], [0,1,0], [1,0,0], [1,0,0]]
#tf.one_hot([2,2,2,1,1,1,0,0], depth=3)

X = tf.placeholder(tf.float32, shape=[None, 4])
Y = tf.placeholder(tf.float32, shape=[None, 3])
nb_classes = 3

W = tf.Variable(tf.random_normal([4, nb_classes], seed=1), name='variable')
b = tf.Variable(tf.random_normal([nb_classes], seed=1), name='bias')

# make hypothesis : use softmax
hypothesis = tf.nn.softmax(tf.matmul(X, W) + b)

'''
axis관련해서는 아래 설명을 참고한다.
https://www.tensorflow.org/api_docs/python/tf/math/reduce_sum

x = tf.constant([[1, 1, 1], [1, 1, 1]])
tf.reduce_sum(x)  # 6
tf.reduce_sum(x, 0)  # [2, 2, 2]
tf.reduce_sum(x, 1)  # [3, 3]
tf.reduce_sum(x, 1, keepdims=True)  # [[3], [3]]
tf.reduce_sum(x, [0, 1])  # 6
'''
cost = tf.reduce_mean(-tf.reduce_sum(Y * tf.log(hypothesis), axis=1))
optimizer = tf.train.GradientDescentOptimizer(learning_rate=0.1).minimize(cost)

with tf.Session() as session:
    session.run(tf.global_variables_initializer())

    for step in range(2001):
        session.run(optimizer, feed_dict={X:x_data, Y:y_data})
        if step % 200 == 0:
            print(step, session.run(cost, feed_dict={X:x_data, Y:y_data}))

    a = session.run(hypothesis, feed_dict={X:x_data, Y:y_data})
    print(a, session.run(tf.argmax(a, axis=1))) #argmax()의 axis=1인 이유를 짚어볼것