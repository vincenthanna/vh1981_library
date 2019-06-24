import face_model
#import tensorflow as tf
import keras.backend as K
from face_model import face_recognition_model_layer

from dataset import prepare_data
from dataset import get_batch

labels, imgpaths = prepare_data()

# model = face_recognition_model_layer(input_shape=(200, 200, 3))






