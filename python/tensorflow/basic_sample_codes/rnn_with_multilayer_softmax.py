import tensorflow as tf
import numpy as np

sentence = ("if you want to build a ship, don't drum up people together to "
            "collect wood and don't assign them tasks and work, but rather "
            "teach them to long for the endless immensity of the sea.")

sequence_length = 10

char_set = list(set(sentence))
char_dic = {w:i for i, w in enumerate(char_set)}

dataX = []
dataY = []
for i in range(0, len(sentence) - sequence_length):
    x_str = sentence[i : i + sequence_length]
    y_str = sentence[i + 1 : i + sequence_length + 1]
    print(i, x_str, "->", y_str)

    x = [char_dic[c] for c in x_str]
    y = [char_dic[c] for c in y_str]

    dataX.append(x)
    dataY.append(y)

data_dim = len(char_set)
hidden_size = len(char_set)
num_classes = len(char_set)
batch_size = len(dataX)

X = tf.placeholder(tf.int32, [None, sequence_length])
X_oh = tf.one_hot(X, num_classes)
Y = tf.placeholder(tf.int32, [None, sequence_length])

rnn_cell = tf.contrib.rnn.BasicLSTMCell(num_units=hidden_size)
rnn_cell = tf.nn.rnn_cell.MultiRNNCell([rnn_cell] * 2)  # MultiRNNCell은 deprecated되어서 2.0부터는 없어진다.
initial_state = rnn_cell.zero_state(batch_size, tf.float32)

outputs, _states = tf.nn.dynamic_rnn(rnn_cell, X_oh, initial_state=initial_state, dtype=tf.float32)

'''
softmax layer 적용:
[batch_size * sequence_length, hidden_size]로 변경해서 input
output은 [-1, num_classes]로 꺼내고 [batch_size, seq_len, num_classes]로 reshape.
'''
print("applying softmax: outputs=", outputs.shape)
X_for_softmax = tf.reshape(outputs, [-1, hidden_size])
softmax_w = tf.get_variable('softmax_w', [hidden_size, num_classes])
softmax_b = tf.get_variable('softmax_b', [num_classes])
outputs = tf.matmul(X_for_softmax, softmax_w) + softmax_b
outputs = tf.reshape(outputs, [batch_size, sequence_length, num_classes]) # 원상복구

print("outputs.shape:", outputs.shape)
prediction = tf.argmax(outputs, axis=2, output_type=tf.int32)
print("prediction.shape:", prediction.shape)
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
    for i in range(501):
        l, _ = session.run([loss, train], feed_dict={X: dataX, Y: dataY})
        result, acc = session.run([prediction, accuracy], feed_dict={X: dataX, Y: dataY})
        if (i % 100) == 0:
            print(i, "loss:", l, " accuracy:", acc)
            reconstructed = ""
            for j, ret in enumerate(result):
                retstr = ''.join([char_set[c] for c in ret])
                if i is 0:
                    reconstructed += retstr
                else:
                    reconstructed += retstr[-1]
            print("reconstructed:", reconstructed)

    print("final test:")
    result = session.run(prediction, feed_dict={X:dataX})
    results = ""
    for i, ret in enumerate(result):
        result_str = ''.join([char_set[c] for c in ret])
        if i is 0:
            results += result_str
        else:
            results += result_str[-1]
    print("predict:", ''.join(results))
    print("source :", sentence)

