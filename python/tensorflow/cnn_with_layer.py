import tensorflow as tf

from tensorflow.examples.tutorials.mnist import input_data
mnist = input_data.read_data_sets("./mnist/data/", one_hot=True)

X = tf.placeholder(tf.float32, [None, 28, 28, 1])
T = tf.placeholder(tf.float32, [None, 10])
dropout_ratio = tf.placeholder(tf.float32)
is_training = tf.placeholder(tf.bool)

'''
첫번째 layer 
W1 = tf.Variable(tf.random_normal([3,3,1,32], stddev=0.01))
L1 = tf.nn.conv2d(X, W1, strides=[1,1,1,1], padding="SAME")
L1 = tf.nn.relu(L1)
L1 = tf.nn.max_pool(L1, ksize=[1,2,2,1], strides=[1,2,2,1], padding="SAME") # pooling layer 생성:
위 코드를 layers 사용해서 아래와 같이 작성할 수 있다.
'''
L1 = tf.layers.conv2d(X, 32, [3, 3])
L1 = tf.layers.max_pooling2d(L1, [2,2], [2,2])
L1 = tf.layers.dropout(L1, dropout_ratio, is_training)

'''
두번째 layer
'''
L2 = tf.layers.conv2d(L1, 64, [3, 3])
L2 = tf.layers.max_pooling2d(L2, [2, 2], [2, 2])
L2 = tf.layers.dropout(L2, dropout_ratio, is_training)

'''
완전연결 계층으로 변환
W3 = tf.Variable(tf.random_normal([7 * 7 * 64, 256], stddev=0.01))
L3 = tf.reshape(L2, [-1, 7 * 7 * 64])
L3 = tf.matmul(L3 * W3)
L3 = tf.nn.relu(L3)
위 코드를 layers 사용해서 다음과 같이 변경할 수 있다.
'''
L3 = tf.contrib.layers.flatten(L2)
L3 = tf.layers.dense(L3, 256, activation=tf.nn.relu)
L3 = tf.layers.dropout(L3, dropout_ratio, is_training)

model = tf.layers.dense(L3, 10, activation=None)

cost = tf.reduce_mean(tf.nn.softmax_cross_entropy_with_logits(logits=model, labels=T))
optimizer = tf.train.AdamOptimizer(0.001).minimize(cost)

'''
training :
'''
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
        _, cost_val = session.run([optimizer, cost], feed_dict={X:batch_xs, T:batch_ts, is_training:True, dropout_ratio:0.7})

        total_cost += cost_val

    print("Epoch ", epoch, " Average Cost : {:.4f}".format(total_cost / total_batch))

print("training completed.")

is_correct = tf.equal(tf.argmax(model, 1), tf.argmax(T, 1))
accuracy = tf.reduce_mean(tf.cast(is_correct, tf.float32))
print("Accuracy : ", session.run(accuracy, feed_dict={X:mnist.test.images.reshape(-1, 28, 28, 1), T:mnist.test.labels, is_training:False, dropout_ratio:1.0}))