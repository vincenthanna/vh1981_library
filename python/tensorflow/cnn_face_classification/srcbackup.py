

def load_data():
    xtrain = []
    ttrain = []
    xtest = []
    ttest = []

    #fig = plt.figure(figsize=(10, 10))

    for index, imgdir in enumerate(imageDirs):

        print(index, imgdir)
        imageDirPath = os.path.join(imgdir, "cropped")
        files = [f for f in os.listdir(imageDirPath)
                 if os.path.isfile(os.path.join(imageDirPath, f)) and f.find("jpeg") > 0]
        label = index

        totalCnt = len(files)
        print("label ", label, " cnt=", totalCnt)

        trainCnt = totalCnt * 3 / 4

        cnt = 0

        for f in files:
            imageFilePath = os.path.join(imageDirPath, f)
            image = get_img(imageFilePath)

            #label을 one-hot으로 변경
            onehot = label_to_onehot(label, depth=categoryCnt)
            image = np.reshape(image, [96, 96, 3])

            # if (cnt < 100):
            #     fig.add_subplot(10, 10, cnt + 1)
            #     plt.imshow(image)

            if cnt < trainCnt:
                xtrain.append(image)
                ttrain.append(onehot)

            else:
                xtest.append(image)
                ttest.append(onehot)

            cnt += 1

            # print("X_train, X_test, t_train ,t_test ", X_train.shape, X_test.shape,
            #       t_train.shape, t_test.shape)

    print("total xtrain = ", len(xtrain))
    global X_train
    global t_train
    global X_test
    global t_test

    X_train = np.asarray(xtrain)
    t_train = np.asarray(ttrain)
    X_test = np.asarray(xtest)
    t_test = np.asarray(ttest)
    print("load_data() :")
    print("        X_train : ", X_train.shape)
    print("        t_train : ", t_train.shape)
    print("        X_test  : ", X_test.shape)
    print("        t_test  : ", t_test.shape)

    p = np.random.permutation(len(X_train))
    X_train = X_train[p]
    t_train = t_train[p]

    fig = plt.figure(figsize=(10, 10))
    for i in range(100):
        fig.add_subplot(10, 10, i + 1)
        plt.imshow(X_train[i])
        plt.title(imageDirs[onehot_to_label(t_train[i])])


    print("t_train[0]", t_train[0].shape, t_train[0])

    plt.show()



def build_layer(prev, ksize, pdepth, ndepth):
    W = tf.Variable(tf.random_normal([ksize, ksize, pdepth, ndepth], stddev=0.01))
    L = tf.nn.conv2d(prev, W, strides=[1, 1, 1, 1], padding='SAME')
    L = tf.nn.relu(L)
    return L

def build_model_2():
    global keep_prob

    L1 = build_layer(X, 7, 3, 32)
    L2 = build_layer(L1, 6, 32, 64)

    # 96 X 96 X 64 => 48 X 48 X 64
    L3 = tf.nn.max_pool(L2, ksize=[1, 2, 2, 1], strides=[1, 2, 2, 1], padding='SAME')

    L4 = build_layer(L3, 5, 64, 128)
    L5 = build_layer(L4, 4, 128, 256)

    # 48 X 48 X 256 => 24 X 24 X 256
    L6 = tf.nn.max_pool(L5, ksize=[1, 2, 2, 1], strides=[1, 2, 2, 1], padding='SAME')

    L7 = build_layer(L6, 3, 256, 512)
    L8 = build_layer(L7, 2, 512, 1024)

    # 24 => 12
    L9 = tf.nn.max_pool(L8, ksize=[1, 2, 2, 1], strides=[1, 2, 2, 1], padding='SAME')
    L10 = build_layer(L9, 1, 1024, 2048)

    # 12 => 6
    L11 = tf.nn.max_pool(L10, ksize=[1, 2, 2, 1], strides=[1, 2, 2, 1], padding='SAME')

    # fully connected layer
    W12 = tf.Variable(tf.random_normal([6 * 6 * 2048, 8192], stddev=0.01))
    L12 = tf.reshape(L11, [-1, 6 * 6 * 2048])
    L12 = tf.matmul(L12, W12)
    L12 = tf.nn.relu(L12)
    L12 = tf.nn.dropout(L12, keep_prob)

    W13 = tf.Variable(tf.random_normal([8192, 2048], stddev=0.01))
    L13 = tf.reshape(L12, [-1, 8192])
    L13 = tf.matmul(L13, W13)
    L13 = tf.nn.relu(L13)
    L13 = tf.nn.dropout(L13, keep_prob)

    W14 = tf.Variable(tf.random_normal([2048, 512], stddev=0.01))
    L14 = tf.reshape(L13, [-1, 2048])
    L14 = tf.matmul(L14, W14)
    L14 = tf.nn.relu(L14)
    L14 = tf.nn.dropout(L14, keep_prob)

    W15 = tf.Variable(tf.random_normal([512, 256], stddev=0.01))
    L15 = tf.reshape(L14, [-1, 512])
    L15 = tf.matmul(L15, W15)
    L15 = tf.nn.relu(L15)
    L15 = tf.nn.dropout(L15, keep_prob)

    W16 = tf.Variable(tf.random_normal([256, categoryCnt], stddev=0.01))
    model = tf.matmul(L15, W16)

    return model

