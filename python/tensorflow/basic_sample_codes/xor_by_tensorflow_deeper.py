import tensorflow as tf
import numpy as np

x_data = [[0, 0], [0, 1], [1, 0], [1, 1]]
y_data = [[0], [1], [1], [0]]

input_cnt = 4

def build_layer(layer_id, input, inputsize, outputsize, last):
    layername = "layer_{:d}".format(layer_id)
    with tf.name_scope(layername) as scope:
        wname = "{}_w".format(layername)
        bname = "{}_b".format(layername)
        W = tf.Variable(tf.random_uniform([inputsize, outputsize], -1.0, 1.0, seed=1),
                        dtype=tf.float32, name=wname)
        b = tf.Variable(tf.random_uniform([outputsize], -1.0, 1.0, seed=1),
                        dtype=tf.float32, name=bname)

        # tensorboard summary:
        tb_wname = "tb_{}".format(wname)
        tb_bname = "tb_{}".format(bname)
        print("tb_wname:", tb_wname)
        print("tb_bname:", tb_bname)
        tf.summary.histogram(tb_wname, W)
        tf.summary.histogram(tb_bname, b)

        if last == False:
            layer = tf.nn.relu(tf.matmul(input, W) + b)
        else:
            layer = tf.sigmoid(tf.matmul(input, W) + b)

        return layer

def build_model(input, inputsize, outputsize):
    l1 = build_layer(1, input, inputsize, 5, False)
    l2 = build_layer(2, l1, 5, 5, False)
    l3 = build_layer(3, l2, 5, 5, False)
    l4 = build_layer(4, l3, 5, 5, False)
    l5 = build_layer(5, l4, 5, 5, False)
    l6 = build_layer(6, l5, 5, 5, False)
    l7 = build_layer(7, l6, 5, 5, False)
    l8 = build_layer(8, l7, 5, 5, False)
    l9 = build_layer(9, l8, 5, 5, False)
    l10 = build_layer(10, l9, 5, 5, False)

    last = build_layer(10, l10, 5, outputsize, True)

    output = last
    return output


X = tf.placeholder(tf.float32, name="X", shape=[input_cnt, 2])
Y = tf.placeholder(tf.float32, name="Y", shape=[input_cnt, 1])

# my1 = tf.sigmoid(tf.matmul(X, W1) + b1)
# my2 = tf.sigmoid(tf.matmul(my1, W2) + b2)
# my3 = tf.sigmoid(tf.matmul(my2, W3) + b3)
# hypothesis = tf.sigmoid(tf.matmul(my3, W4) + b4)

hypothesis = build_model(X, 2, 1)
# print("hypothesis.shape:", hypothesis.shape)  # (4, 1)

cost = -tf.reduce_mean(Y * tf.log(hypothesis) + (1 - Y) * tf.log(1 - hypothesis))
train = tf.train.GradientDescentOptimizer(learning_rate=0.1).minimize(cost)

'''
building tensorboard summary
'''
# w1_hist = tf.summary.histogram('weights01', W1)
# w2_hist = tf.summary.histogram('weights02', W2)
# w3_hist = tf.summary.histogram('weights03', W3)
# w4_hist = tf.summary.histogram('weights04', W4)
# tf.summary.histogram('b1', b1)
# tf.summary.histogram('b2', b2)
# tf.summary.histogram('b3', b3)
# tf.summary.histogram('b4', b4)
cost_sum = tf.summary.scalar('cost', cost)
summary = tf.summary.merge_all()
summary_step = 0


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
            print(step, "cost:", session.run([cost], feed_dict={X: x_data, Y: y_data}))
            # h, c, a = session.run([hypothesis, predicted, accuracy], feed_dict={X: x_data, Y: y_data})
            # print("hypothesis:", h, "correct:", c, "accuracy:", a)

    # 최종 결과
    h, c, a = session.run([hypothesis, predicted, accuracy], feed_dict={X: x_data, Y: y_data})
    print("hypothesis:", h, "correct:", c, "accuracy:", a)


