import tensorflow as tf
import numpy as np

tf.set_random_seed(777)

xy = np.loadtxt('data-01-test-score.csv', delimiter=',', dtype=np.float32)

x_data = xy[:, 0:-1] #전체 행, 열은 끝에서 하나 뺀다.
y_data = xy[:, [-1]] #한번더 감싼 이유는 [n행 1열로 변경하기 위함

print(x_data.shape)
print(y_data.shape)

# model 빌딩 단계:
X = tf.placeholder(tf.float32, shape=[None, 3])
Y = tf.placeholder(tf.float32, shape=[None, 1])

W = tf.Variable(tf.random_normal([3, 1], seed=1), dtype=tf.float32)
b = tf.Variable(tf.random_normal([1], seed=1), dtype=tf.float32)

hypothesis = tf.matmul(X, W) + b

cost = tf.reduce_mean(tf.square(hypothesis - Y))
optimizer = tf.train.GradientDescentOptimizer(learning_rate=1e-5)
train = optimizer.minimize(cost)

session = tf.Session()

session.run(tf.global_variables_initializer())

i_w, i_b = session.run([W, b])
print("initial variables:", "W,b : {", i_w, i_b, "}")

# train 단계
for step in range(2001):
    #print("x:", x_data, "y:", y_data)
    cv, hv, _ = session.run([cost, hypothesis, train], feed_dict={X:x_data, Y:y_data})
    if step % 10 == 0:
        print(step, "cost:", cv, "prediction:", hv)

_w, _b = session.run([W, b])
print("variables:", "W,b : {", _w, _b, "}")
# test 단계 : train된 모델로 계산해보자
print("your score will be:", session.run(hypothesis, feed_dict={X:[[100, 70, 101], [60, 70, 110], [90, 100, 80]]}))