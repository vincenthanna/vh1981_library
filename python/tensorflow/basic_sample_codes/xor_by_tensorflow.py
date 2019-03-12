import tensorflow as tf
import numpy as np

#x_data = [[0, 0], [0, 1], [1, 0], [1, 1]]
input_cnt = 1

W1 = tf.Variable([[5],
                  [5]], dtype=tf.float32)
b1 = tf.Variable([-8], dtype=tf.float32)

W2 = tf.Variable([[-7],
                  [-7]], dtype=tf.float32)
b2 = tf.Variable([3], dtype=tf.float32)

W3 = tf.Variable([[-11],
                  [-11]], dtype=tf.float32)
b3 = tf.Variable([6], dtype=tf.float32)

X = tf.placeholder(tf.float32, name="X", shape=[input_cnt, 2])
Y = tf.placeholder(tf.float32, name="Y", shape=[1])

my1 = tf.sigmoid(tf.matmul(X, W1) + b1)
my2 = tf.sigmoid(tf.matmul(X, W2) + b2)

mx = tf.reshape(tf.stack([my1, my2], axis=1), [input_cnt,2])
print(mx.shape)

hypothesis = tf.sigmoid(tf.matmul(mx, W3) + b3)

with tf.Session() as session:
    session.run(tf.global_variables_initializer())
    h = session.run([hypothesis], feed_dict={X:[[0, 0]], Y: [0]})
    print(np.array(h).reshape([1]))
    h = session.run([hypothesis], feed_dict={X: [[0, 1]], Y: [0]})
    print(np.array(h).reshape([1]))
    h = session.run([hypothesis], feed_dict={X: [[1, 0]], Y: [0]})
    print(np.array(h).reshape([1]))
    h = session.run([hypothesis], feed_dict={X: [[1, 1]], Y: [0]})
    print(np.array(h).reshape([1]))


