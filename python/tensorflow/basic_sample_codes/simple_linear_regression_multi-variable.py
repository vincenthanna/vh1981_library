import tensorflow as tf
import numpy as np

# x1_data = [73, 93, 89, 96, 73]
# x2_data = [80, 88, 91, 98, 66]
# x3_data = [75, 93, 90, 100, 70]

x_data = [[73., 80., 75.],
          [93., 88., 93.],
          [89., 91., 90.],
          [96., 98., 100.],
          [73., 66., 70.]]

y_data = [[152.], [185.], [180.], [196.], [142.]]

X = tf.placeholder(tf.float32, shape=[None, 3], name='placeholder_x')
Y = tf.placeholder(tf.float32, shape=[None, 1], name='placeholder_y')

W = tf.Variable(tf.random_normal([3, 1]), name='weight')
b = tf.Variable(tf.random_normal([1]), name='bias')

hypothesis = tf.matmul(X, W) + b #hypothesis 수식 구성

'''
cost : 정답(Y)와 prediction값과의 L2 norm의 평균.
'''
cost = tf.reduce_mean(tf.square(hypothesis - Y)) #X(train or test) 데이터 수가 여러개이면 여기서 reduce_mean으로 배열을 평균값으로 변경
optimizer = tf.train.GradientDescentOptimizer(learning_rate=1e-5)
train = optimizer.minimize(cost)

session = tf.Session() # tensorflow 세션 생성
# 전역 tensor를 초기화 한다.
session.run(tf.global_variables_initializer()) #Returns an Op that initializes global variables.

for step in range(2001):
    cost_val, hy_val, _ = session.run([cost, hypothesis, train], feed_dict={X: x_data, Y: y_data})

    if step % 10 == 0:
        print(step, "Cost:", cost_val, "prediction:", hy_val)