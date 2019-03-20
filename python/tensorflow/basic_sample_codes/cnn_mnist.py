import tensorflow as tf
from tensorflow.examples.tutorials.mnist import input_data
import matplotlib.pyplot as plt
import random

mnist = input_data.read_data_sets("MINST_data/", one_hot=True)

nb_classes = 10
learning_rate = 0.001

# MNIST data image of shape : 28 * 28 = 784
X = tf.placeholder(tf.float32, shape=[None, 784])
X_img = tf.reshape(X, [-1, 28, 28, 1]) #mnist 이미지는 일자형 배열이므로 처리할 shape으로 변경
Y = tf.placeholder(tf.float32, shape=[None, nb_classes])
dropout = tf.placeholder(tf.float32)

def build_model(X, input_channel_count, dropout):
    layer = build_conv_layer(X, 3, 3, inch=input_channel_count, outch=32, dropout=dropout) #=>(14, 14, 32)
    layer = build_conv_layer(layer, 3, 3, inch=32, outch=64, dropout=dropout) #=>(7, 7, 64)
    layer = build_conv_layer(layer, 3, 3, inch=64, outch=128, dropout=dropout)  # =>(4, 4, 128)

    layer = tf.reshape(layer, [-1, 4 * 4 * 128])

    layer = build_fc_layer(layer, 4 * 4 * 128, 625, dropout=dropout, final=False)
    layer = build_fc_layer(layer, 625, nb_classes, dropout=dropout, final=True)
    return layer

def build_conv_layer(input, width, height, inch, outch, dropout):

    W = tf.Variable(tf.random_normal([width, height, inch, outch], stddev=0.01))
    L = tf.nn.conv2d(input, W, strides=[1,1,1,1], padding='SAME')
    L = tf.nn.relu(L)
    '''
    #max_pool에서의 'SAME'의 의미는?
    https://stackoverflow.com/questions/37674306/what-is-the-difference-between-same-and-valid-padding-in-tf-nn-max-pool-of-t
    VALID: stride만큼 이동하다가 stride만큼 남아있지 않아 이동할 수 없으면 남은 것을 버린다.
    SAME: 남은 것이 있으면, 남은 것을 처리할 수 있게 zero padding을 추가한다. 상하좌우 균형을 맞춰서 추가한다.
    '''
    L = tf.nn.max_pool(L, ksize=[1,2,2,1], strides=[1,2,2,1], padding='SAME')
    L = tf.nn.dropout(L, keep_prob=dropout)
    return L

def build_fc_layer(input, inputsize, outputsize, dropout, final):
    wname = "fc_W_{:d}_{:d}".format(inputsize, outputsize)
    W = tf.get_variable(wname, shape=[inputsize, outputsize],
                        initializer=tf.contrib.layers.xavier_initializer())
    b = tf.Variable(tf.random_uniform([outputsize]))
    layer = tf.matmul(input, W) + b
    if not final:
        layer = tf.nn.relu(layer)
        layer = tf.nn.dropout(layer, keep_prob=dropout)
    return layer


logits = build_model(X_img, 1, dropout=dropout)
hypothesis = tf.nn.softmax(logits=logits)
cost = tf.reduce_mean(tf.nn.softmax_cross_entropy_with_logits(logits=logits, labels=Y))
optimizer = tf.train.AdamOptimizer(learning_rate=learning_rate).minimize(cost)

prediction = tf.argmax(hypothesis, axis=1)
is_correct = tf.equal(prediction, tf.argmax(Y, axis=1))
accuracy = tf.reduce_mean(tf.cast(is_correct, tf.float32))

training_epochs = 15
batch_size = 100

with tf.Session() as session:
    session.run(tf.global_variables_initializer())

    # training:
    for epoch in range(training_epochs):
        avg_cost = 0
        total_batch = int(mnist.train.num_examples / batch_size)

        for i in range(total_batch):
            bx, by = mnist.train.next_batch(batch_size)
            c, _ = session.run([cost, optimizer], feed_dict={X:bx, Y:by, dropout:0.7})
            avg_cost += (c / total_batch)

        print("epoch:", epoch, "cost:{:.9f}".format(avg_cost))

    #tensor.eval()함수를 사용해도 된다.
    #print("accuracy : ", session.run(accuracy, feed_dict={X: mnist.test.images, Y: mnist.test.labels}))
    print("accuracy : ", accuracy.eval(session=session, feed_dict={X: mnist.test.images, Y: mnist.test.labels, dropout:1.0}))


    # # Get one and predict
    # r = random.randint(0, mnist.test.num_examples - 1)
    # print("label : ", session.run(tf.argmax(mnist.test.labels[r], axis=0)))
    # '''
    # 여기 원래 [r:r+1] 이렇게 되어 있는데, [784]인 것을
    # reshape을 사용해서 명시적으로 [1,784]로 변경하는것이 더 나아 보인다.
    # '''
    # print("prediction:", session.run(prediction, feed_dict={X:mnist.test.images[r].reshape(1, 784)}))
    #
    # plt.imshow(mnist.test.images[r:r+1].reshape(28, 28)) #width/height 2차원 배열로 변경해준다.
    # plt.show()