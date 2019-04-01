import tensorflow as tf
import numpy as np

sample = 'if you want you'
idx2char = list(set(sample))
char2idx = {c:i for i, c in enumerate(idx2char)}
print(char2idx)
sample_idx = [char2idx[c] for c in sample]
print(sample_idx)
x_data = [sample_idx[:-1]] # X data sample(0 ~ n-1)
y_data = [sample_idx[1:]] # Y label sample(1 ~ n)

dic_size = len(char2idx)
rnn_hidden_size = len(char2idx)
num_classes = len(char2idx)
batch_size = 1
sequence_length = len(sample) - 1

X = tf.placeholder(tf.int32, [None, sequence_length])
X_oh = tf.one_hot(X, num_classes)
Y = tf.placeholder(tf.int32, [None, sequence_length])

rnn_cell = tf.contrib.rnn.BasicLSTMCell(num_units=rnn_hidden_size)
initial_state = rnn_cell.zero_state(batch_size, tf.float32)

outputs, _states = tf.nn.dynamic_rnn(rnn_cell, X_oh, initial_state=initial_state, dtype=tf.float32)
print("outputs.shape:", outputs.shape)
prediction = tf.argmax(outputs, axis=2, output_type=tf.int32)
correct = tf.equal(tf.reshape(prediction, [-1]), tf.reshape(Y, [-1]))
accuracy = tf.reduce_mean(tf.cast(correct, dtype=tf.float32))

weights = tf.ones([batch_size, sequence_length])
sequence_loss = tf.contrib.seq2seq.sequence_loss(logits=outputs, targets=Y, weights=weights)
print("sequence_loss.shape:", sequence_loss.shape)
loss = tf.reduce_mean(sequence_loss)
print("loss.shape:", loss.shape)
train = tf.train.AdamOptimizer(learning_rate=0.1).minimize(loss)

with tf.Session() as session:
    session.run(tf.global_variables_initializer())
    for i in range(3000):
        l, _ = session.run([loss, train], feed_dict={X: x_data, Y: y_data})
        result, acc = session.run([prediction, accuracy], feed_dict={X: x_data, Y: y_data})
        if (i % 100) == 0:
            print(i, "loss:", l, "prediction:", result, "expected:", y_data, "accuracy:", acc)
            result_str = [idx2char[c] for c in np.squeeze(result)]
            print("\tprediction str: ", ''.join(result_str))
    print("final test:")
    result = session.run(prediction, feed_dict={X:x_data})
    result_str = [idx2char[c] for c in np.squeeze(result)]
    print("\tprediction str: ", ''.join(result_str))