import tensorflow as tf
import random
import numpy as np

import pandas as pd

train_imgs = pd.read_csv("./input/train_images_mnist.csv")
train_labels = pd.read_csv("./input/train_labels_mnist.csv")
test_imgs = pd.read_csv("./input/test_images_mnist.csv")
test_labels = pd.read_csv("./input/test_labels_mnist.csv")


def reorganizeMNIST(x, y):
    assert x.shape[0] == y.shape[0]

    dataset = {i: [] for i in range(10)}

    for i in range(x.shape[0]):
        dataset[y[i]].append(x[i])

    return dataset


def get_batch(dataset, k):
    # Sample BATCH_K random images from each category of the MNIST dataset,
    # returning the data along with its labels
    batch = []
    labels = []

    for l in range(10):
        indices = random.sample(range(len(dataset[l])), k)
        indices = np.array(indices)

        batch.append([dataset[l][i] for i in indices])
        labels += [l] * k

    batch = np.array(batch).reshape(10 * k, 28, 28, 1)
    labels = np.array(labels)

    # Shuffling labels and batch the same way
    s = np.arange(batch.shape[0])
    np.random.shuffle(s)

    batch = batch[s]
    labels = labels[s]

    return batch, labels


train_set = reorganizeMNIST(train_imgs.values, train_labels.values.reshape(-1))
valid_set = reorganizeMNIST(test_imgs.values, test_labels.values.reshape(-1))

# print(train_set)
# print(valid_set)


def all_diffs(a, b):
    # Returns a tensor of all combinations of a - b
    print("shapes:", a.shape, b.shape)
    return tf.expand_dims(a, axis=1) - tf.expand_dims(b, axis=0)


def euclidean_dist(embed1, embed2):
    # Measures the euclidean dist between all samples in embed1 and embed2

    diffs = all_diffs(embed1, embed2)  # get a square matrix of all diffs
    print("diffs.shape=", diffs.shape)
    return tf.sqrt(tf.reduce_sum(tf.square(diffs), axis=-1) + 1e-12)


TL_MARGIN = 0.2  # The minimum distance margin


def bh_triplet_loss(dists, labels):
    """
    입력된 이미지들에 대해 각각,
        furthest_positive : 가장 큰 차이를 지닌 같은값
        closest_negative : 가장 작은 차이를 지닌 다른값
    을 구하고 furthest_positive - closest_negative 값을 반환

    """
    
    # Defines the "batch hard" triplet loss function.
    print("labels.shape", labels.shape)
    same_identity_mask = tf.equal(tf.expand_dims(labels, axis=1),
                                  tf.expand_dims(labels, axis=0))
    print("same_identity_mask.shape=", same_identity_mask.shape)
    aaa = tf.eye(tf.shape(labels)[0], dtype=tf.bool)
    print("tf.eye.shape=", aaa.shape)
    negative_mask = tf.logical_not(same_identity_mask)
    positive_mask = tf.logical_xor(same_identity_mask,
                                   tf.eye(tf.shape(labels)[0], dtype=tf.bool))

    print("masks.shape : ", negative_mask.shape, positive_mask.shape) #[None, None], [None, None]

    # 곱해서 차원은 변경없고(nxn행렬이므로), 
    furthest_positive = tf.reduce_max(dists * tf.cast(positive_mask, tf.float32), axis=1)
    """
    map_fn : map on the list of tensors unpacked from elems on dimension 0.
    (dists, negative_mask)에서 element하나씩 뽑아서 lambda식에 적용한다.
    =>한 행씩 masking해서 negative값만 남기고 가장작은 값만 추린다.
    [None,]의 1차원 값 배열만 남는다.(furthest_positive도 reduce_max로 동일.)
    의미 : 각각의 image에 대해 다른 이미지들 중 값이 가장 작은 값만 남긴다.
    """
    closest_negative = tf.map_fn(lambda x: tf.reduce_min(tf.boolean_mask(x[0], x[1])),
                                 (dists, negative_mask), tf.float32)

    print("f/c shapes:", furthest_positive.shape, closest_negative.shape) #[None,], [None,]
    diff = furthest_positive - closest_negative

    return tf.maximum(diff + TL_MARGIN, 0.0)


