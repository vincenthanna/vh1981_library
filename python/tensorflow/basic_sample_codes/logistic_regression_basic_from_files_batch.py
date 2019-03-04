import tensorflow as tf
import numpy as np

# file batch 사용하도록 준비
filename_queue = tf.train.string_input_producer(['data-03-diabetes.csv'], shuffle=False, name='filename_queue')
reader = tf.TextLineReader()
key, val = reader.read(filename_queue)
record_defaults = [[0.], [0.], [0.], [0.], [0.], [0.], [0.], [0.], [0.]]
xy = tf.decode_csv(val, record_defaults=record_defaults)
x_data_batch, y_data_batch = tf.train.batch([xy[0:-1], xy[-1:]], batch_size=50)

X = tf.placeholder(tf.float32, shape=[None, 8])
Y = tf.placeholder(tf.float32, shape=[None, 1])
W = tf.Variable(tf.random_normal([8, 1], seed=1), name='weight')
b = tf.Variable(tf.random_normal([1], seed=1), name='bias')

# hypothesis using sigmoid
hypothesis = tf.sigmoid(tf.matmul(X, W) + b)

# cost function : 수식을 그대로 적용
cost = -tf.reduce_mean(Y * tf.log(hypothesis) + (1-Y) * tf.log(1 - hypothesis))

# optimize(parameter update)
train = tf.train.GradientDescentOptimizer(learning_rate=0.01).minimize(cost)

# 정확도 계산
pred = tf.cast(hypothesis > 0.5, dtype=tf.float32)
accuracy = tf.reduce_mean(tf.cast(tf.equal(pred, Y), dtype=tf.float32))

with tf.Session() as session:
    session.run(tf.global_variables_initializer())

    # start populating the filename queue:
    coord = tf.train.Coordinator()
    threads = tf.train.start_queue_runners(sess=session, coord=coord)

    for step in range(10001):
        x, y = session.run([x_data_batch, y_data_batch])
        cv, _ = session.run([cost, train], feed_dict={X:x, Y:y})
        if step % 200 == 0:
            print(step, cv)

    # stop populating filename queue
    coord.request_stop()
    coord.join(threads)

    h, c, a = session.run([hypothesis, pred, accuracy], feed_dict={X: x, Y: y})
    print("Hypothesis:", h, "correct(Y):", c, "accuracy:", a)

    # tf.equal() 결과값이 궁금해서 별도 출력해봄
    #print("tf.equal(pred, Y)=", session.run([tf.equal(pred, Y)], feed_dict={X:x_data, Y:y_data}))
