import face_model
import tensorflow as tf
from face_model import face_recognition_model, euclidean_dist, triplet_loss

from dataset import prepare_data
from dataset import get_batch

labels, imgpaths = prepare_data()
batch_size = 32

imgs = get_batch(imgpaths, batch_size, 0, isImgPath=True)
input_shape = imgs.shape[1:]


hypothesis = face_recognition_model(imgs)

dists = euclidean_dist(hypothesis, hypothesis)
loss = tf.reduce_mean(triplet_loss(dists, labels))

learning_rate = 0.001
optimizer = tf.train.AdamOptimizer(learning_rate)

epochs = 1
batch_size = 32
total_data_cnt = imgpaths.shape[0]

with tf.Session() as session:
    session.run(tf.global_variables_initializer())
    for epoch in range(epochs):
        total_batch = int(total_data_cnt / batch_size)
        for i in range(total_batch):
            labels = get_batch(labels, batch_size=batch_size, idx=i)
            images = get_batch(imgpaths, batch_size=batch_size, idx=i, isImgPath=True)

            session.run([optimizer])





# model = face_recognition_model_layer(input_shape=(200, 200, 3))






