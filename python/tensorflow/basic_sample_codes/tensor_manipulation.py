import tensorflow as tf
import numpy as np

t = np.array([0, 1, 2, 3, 4, 5, 6,], )
print(t.ndim)
print(t.shape)

# 2d array
t = np.array([[1,2,3], [4,5,6], [7,8,8], [10,11,12]])
print("t.ndim:", t.ndim) #rank
print("t.shape:", t.shape)

t = tf.constant([[[[1,2,3,4], [5,6,7,8], [9,10,11,12]],
                  [[13,14,15,16], [17,18,19,20], [21,22,23,24]]]])
with tf.Session() as session:
    session.run(tf.global_variables_initializer())
    print("tf.shape(t).eval() ", tf.shape(t).eval())
    print(t.shape)


t = np.array([[[[1,2,3,4], [5,6,7,8], [9,10,11,12]],
                  [[13,14,15,16], [17,18,19,20], [21,22,23,24]]]])
print(t.ndim)



