import tensorflow as tf
from tensorflow.examples.tutorials.mnist import input_data
import matplotlib.pyplot as plt
import random

def build_logit(X, inputsize, outputsize, dropout):
    '''
    :param X: input tensor
    :param inputsize: input size
    :param outputsize: output size
    :return: model
    '''

    l1 = build_layer(X, inputsize, 256, False, dropout)
    l2 = build_layer(l1, 256, 256, False, dropout)
    l3 = build_layer(l2, 256, outputsize, True, dropout)
    output_layer = l3

    return output_layer

def build_layer(input, inputsize, outputsize, last, dropout):
    '''
    :param input: input layer
    :param inputsize: input width
    :param outputsize: output width
    :return: layer
    '''
    layer_name = "layer_{:d}_{:d}".format(inputsize, outputsize)
    with tf.name_scope(layer_name) as scope:
        wname = "{}_w".format(layer_name)
        bname = "{}_b".format(layer_name)
        print(layer_name, wname, bname)
        #W = tf.Variable(tf.random_uniform([inputsize, outputsize]), name=wname)
        W = tf.get_variable(wname, shape=[inputsize, outputsize], initializer=tf.contrib.layers.xavier_initializer())
        b = tf.Variable(tf.random_uniform([outputsize]), name=bname)

        # tensorboard summary:
        tf.summary.histogram(wname, W)
        tf.summary.histogram(bname, b)

        '''
        지금 상황에서 layer를 쌓아도 기존 단일 softmax보다 성능이 잘 나오지 않는다.
        multi-layer 성능을 올리려면 나중에 나올 relu/adam 사용해야 95%정도까지 올라감
        '''
        output = tf.matmul(input, W) + b
        if last == False:
            output = tf.nn.relu(output)
            output = tf.nn.dropout(output, keep_prob=dropout)
        return output

mnist = input_data.read_data_sets("MINST_data/", one_hot=True)

nb_classes = 10

# MNIST data image of shape : 28 * 28 = 784
X = tf.placeholder(tf.float32, shape=[None, 784])
Y = tf.placeholder(tf.float32, shape=[None, nb_classes])
keep_prob = tf.placeholder(tf.float32)

logits = build_logit(X, 784, nb_classes, keep_prob)
hypothesis = tf.nn.softmax(logits=logits)
cost = tf.reduce_mean(tf.nn.softmax_cross_entropy_with_logits(logits=logits, labels=Y))
# 여러 레이어를 쌓은 경우에는 기존 GD로는 성능이 잘 나오지 않아서 Adam으로 교체
optimizer = tf.train.AdamOptimizer(learning_rate=0.001).minimize(cost)

prediction = tf.argmax(hypothesis, axis=1)
is_correct = tf.equal(prediction, tf.argmax(Y, axis=1))
accuracy = tf.reduce_mean(tf.cast(is_correct, tf.float32))

#tensorboard summary:
tf.summary.scalar("cost", cost)
tb_summary = tf.summary.merge_all()
summary_step = 0

training_epochs = 15
batch_size = 100

with tf.Session() as session:
    session.run(tf.global_variables_initializer())

    # create writer and add graph:
    writer = tf.summary.FileWriter("./logs_mnist_deeper_tensorboard")
    writer.add_graph(session.graph)

    # training:
    for epoch in range(training_epochs):
        avg_cost = 0
        total_batch = int(mnist.train.num_examples / batch_size)

        for i in range(total_batch):
            bx, by = mnist.train.next_batch(batch_size)
            c, _, summary= session.run([cost, optimizer, tb_summary], feed_dict={X:bx, Y:by, keep_prob:0.7})

            # summary = session.run([tb_summary], feed_dict={X:bx, Y:by, keep_prob:0.7})

            # tensorboard summary write:
            writer.add_summary(summary, global_step=summary_step)
            summary_step += 1

            avg_cost += (c / total_batch)

        print("epoch:", epoch, "cost:{:.9f}".format(avg_cost))

    #tensor.eval()함수를 사용해도 된다.
    #print("accuracy : ", session.run(accuracy, feed_dict={X: mnist.test.images, Y: mnist.test.labels}))
    print("accuracy : ", accuracy.eval(session=session, feed_dict={X: mnist.test.images, Y: mnist.test.labels, keep_prob:1.0}))

    # Get one and predict
    r = random.randint(0, mnist.test.num_examples - 1)
    print("label : ", session.run(tf.argmax(mnist.test.labels[r], axis=0)))
    '''
    여기 원래 [r:r+1] 이렇게 되어 있는데, [784]인 것을 
    reshape을 사용해서 명시적으로 [1,784]로 변경하는것이 더 나아 보인다.
    '''
    print("prediction:", session.run(prediction, feed_dict={X:mnist.test.images[r].reshape(1, 784), keep_prob:1.0}))

    plt.imshow(mnist.test.images[r:r+1].reshape(28, 28)) #width/height 2차원 배열로 변경해준다.
    plt.show()

