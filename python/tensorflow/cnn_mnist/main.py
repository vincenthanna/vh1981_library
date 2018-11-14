import tensorflow as tf
import struct
import numpy as np
import matplotlib.pyplot as plt

def load_mnists():
    trainSet_label_filename = "train-labels.idx1-ubyte"
    trainSet_image_filename = "train-images.idx3-ubyte"

    testSet_label_filename = "t10k-labels.idx1-ubyte"
    testSet_image_filename = "t10k-images.idx3-ubyte"


    trainSet_labels, trainSet_images = load_mnist_set(trainSet_label_filename, trainSet_image_filename)
    testSet_labels, testSet_images = load_mnist_set(testSet_label_filename, testSet_image_filename)
    return trainSet_images, trainSet_labels, testSet_images, testSet_labels

def one_hot(value, classes):
    return np.eye(classes)[value]

def load_mnist_set(labelfilename, imagefilename):
    labels = []
    images = []
    num_items = 0
    row = 0
    col = 0
    with open(labelfilename, "rb") as f:
        magic_number = struct.unpack('>i', f.read(4))  # 꺽쇠방향은 big/little endian 구분용 < : little, > : big
        # 아래 링크 참조
        # https://stackoverflow.com/questions/3783677/how-to-read-integers-from-a-file-that-are-24bit-and-little-endian-using-python
        magic_number = magic_number[0]
        num_items = struct.unpack('>i', f.read(4))[0]

        for i in range(num_items):
            # label = struct.unpack('b', f.read(1))
            label = struct.unpack('b', f.read(1))
            label_one_hot = one_hot(label, 10)

            #print("label : ", label, " one_hot : ", label_one_hot)
            labels.append(label_one_hot)

        print("magic_number : ", format(magic_number, '08x'))
        print("items : ", num_items)

    with open(imagefilename, "rb") as f:
        magic_number = struct.unpack('>i', f.read(4))[0]  # 꺽쇠방향은 big/little endian 구분용 < : little, > : big
        num_items = struct.unpack('>i', f.read(4))[0]
        row = struct.unpack('>i', f.read(4))[0]
        col = struct.unpack('>i', f.read(4))[0]
        print("magic:", magic_number, " items:", num_items, " (", row, ",", col, ")")

        for i in range(num_items):
            image = bytearray(f.read(28 * 28))
            if i == 1 :
                print("image len", len(image))
            images.append(image)
            #print("images len", len(images))
        print("images len : ", len(images))
        # print("img 0", len(images[0]), " ", images[0])
        # print("img 1", len(images[1]), " ", images[1])
        # print("img 2", len(images[2]), " ", images[2])

        '''
        plt.title(str(labels[0]))
        imgs = np.array(images)
        imgs = imgs.reshape(num_items, col, row)
        print(imgs[0].shape)

        for i in range(0, 4):
            plt.subplot(4, 1, i + 1)
            plt.title(str(labels[i]))
            plt.imshow(imgs[i])


        # plt.subplot(4, 1, 2)
        # plt.imshow(imgs[1])
        # plt.subplot(4, 1, 3)
        # plt.imshow(imgs[2])
        # plt.subplot(4, 1, 4)
        # plt.imshow(imgs[3])

        plt.show()
        '''

    #return labels, images
    return np.array(labels), np.array(images).reshape((num_items, col * row))

trainX, trainY, testX, testY, = load_mnists()
print("read data : ")
print("trainX : ", trainX.shape)
print("trainY : ", trainY.shape)
print("testX  : ", testX.shape)
print("testY  : ", testY.shape)

num_train = trainX.shape[0]
num_test = testX.shape[0]

print("num_train : ", num_train, " num_test : ", num_test)

batch_size = 100
total_batch = int(num_train / batch_size)

def get_batch(data, batch_size, idx):
    ret = data[batch_size*idx:batch_size*(idx+1),]
    return ret

for epoch in range(1):
    total_cost = 0

    for i in range(total_batch):
        batch_xs = get_batch(trainX, batch_size, i)
        batch_ys = get_batch(trainY, batch_size, i)
        print("batch ", str(i), " : ", batch_xs.shape, batch_ys.shape)
