import face_model
import tensorflow as tf


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


def triplet_loss(dists, labels):
    # Defines the "batch hard" triplet loss function.
    print("labels.shape", labels)
    same_identity_mask = tf.equal(tf.expand_dims(labels, axis=1),
                                  tf.expand_dims(labels, axis=0))
    print("same_identity_mask.shape=", same_identity_mask.shape)
    aaa = tf.eye(tf.shape(labels)[0], dtype=tf.bool)
    print("tf.eye.shape=", aaa.shape)
    negative_mask = tf.logical_not(same_identity_mask)
    positive_mask = tf.logical_xor(same_identity_mask,
                                   tf.eye(tf.shape(labels)[0], dtype=tf.bool))

    furthest_positive = tf.reduce_max(dists * tf.cast(positive_mask, tf.float32), axis=1)
    closest_negative = tf.map_fn(lambda x: tf.reduce_min(tf.boolean_mask(x[0], x[1])),
                                 (dists, negative_mask), tf.float32)

    diff = furthest_positive - closest_negative

    return tf.maximum(diff + TL_MARGIN, 0.0)