import tensorflow as tf
import numpy as np

tf.set_random_seed(777)

filename_queue = tf.train.string_input_producer(['data-01-test-score.csv'], shuffle=False, name='filename_queue')

reader = tf.TextLineReader()
key, val = reader.read(filename_queue)

record_defaults = [[0.],[0.],[0.],[0.]]
xy = tf.decode_csv(val, record_defaults=record_defaults)

x_data_batch, y_data_batch = tf.train.batch([xy[0:-1], xy[-1:]], batch_size=25)

# model 빌딩 단계:
X = tf.placeholder(tf.float32, shape=[None, 3])
Y = tf.placeholder(tf.float32, shape=[None, 1])

W = tf.Variable(tf.random_normal([3, 1], seed=1), dtype=tf.float32, name='weight')
b = tf.Variable(tf.random_normal([1], seed=1), dtype=tf.float32, name='bias')

hypothesis = tf.matmul(X, W) + b

cost = tf.reduce_mean(tf.square(hypothesis - Y))
optimizer = tf.train.GradientDescentOptimizer(learning_rate=1e-5)
train = optimizer.minimize(cost)

session = tf.Session()

session.run(tf.global_variables_initializer())

# start populating the filename queue:
coord = tf.train.Coordinator()
threads = tf.train.start_queue_runners(sess=session, coord=coord)

i_w, i_b = session.run([W, b])
print("initial variables:", "W,b : {", i_w, i_b, "}")

# train 단계
for step in range(2001):
    x, y = session.run([x_data_batch, y_data_batch])
    #print("x:", x, "y:", y)
    # print("y:", y)
    cv, hv, _ = session.run([cost, hypothesis, train], feed_dict={X:x, Y:y})
    if step % 10 == 0:
        print(step, "cost:", cv, "prediction:", hv)

coord.request_stop()
coord.join(threads)

_w, _b = session.run([W, b])
print("variables:", "W,b : {", _w, _b, "}")

# test 단계 : train된 모델로 계산해보자
print("your score will be:", session.run(hypothesis, feed_dict={X:[[100, 70, 101], [60, 70, 110], [90, 100, 80]]}))