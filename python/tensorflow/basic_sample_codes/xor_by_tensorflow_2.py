import tensorflow as tf
import numpy as np

#x_data = [[0, 0], [0, 1], [1, 0], [1, 1]]
input_cnt = 1

W1 = tf.Variable([[5, -7],
                  [5, -7]], dtype=tf.float32)
b1 = tf.Variable([-8, 3], dtype=tf.float32)

W3 = tf.Variable([[-11],
                  [-11]], dtype=tf.float32)
b3 = tf.Variable([6], dtype=tf.float32)

X = tf.placeholder(tf.float32, name="X", shape=[input_cnt, 2])
Y = tf.placeholder(tf.float32, name="Y", shape=[1])

my1 = tf.sigmoid(tf.matmul(X, W1) + b1)

hypothesis = tf.sigmoid(tf.matmul(my1, W3) + b3)
print("my1:", my1.shape)
print("W3:", W3.shape)

with tf.Session() as session:
    session.run(tf.global_variables_initializer())

    h = session.run([hypothesis], feed_dict={X: [[0, 0]], Y: [0]})
    print("h:", h)

    h = session.run([hypothesis], feed_dict={X: [[0, 0]], Y: [0]})
    print(np.array(h).reshape([1]))
    h = session.run([hypothesis], feed_dict={X: [[0, 1]], Y: [0]})
    print(np.array(h).reshape([1]))
    h = session.run([hypothesis], feed_dict={X: [[1, 0]], Y: [0]})
    print(np.array(h).reshape([1]))
    h = session.run([hypothesis], feed_dict={X: [[1, 1]], Y: [0]})
    print(np.array(h).reshape([1]))