EMBEDDING_DIM = 4  # Size of the embedding dimension (units in the last layer)
def embedImages(Images):
    conv1 = tf.layers.conv2d(Images,
                             filters=128, kernel_size=(7, 7),
                             padding='same',
                             activation=tf.nn.relu,
                             kernel_initializer=tf.truncated_normal_initializer,
                             name='conv1')

    pool1 = tf.layers.max_pooling2d(conv1,
                                    pool_size=(2, 2), strides=(2, 2),
                                    padding='same',
                                    name='pool1')

    conv2 = tf.layers.conv2d(pool1,
                             filters=256, kernel_size=(5, 5),
                             padding='same',
                             activation=tf.nn.relu,
                             kernel_initializer=tf.truncated_normal_initializer,
                             name='conv2')

    pool2 = tf.layers.max_pooling2d(conv2,
                                    pool_size=(2, 2), strides=(2, 2),
                                    padding='same',
                                    name='pool2')

    flat = tf.layers.flatten(pool2, name='flatten')

    # Linear activated embeddings
    embeddings = tf.layers.dense(flat,
                                 activation=None,
                                 kernel_initializer=tf.truncated_normal_initializer,
                                 units=EMBEDDING_DIM,
                                 name='embeddings')

    return embeddings


# Placeholders for inserting data
Images = tf.placeholder(tf.float32, [None, 28, 28, 1], name='images_ph')
Labels = tf.placeholder(tf.int32, [None], name='labels_ph')

# Embeds images using the defined model
embedded_images = embedImages(Images)
print("embedded_images.shape=", embedded_images.shape) #[None, 4]

# Measure distance between al embeddings
dists = euclidean_dist(embedded_images, embedded_images)
print("dists.shape : ", dists.shape)

# Calculate triplet loss for the give dists
print("triplet_loss shape=", bh_triplet_loss(dists, Labels).shape) # [None,] 1차원
loss = tf.reduce_mean(bh_triplet_loss(dists, Labels))
print("loss.shape=", loss.shape)

global_step = tf.Variable(0, trainable=False, name='global_step')
learning_rate = tf.train.exponential_decay(0.001, global_step, 5000, 0.96, staircase=True)

optimizer = tf.train.AdamOptimizer(learning_rate)
train_step = optimizer.minimize(loss=loss, global_step=global_step)

with tf.Session() as sess:
    tf.global_variables_initializer().run()

    loss_hist = []
    lr_hist = []

    epochs = 10
    # Train for epochs
    for i in range(epochs):
        data, labels = get_batch(train_set, 8)

        feed_dict = {Images: data, Labels: labels}

        _, lr, raw_loss, embeddings = sess.run([train_step,
                                                optimizer._lr, loss, embedded_images], feed_dict)
        print("raw_loss : ", raw_loss)

        lr_hist.append(lr)
        loss_hist.append(raw_loss)

    # Training is finished, get a batch from training and validation
    # data to visualize the results
    x_train, y_train = get_batch(train_set, 32)
    x_val, y_val = get_batch(valid_set, 32)

    # Embed the images using the network
    train_embeds = sess.run(embedded_images,
                            feed_dict={Images: x_train, Labels: y_train})
    val_embeds = sess.run(embedded_images,
                          feed_dict={Images: x_val, Labels: y_val})

    # tsne_train = tsne.fit_transform(train_embeds)
    # tsne_val = tsne.fit_transform(val_embeds)
    #
    # scatter(tsne_train, y_train, "Results on Training Data")
    # scatter(tsne_val, y_val, "Results on Validation Data")