def build_fclayer(prevLayer, inputsize, outputsize):
    global keep_prob
    W = tf.Variable(tf.random_normal([inputsize, outputsize], stddev=0.01))
    L = tf.reshape(prevLayer, [-1, inputsize])
    L = tf.matmul(L, W)
    L = tf.nn.relu(L)
    L = tf.nn.dropout(L, keep_prob)
    return L

def build_model():
    global keep_prob

    '''
    1st layer
    input : 96 X 96 X 3
    '''
    W1 = tf.Variable(tf.random_normal([5, 5, 3, 32], stddev=0.01))
    L1 = tf.nn.conv2d(X, W1, strides=[1, 1, 1, 1], padding='SAME')
    L1 = tf.nn.relu(L1)

    '''
    1st pooling layer
    input : 96 X 96 X 32
    output : 48 X 48 X 32
    '''
    L1 = tf.nn.max_pool(L1, ksize=[1, 2, 2, 1], strides=[1, 2, 2, 1], padding='SAME')

    '''
    2nd layer
    input : 48 X 48 X 32
    output : 48 X 48 X 64
    '''
    W2 = tf.Variable(tf.random_normal([3, 3, 32, 64], stddev=0.01))
    L2 = tf.nn.conv2d(L1, W2, strides=[1, 1, 1, 1], padding='SAME')
    L2 = tf.nn.relu(L2)

    '''
    2nd pooling layer
    input : 48 X 48 X 64
    output : 16 X 16 X 64
    '''
    L2 = tf.nn.max_pool(L2, ksize=[1,3,3,1], strides=[1,3,3,1], padding='SAME')

    # fully connected layer
    # W3 = tf.Variable(tf.random_normal([16 * 16 * 64, 256], stddev=0.01))
    # L3 = tf.reshape(L2, [-1, 16 * 16 * 64])
    # L3 = tf.matmul(L3, W3)
    # L3 = tf.nn.relu(L3)
    # L3 = tf.nn.dropout(L3, keep_prob)
    L3 = build_fclayer(L2, 16 * 16 * 64, 256)
    L4 = build_fclayer(L3, 256, 128)
    L5 = build_fclayer(L4, 128, 64)
    L6 = build_fclayer(L5, 64, 32)

    # W4 = tf.Variable(tf.random_normal([256, 128], stddev=0.01))
    # L4 = tf.reshape(L3, [-1, 256])
    # L4 = tf.matmul(L4, W4)
    # L4 = tf.nn.relu(L4)
    # L4 = tf.nn.dropout(L4, keep_prob)

    lastW = tf.Variable(tf.random_normal([32, categoryCnt], stddev=0.01))
    model = tf.matmul(L6, lastW)

    return model
