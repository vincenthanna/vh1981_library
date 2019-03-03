import tensorflow as tf
import numpy as np

#aaa = np.random.rand(1,3,5)
aaa = tf.constant([[[1,2,3,4,5],[6,7,8,9,10],[11,12,13,14,15]]])
aaa = tf.transpose(aaa, [1,0,2])
t1 = tf.Print(aaa, [aaa], message="this is a 1")
print("t1:", aaa)
aaa = aaa[-1]
t2 = tf.Print(aaa, [aaa], message="this is a 2")


from tensorflow.examples.tutorials.mnist import input_data
mnist = input_data.read_data_sets("./mnist/data/", one_hot=True)

lr = 0.001
epochs = 5
batch_size = 128

n_input = 28
n_step = 28
n_hidden = 128 # cell의 output의 크기가 된다.
n_class = 10

X = tf.placeholder(tf.float32, [None, n_step, n_input]) # (?, 28, 28) #[batch_size , sequence_size, input_size]
Y = tf.placeholder(tf.float32, [None, n_class]) # (?, 10)

print("X:", X.shape, "Y:", Y.shape)

W = tf.Variable(tf.random_normal([n_hidden, n_class]))
b = tf.Variable(tf.random_normal([n_class]))

cell = tf.nn.rnn_cell.BasicRNNCell(n_hidden)
outputs, states = tf.nn.dynamic_rnn(cell, X, dtype=tf.float32)
#print("outputs:", outputs.shape)

#원래 출력은 [?, 28, 128](X에서 input_size만 hidden_layer크기로 변경)임
# outputs : [batch_size, seq_size, n_hidden]
# => [seq_size, batch_size, n_hidden]
outputs = tf.transpose(outputs, [1,0,2])
# => [batch_size, n_hidden]
outputs = outputs[-1] #마지막단계의 결과값만 취한다.

model = tf.matmul(outputs, W) + b #연산 결과가 [batch_size, n_class]가 된다.

cost = tf.reduce_mean(tf.nn.softmax_cross_entropy_with_logits(logits=model, labels=Y))
optimizer = tf.train.AdamOptimizer(lr).minimize(cost)

with tf.Session() as session:
    session.run(tf.global_variables_initializer())

    t1.eval()
    t2.eval()

    total_batch = int(mnist.train.num_examples / batch_size)

    for epoch in range(epochs):
        total_cost = 0
        for i in range(total_batch):
            batch_xs, batch_ys = mnist.train.next_batch(batch_size)
            #batch_xs = (128, 784), batch_ys = (128, 10)
            print("batch_xs:", batch_xs.shape, " batch_ys:", batch_ys.shape)
            batch_xs = batch_xs.reshape((batch_size, n_step, n_input)) # RNN에 넣기위해 변환(batch, seq_len, input_size)

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