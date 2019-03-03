import tensorflow as tf
import numpy as np
import matplotlib.pyplot as plt

# X = [1,2,3,4,5]
# Y = [2,3,4,5,6]

# X, Y값을 직접 넣지 않고 placeholder를 통해 넘겨준다.
X = tf.placeholder(tf.float32)
Y = tf.placeholder(tf.float32)

W = tf.Variable(tf.random_normal([1]), name='weight')
b = tf.Variable(tf.random_normal([1]), name='bias')

hypothesis = X * W + b #hypothesis 수식 구성

'''
cost : 정답(Y)와 prediction값과의 L2 norm의 평균.
'''
cost = tf.reduce_mean(tf.square(hypothesis - Y)) #X(train or test) 데이터 수가 여러개이면 여기서 reduce_mean으로 배열을 평균값으로 변경

session = tf.Session() # tensorflow 세션 생성
# 전역 tensor를 초기화 한다.
session.run(tf.global_variables_initializer()) #Returns an Op that initializes global variables.

optimizer = tf.train.GradientDescentOptimizer(learning_rate=0.01)
train = optimizer.minimize(cost)

# train model :
for step in range(2001):
    curCost, curW, curb, _ = session.run([cost, W, b, train], feed_dict={X: [1, 2, 3, 4, 5], Y: [2.1, 3.1, 4.1, 5.1, 6.1]})
    if step % 50 == 0:
        print("step:", step, "current cost=", curCost, "W=", curW, "b=", curb)

# test model :
print(session.run(hypothesis, feed_dict={X: [5]}))
print(session.run(hypothesis, feed_dict={X: [6]}))
print(session.run(hypothesis, feed_dict={X: [1.5, 2.5]}))