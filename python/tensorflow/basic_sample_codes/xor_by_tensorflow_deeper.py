import tensorflow as tf
import numpy as np

x_data = [[0, 0], [0, 1], [1, 0], [1, 1]]
y_data = [[0], [1], [1], [0]]

input_cnt = 4

W1 = tf.Variable(tf.random_normal([2, 10]), dtype=tf.float32, name='weight01')
b1 = tf.Variable(tf.random_normal([10]), dtype=tf.float32, name='bias01')

W2 = tf.Variable(tf.random_normal([10, 8]), dtype=tf.float32, name='weight02')
b2 = tf.Variable(tf.random_normal([8]), dtype=tf.float32, name='bias02')

W3 = tf.Variable(tf.random_normal([8, 4]), dtype=tf.float32, name='weight03')
b3 = tf.Variable(tf.random_normal([4]), dtype=tf.float32, name='bias03')

W4 = tf.Variable(tf.random_normal([4, 1]), dtype=tf.float32, name='weight04')
b4 = tf.Variable(tf.random_normal([1]), dtype=tf.float32, name='bias04')

X = tf.placeholder(tf.float32, name="X", shape=[input_cnt, 2])
Y = tf.placeholder(tf.float32, name="Y", shape=[input_cnt, 1])

my1 = tf.sigmoid(tf.matmul(X, W1) + b1)
my2 = tf.sigmoid(tf.matmul(my1, W2) + b2)
my3 = tf.sigmoid(tf.matmul(my2, W3) + b3)
hypothesis = tf.sigmoid(tf.matmul(my3, W4) + b4)
print("hypothesis.shape:", hypothesis.shape)  # (4, 1)

cost = -tf.reduce_mean(Y * tf.log(hypothesis) + (1 - Y) * tf.log(1 - hypothesis))
train = tf.train.GradientDescentOptimizer(learning_rate=0.1).minimize(cost)

'''
building tensorboard summary
'''
w1_hist = tf.summary.histogram('weights01', W1)
w2_hist = tf.summary.histogram('weights02', W2)
w3_hist = tf.summary.histogram('weights03', W3)
w4_hist = tf.summary.histogram('weights04', W4)
tf.summary.histogram('b1', b1)
tf.summary.histogram('b2', b2)
tf.summary.histogram('b3', b3)
tf.summary.histogram('b4', b4)
cost_sum = tf.summary.scalar('cost', cost)
summary = tf.summary.merge_all()


# accuracy check
predicted = tf.cast(hypothesis > 0.5, dtype=tf.float32)
accuracy = tf.reduce_mean(tf.cast(tf.equal(predicted, Y), dtype=tf.float32))

with tf.Session() as session:
    session.run(tf.global_variables_initializer())

    # create writer and add graph:
    writer = tf.summary.FileWriter("./logs")
    writer.add_graph(session.graph)

    for step in range(10001):
        s, _ = session.run([summary, train], feed_dict={X: x_data, Y: y_data})
        writer.add_summary(s, global_step=step)
        if step % 1000 == 0:
            print(step, session.run([cost], feed_dict={X: x_data, Y: y_data}), session.run([W1, W2, W3, W4]))
            h, c, a = session.run([hypothesis, predicted, accuracy], feed_dict={X: x_data, Y: y_data})
            print("hypothesis:", h, "correct:", c, "accuracy:", a)

    # 최종 결과
    h, c, a = session.run([hypothesis, predicted, accuracy], feed_dict={X: x_data, Y: y_data})
    print("hypothesis:", h, "correct:", c, "accuracy:", a)


