import face_model
import tensorflow as tf

def triplet_loss_func(y_pred, alpha=0.2):
    """triplet loss 구현 함수

    Arguments:
        y_pred : anchor, positive, negative vector 리스트
            anchor : 입력 이미지 vector
            positive : anchor와 동일인 이미지 vector
            negative : anchor와 다른사람 이미지 vector

    Returns:
        loss 값.
    """

    anchor, pos, neg = y_pred[0], y_pred[1], y_pred[2]

    print(anchor.shape)

    pos_dist = tf.reduce_sum(tf.square(anchor - pos), axis=-1)
    neg_dist = tf.reduce_sum(tf.square(anchor - neg), axis=-1)
    # pos_dist가 neg_dist보다 alpha 이상 작으면 loss값이 0이 된다.
    loss = pos_dist - neg_dist + alpha
    loss = tf.reduce_sum(tf.maximum(loss, 0.0))


    return loss
