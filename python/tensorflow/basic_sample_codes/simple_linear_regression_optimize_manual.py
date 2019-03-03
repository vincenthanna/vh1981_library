import tensorflow as tf
import numpy as np
import matplotlib.pyplot as plt

# X = [1,2,3,4,5]
# Y = [2,3,4,5,6]

# X, Y값을 직접 넣지 않고 placeholder를 통해 넘겨준다.
X = tf.placeholder(tf.float32)
Y = tf.placeholder(tf.float32)

#W = tf.Variable(tf.random_normal([1]), name='weight')
W = tf.Variable(5.0)

hypothesis = X * W #hypothesis 수식 구성

cost = tf.reduce_sum(tf.square(hypothesis - Y))

session = tf.Session() # tensorflow 세션 생성
# 전역 tensor를 초기화 한다.
session.run(tf.global_variables_initializer()) #Returns an Op that initializes global variables.

'''
gradient descent를 직접 구현한 부분
주석 코드의 내용을 직접 구현해 본 모습니다.
'''
# optimizer = tf.train.GradientDescentOptimizer(learning_rate=0.01)
# train = optimizer.minimize(cost)
learning_rate = 0.1
gradient = tf.reduce_mean((W * X - Y) * X) #미분식을 알고 있어야 계산이 가능
descent = W - learning_rate * gradient
update = W.assign(descent)
'''======'''

# train model :
for step in range(20):
    print(step, session.run(cost, feed_dict={X: [1, 2, 3, 4, 5], Y: [1, 2, 3, 4, 5]}), session.run(W))
    session.run([update], feed_dict={X:[1,2,3,4,5], Y:[1,2,3,4,5]})


# test model :
print(session.run(hypothesis, feed_dict={X: [5]}))
print(session.run(hypothesis, feed_dict={X: [6]}))
print(session.run(hypothesis, feed_dict={X: [1.5, 2.5]}))