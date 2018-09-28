import tensorflow as tf

from tensorflow.examples.tutorials.mnist import input_data
mnist = input_data.read_data_sets("./mnist/data/", one_hot=True)

lr = 0.001
epochs = 5
batch_size = 128

n_input = 28
n_step = 28
n_hidden = 128
n_class = 10

X = tf.placeholder(tf.float32, [None, n_step, n_input]) # (?, 28, 28)
Y = tf.placeholder(tf.float32, [None, n_class]) # (

print("X:", X.shape, "Y:", Y.shape)

W = tf.Variable(tf.random_normal([n_hidden, n_class]))
b = tf.Variable(tf.random_normal([n_class]))

cell = tf.nn.rnn_cell.BasicRNNCell(n_hidden)
outputs, states = tf.nn.dynamic_rnn(cell, X, dtype=tf.float32)

outputs = tf.transpose(outputs, [1,0,2])
print("outputs:", outputs.shape)
outputs = outputs[-1]
print("outputs:", outputs.shape)

model = tf.matmul(outputs, W) + b

cost = tf.reduce_mean(tf.nn.softmax_cross_entropy_with_logits(logits=model, labels=Y))
optimizer = tf.train.AdamOptimizer(lr).minimize(cost)

with tf.Session() as session:
    session.run(tf.global_variables_initializer())

    total_batch = int(mnist.train.num_examples / batch_size)

    for epoch in range(epochs):
        total_cost = 0
        for i in range(total_batch):
            batch_xs, batch_ys = mnist.train.next_batch(batch_size)
            #print("batch_xs:", batch_xs.shape, " batch_ys:", batch_ys.shape)
            batch_xs = batch_xs.reshape((batch_size, n_step, n_input))

            _, cost_val = session.run([optimizer, cost], feed_dict={X:batch_xs, Y:batch_ys})
            total_cost += cost_val

        print("epoch:", epoch+1, " avg_cost={:.3f}".format(total_cost/total_batch))

    print("optimization completed")

    is_correct = tf.equal(tf.argmax(model, 1), tf.argmax(Y, 1))
    accuracy = tf.reduce_mean(tf.cast(is_correct, tf.float32))

    test_batch_size = len(mnist.test.images)
    test_xs = mnist.test.images.reshape(test_batch_size, n_step, n_input)
    test_ys = mnist.test.labels

    print("accuracy:", session.run(accuracy, feed_dict={X:test_xs, Y:test_ys}))