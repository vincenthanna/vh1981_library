import tensorflow as tf

from tensorflow.examples.tutorials.mnist import input_data
mnist = input_data.read_data_sets("./mnist/data/", one_hot=True)

X = tf.placeholder(tf.float32, [None, 28, 28, 1])
T = tf.placeholder(tf.float32, [None, 10])
dropout_ratio = tf.placeholder(tf.float32)

# 첫번째 layer
W1 = tf.Variable(tf.random_normal([3,3,1,32], stddev=0.01))
L1 = tf.nn.conv2d(X, W1, strides=[1,1,1,1], padding="SAME")
L1 = tf.nn.relu(L1)
L1 = tf.nn.max_pool(L1, ksize=[1,2,2,1], strides=[1,2,2,1], padding="SAME") # pooling layer 생성:

# 두번째 layer
W2 = tf.Variable(tf.random_normal([3,3,32,64], stddev=0.01))
L2 = tf.nn.conv2d(L1, W2, strides=[1,1,1,1], padding="SAME")
L2 = tf.nn.relu(L2)
L2 = tf.nn.max_pool(L2, ksize=[1,2,2,1], strides=[1,2,2,1], padding="SAME") # pooling layer 생성:

W3 = tf.Variable(tf.random_normal([7 * 7 * 64, 256], stddev=0.01))
L3 = tf.reshape(L2, [-1, 7 * 7 * 64])
L3 = tf.matmul(L3, W3)
L3 = tf.nn.relu(L3)
L3 = tf.nn.dropout(L3, dropout_ratio)

'''
직전 L3의 출력 256을 받아 최종값인 0~9 레이블을 가지는 10개의 출력값을 만든다.
'''
W4 = tf.Variable(tf.random_normal([256, 10], stddev=0.01))
model = tf.matmul(L3, W4)

cost = tf.reduce_mean(tf.nn.softmax_cross_entropy_with_logits(logits=model, labels=T))
optimizer = tf.train.AdamOptimizer(0.001).minimize(cost)
#optimizer = tf.train.RMSPropOptimizer(0.001, 0.9).minimize(cost)

init = tf.global_variables_initializer()
session = tf.Session()
session.run(init)

batch_size = 100
total_batch = int(mnist.train.num_examples / batch_size)

epochs = 15
for epoch in range(epochs):
    total_cost = 0

    for i in range(total_batch):
        batch_xs, batch_ts = mnist.train.next_batch(batch_size)
        batch_xs = batch_xs.reshape(-1, 28, 28, 1)

        _, cost_val = session.run([optimizer, cost], feed_dict={X:batch_xs, T:batch_ts, dropout_ratio:0.7})

        total_cost += cost_val

    print("Epoch:", epoch + 1, " Average Cost : {:.3f}".format(total_cost / total_batch))

print("training completed.")

is_correct = tf.equal(tf.argmax(model, 1), tf.argmax(T, 1))
accuracy = tf.reduce_mean(tf.cast(is_correct, tf.float32))

print("정확도:", session.run(accuracy, feed_dict={X:mnist.test.images.reshape(-1, 28, 28, 1), T:mnist.test.labels, dropout_ratio:1.0}))



