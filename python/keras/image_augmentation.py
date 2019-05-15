from sklearn import model_selection
from keras import datasets

import keras
from keras.preprocessing.image import load_img, img_to_array

source_img = load_img('dog.jpg')
x = img_to_array(source_img)
w, h, c = x.shape
x = x.reshape(1, w, h, c)
print(x.shape)


from keras.preprocessing.image import ImageDataGenerator
'''
https://keras.io/preprocessing/image/#imagedatagenerator-methods
'''
generator = ImageDataGenerator(
        rotation_range=15,
        rescale=1.0/255,
        shear_range=0.2,
        zoom_range=0.2,
        horizontal_flip=True)

'''
Fits the data generator to some sample data.
Only required if featurewise_center or featurewise_std_normalization or zca_whitening are set to True.
'''
generator.fit(x, seed=487)

imgsCnt = 0
for batch in generator.flow(x, batch_size=1, save_to_dir='preview', save_prefix='modified', save_format='jpeg'):
    imgsCnt += 1
    if imgsCnt > 20:
        break

