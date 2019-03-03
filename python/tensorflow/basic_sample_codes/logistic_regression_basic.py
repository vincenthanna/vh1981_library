import tensorflow as tf
import numpy as np

x_data = [[1,2], [2,3], [3,1], [4,3], [5,3], [6,2]]
y_data = [[0], [0], [0], [1], [1], [1]]

X = tf.placeholder(tf.float32, shape=[None, 2])
Y = tf.placeholder(tf.float32, shape=[None, 1])
W = tf.Variable(tf.random_normal([2, 1], seed=1), name='weight')
b = tf.Variable(tf.random_normal([1], seed=1), name='bias')

# hypothesis using sigmoid
hypothesis = tf.sigmoid(tf.matmul(X, W) + b)

# cost function
cost = -tf.reduce_mean(Y * tf.log(hypothesis) + (1-Y) * tf.log(1 - hypothesis))

# optimize
train = tf.train.GradientDescentOptimizer(learning_rate=0.01).minimize(cost)

# 정확도 계산
pred = tf.cast(hypothesis > 0.5, dtype=tf.float32)
accuracy = tf.reduce_mean(tf.cast(tf.equal(pred, Y), dtype=tf.float32))

with tf.Session() as session:
    session.run(tf.global_variables_initializer())

    for step in range(10001):
        cv, _ = session.run([cost, train], feed_dict={X:x_data, Y:y_data})
        if step % 200 == 0:
            print(step, cv)

    h, c, a = session.run([hypothesis, pred, accuracy],
                          feed_dict={X:x_data, Y:y_data})

    print("Hypothesis:", h, "correct(Y):", c, "accuracy:", a)
