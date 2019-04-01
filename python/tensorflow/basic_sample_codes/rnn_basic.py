import tensorflow as tf
import numpy as np


def rnn_simple():
    hidden_size = 3

    cell = tf.contrib.rnn.BasicLSTMCell(num_units=hidden_size)
    x_data = np.array([[[1, 0, 0, 0]]], dtype=np.float32)
    print("input shape :", x_data.shape)
    outputs, _states = tf.nn.dynamic_rnn(cell, x_data, dtype=tf.float32)

    with tf.Session() as session:
        session.run(tf.global_variables_initializer())
        out = session.run(outputs)

        print("out : shape=", out.shape, "value:", out)


h = [1, 0, 0, 0]
e = [0, 1, 0, 0]
l = [0, 0, 1, 0]
o = [0, 0, 0, 1]
def rnn_simple_wide():

    hidden_size = 2
    cell = tf.contrib.rnn.BasicLSTMCell(num_units=hidden_size)
    x_data = np.array([[h, e, l, l, o]], dtype=np.float32)
    print("input shape:", x_data.shape)
    outputs, states = tf.nn.dynamic_rnn(cell, x_data, dtype=tf.float32)

    with tf.Session() as session:
        session.run(tf.global_variables_initializer())
        print("outputs shape:", outputs.eval().shape, "\nvalue:\n", outputs.eval())


def rnn_simple_batch_input():
    hidden_size = 2
    x_data = np.array([[h, e, l, l, o],
                       [e, o, l, l, l],
                       [l, l, e, e, l]], dtype=np.float32)
    print("input shape:", x_data.shape)

    cell = tf.contrib.rnn.BasicLSTMCell(num_units=hidden_size)

    outputs, states = tf.nn.dynamic_rnn(cell, x_data, dtype=tf.float32)

    with tf.Session() as session:
        session.run(tf.global_variables_initializer())
        print("outputs shape:", outputs.eval().shape, "\nvalue:\n", outputs.eval())

rnn_simple_batch_input()