def label_to_onehot(label, depth):
    onehot = np.zeros((depth), float)
    onehot[label] = 1
    return onehot

def onehot_to_label(onehot):
    return np.argmax(onehot)

def get_img(imageFilePath):
    image = Image.open(imageFilePath)
    image = image.resize((96,96))
    image = np.asarray(image)

    #print("image.shape=", image.shape)

    return image

# 여러 이미지를 단체로 출력해야한다면...
fig = plt.figure(figsize=(10, 10))
for i in range(100):
    fig.add_subplot(10, 10, i + 1)
    img = get_img(xtrain[i])
    plt.imshow(img)
    plt.title(imageDirs[ttrain[i]])

plt.show()