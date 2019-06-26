import face_model
import tensorflow as tf
from face_model import face_recognition_model, euclidean_dist, triplet_loss

from dataset import prepare_data
from dataset import get_batch

total_labels, total_imgpaths = prepare_data()
batch_size = 32

imgs = get_batch(total_imgpaths, batch_size, 0, isImgPath=True)
input_shape = imgs.shape[1:]
width, height, channels = input_shape



# placeholders :
tImages = tf.placeholder(tf.float32, [None, width, height, channels], name="images_tensor")
tLabels = tf.placeholder(tf.int32, [None], name="labels_tensor")

hypothesis = face_recognition_model(tImages)

dists = euclidean_dist(hypothesis, hypothesis)
cost = tf.reduce_mean(triplet_loss(dists, tLabels))


global_step = tf.Variable(0, trainable=False, name='global_step')
learning_rate = tf.train.exponential_decay(0.001, global_step, 5000, 0.96, staircase=True)

optimizer = tf.train.AdamOptimizer(learning_rate).minimize(loss=cost, global_step=global_step)


epochs = 5
batch_size = 400
total_data_cnt = len(total_imgpaths)

with tf.Session() as session:
    session.run(tf.global_variables_initializer())
    for epoch in range(epochs):
        total_batch = int(total_data_cnt / batch_size)
        for i in range(total_batch):
            labels = get_batch(total_labels, batch_size=batch_size, idx=i)
            images = get_batch(total_imgpaths, batch_size=batch_size, idx=i, isImgPath=True)
            c, _ = session.run([cost, optimizer], feed_dict={tImages:images, tLabels:labels})
            print(c.shape)
            print("batch", i, "cost=", c)





# model = face_recognition_model_layer(input_shape=(200, 200, 3))






