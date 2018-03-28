import tensorflow as tf
import numpy as np

x_data = np.array([[0, 0],
                   [1, 0],
                   [1, 1],
                   [0, 0],
                   [0, 0],
                   [0, 1]])


t_data = np.array([[1, 0, 0],
                   [0, 1, 0],
                   [0, 0, 1],
                   [1, 0, 0],
                   [1, 0, 0],
                   [0, 0, 1]])

X = tf.placeholder(tf.float32)
T = tf.placeholder(tf.float32)

#X.shape = (1,2)
#W = tf.Variable(tf.random_uniform([2,3], -1, 1))
W1 = tf.Variable(tf.random_uniform([2,10], -1.0, 1.0))
W2 = tf.Variable(tf.random_uniform([10, 3], -1.0, 1.0))
# 결국 [1,3] 형태로 나온다.

#b = tf.Variable(tf.zeros([3]))
b1 = tf.Variable(tf.zeros([10]))
b2 = tf.Variable(tf.zeros([3]))

#L = tf.nn.relu(tf.add(tf.matmul(X, W), b))
#model = tf.nn.softmax(L)
L1 = tf.nn.relu(tf.add(tf.matmul(X, W1), b1))
model = tf.add(tf.matmul(L1, W2), b2)


'''
cross entropy 함수를 손실함수로 사용:
cross entropy 함수 : 예측값과 실제값 사이의 확률분포 차이를 계산한 값.
음수로 표현하기 위해 -붙임
'''
# cost = tf.reduce_mean(-tf.reduce_sum(T * tf.log(model), axis=1))
# optimizer = tf.train.GradientDescentOptimizer(learning_rate=0.01)
# train_op = optimizer.minimize(cost)
cost = tf.reduce_mean(tf.nn.softmax_cross_entropy_with_logits(labels=T, logits=model))
optimizer = tf.train.AdamOptimizer(learning_rate=0.01)
train_op = optimizer.minimize(cost)

init = tf.global_variables_initializer()
session = tf.Session()
session.run(init)

for step in range(100):
    session.run(train_op, feed_dict={X: x_data, T: t_data})

    if (step + 1) % 10 == 0:
        print(step, "cost :", session.run(cost, feed_dict={X: x_data, T: t_data}))

prediction = tf.argmax(model, axis=1) #argmax() : 가장 큰 값의 인덱스를 리턴한다.
target = tf.argmax(T, axis=1) # 실제값 인덱스
print("예측값:", session.run(prediction, feed_dict={X:x_data}))
print("실제값:", session.run(target, feed_dict={T: t_data}))

is_correct = tf.equal(prediction, target)
accuracy = tf.reduce_mean(tf.cast(is_correct, tf.float32))
print("정확도: %.2f" % session.run(accuracy * 100, feed_dict={X: x_data, T: t_data}))

