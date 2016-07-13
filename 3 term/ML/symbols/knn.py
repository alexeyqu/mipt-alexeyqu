import numpy
from collections import Counter
import math

"""
################################################################################
#><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><#
#....................................MATRIX....................................#
#><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><#
################################################################################
"""

class MatrixBasedKNearestNeighbor(object):
    """
        Matrix-based KNN classifier with L2 distance
    """

    def __init__(self, k, num_loops=0):
        self.dist_mtx = None
        self.num_loops = num_loops
        self.k = k
        self.size = 28
        self.black = 255
        self.white = 0

        self.G = None
        self.Gsqrt = None# numpy.zeros(self.size * self.size * self.size * self.size).reshape(self.size, self.size, self.size, self.size)

    def fit(self, X_train, y_train):
        """
            Only save the training data.
        """
        self.X_train = X_train
        self.y_train = y_train
        return self

    def binarize(self, image, black_border):
        """
            Technical: binarize image
        """
        for row in range(self.size):
            for col in range(self.size):
                if image[row][col] > black_border:
                    image[row][col] = self.black
                else:
                    image[row][col] = self.white
        return image

    def center_image(self, image):
        """
            Technical: center image
        """
        center_coord = {"row": 0, "col": 0}
        total_paint = 0
        for row in range(self.size):
            for col in range(self.size):
                center_coord["row"] += row * image[row][col]
                center_coord["col"] += col * image[row][col]
                total_paint += image[row][col]

        center_coord["row"] = math.floor(center_coord["row"] / total_paint)
        center_coord["col"] = math.floor(center_coord["col"] / total_paint)

        new_image = numpy.zeros(self.size * self.size)
        new_image = new_image.reshape(self.size, self.size)

        for row in range(self.size):
            for col in range(self.size):
                if row + center_coord["row"] - self.size / 2 in range(self.size) and \
                                                col + center_coord["col"] - self.size / 2 in range(self.size):
                    new_image[row][col] = image[row + center_coord["row"] - self.size / 2][
                        col + center_coord["col"] - self.size / 2]
                else:
                    new_image[row][col] = 0
        return new_image

    def init_G(self, G = None):
        """
            Technical: calculate G and its decomposition
        """
        from scipy import linalg

        if G != None:
            self.G = numpy.zeros(self.size * self.size * self.size * self.size).reshape(self.size, self.size, self.size, self.size)
            self.G = G
        else:
            self.G = numpy.zeros(self.size * self.size * self.size * self.size).reshape(self.size, self.size, self.size, self.size)

            sigma_squared = math.pow(1.005, 2)

            # fill G
            for row_i in range(self.size):
                for col_i in range(self.size):
                    for row_j in range(self.size):
                        for col_j in range(self.size):
                            self.G[row_i][col_i][row_j][col_j] = math.exp(((row_i - row_j) * (row_i - row_j) + (col_i - col_j) * (col_i - col_j))/(-2*sigma_squared))

        self.G = self.G.reshape(self.size * self.size, self.size * self.size)

        # open("log.txt", "w").write(self.G)

        self.Gsqrt = linalg.sqrtm(numpy.asarray(self.G)).real
        #
        # print(self.G[0:10][0:10])
        # print(self.Gsqrt.dot(self.Gsqrt)[0:10][0:10])
        #
        # print(numpy.allclose(self.G, self.Gsqrt.dot(self.Gsqrt)))
        #
        # print(self.Gsqrt)

        self.Gsqrt = self.Gsqrt.reshape(self.size * self.size, self.size * self.size)

        return self

    def ST(self, image):
        from scipy import linalg

        image = image.reshape(self.size * self.size)

        return self.Gsqrt.dot(image)

    def ST_batch(self, img_set):
        for index, img in enumerate(img_set):
            img_set[index] = self.ST(img.copy())
        return img_set

    def dist_pixel_L1(self, img1, img2):
        img1 = img1.reshape(self.size, self.size)
        img2 = img2.reshape(self.size, self.size)
        dist = 0
        for row in range(self.size):
            for col in range(self.size):
                dist += abs(img1[row][col] - img2[row][col])
        return dist

    def dist_pixel_L2(self, img1, img2):
        img1 = img1.reshape(self.size, self.size)
        img2 = img2.reshape(self.size, self.size)
        dist = 0
        for row in range(self.size):
            for col in range(self.size):
                dist += math.sqrt(abs(img1[row][col] * img1[row][col] - img2[row][col] * img2[row][col]))
        return dist

    def dist_pixel_L3(self, img1, img2):
        img1 = img1.reshape(self.size, self.size)
        img2 = img2.reshape(self.size, self.size)
        dist = 0
        for row in range(self.size):
            for col in range(self.size):
                dist += math.pow(abs(math.pow(img1[row][col], 3) - math.pow(img2[row][col], 3)), 1.0 / 3.0)
        return dist

    def dist_IMED(self, img_x, img_y, G = None):
        """
        http://www.cis.pku.edu.cn/faculty/vision/wangliwei/pdf/IMED.pdf
        """
        dist = (numpy.transpose(img_x - img_y)).dot(img_x - img_y)
        # for row_i in range(self.size):
        #     for col_i in range(self.size):
        #         for row_j in range(self.size):
        #             for col_j in range(self.size):
        #                 dist += (img_x[row_i][col_i] - img_y[row_i][col_i]) * (img_x[row_j][col_j] - img_y[row_j][col_j])
                        # dist += self.G[row_i][col_i][row_j][col_j] * (img_x[row_i][col_i] - img_y[row_i][col_i]) * (img_x[row_j][col_j] - img_y[row_j][col_j])
        # dist = math.sqrt(dist/(2*math.pi)) it doesn't change the relations
        return dist

    def dist(self, img1, img2, metric="pixel_L2"):
        # print(metric)

        if metric == "pixel_L1":
            return self.dist_pixel_L1(img1, img2)
        elif metric == "pixel_L2":
            return self.dist_pixel_L2(img1, img2)
        elif metric == "pixel_L3":
            return self.dist_pixel_L3(img1, img2)
        elif metric == "IMED":
            return self.dist_IMED(img1, img2)
        else:
            print("Unknown metric")
            return 0

    def calc_dist(self, X_test, metric, k=None):
        """
            Predict labels for test data using this classifier.

            Inputs:
            - X_test: A numpy array of shape (num_test, D) containing test data consisting
                    of num_test samples each of dimension D.
            - k: The number of nearest neighbors that vote for the predicted labels.
            - num_loops: Determines which implementation to use to compute distances
            between training points and testing points.

            Returns:
            - y: A numpy array of shape (num_test,) containing predicted labels for the
                test data, where y[i] is the predicted label for the test point X[i].
        """
        if k == None:
            k = self.k

        num_train_objects = self.X_train.shape[0]
        num_test_objects = X_test.shape[0]

        self.dist_mt = numpy.zeros((num_test_objects, num_train_objects))

        def set_row(X_test_i):
            return numpy.apply_along_axis(self.dist, 1, self.X_train, X_test_i, metric=metric)

        if self.num_loops == 2:
            #########################################################################
            # Fill matrix self.dist_mt by using 2 loops                             #
            #########################################################################
            for i in range(num_test_objects):
                for j in range(num_train_objects):
                    self.dist_mt[i][j] = self.dist(X_test[i], self.X_train[j], metric=metric)

        if self.num_loops == 1:
            #########################################################################
            # Fill matrix self.dist_mt by using 1 loop                              #
            #########################################################################
            for i in range(num_test_objects):
                self.dist_mt[i] = set_row(X_test[i])

        if self.num_loops == 0:
            #########################################################################
            # Fill matrix self.dist_mt by using 0 loops                             #
            #########################################################################
            self.dist_mt = numpy.apply_along_axis(set_row, 1, X_test)

        # print(self.dist_mt)

        return self

    def predict_labels(self, X_test, k=None):
        """
            Given a matrix of distances between test points and training points,
            predict a label for each test point.

            Inputs:
            - dists: A numpy array of shape (num_test, num_train) where dists[i, j]
              gives the distance between the ith test point and the jth training point.

            Returns:
            - y: A numpy array of shape (num_test,) containing predicted labels for the
              test data, where y[i] is the predicted label for the test point X[i].
        """
        if k == None:
            k = self.k

        num_test = X_test.shape[0]
        y_pred = numpy.zeros(num_test)

        for i in xrange(num_test):
            closest_y = self.y_train[numpy.argsort(self.dist_mt[i])[0:k]]
            count = Counter(closest_y)
            # print(count.most_common(1))
            y_pred[i] = count.most_common   (1)[0][0]

        return y_pred

"""
################################################################################
#><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><#
#....................................KDTREE....................................#
#><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><#
################################################################################
"""

from sklearn.neighbors import KDTree


class KDBasedKNearestNeighbor(object):
    """
        KDTree-based KNN classifier with L2 distance
    """

    def __init__(self, k=1):
        self.k = k

    def fit(self, X_train, y_train):
        """
            Build KDtree using
            http://scikit-learn.org/stable/modules/generated/sklearn.neighbors.KDTree.html
        """
        self.X_train = X_train
        self.y_train = y_train

        return self

    def calc_dist(self, X_test, metric, k=None):
        if k == None:
            k = self.k

        self.kd_tree = KDTree(self.X_train, metric=metric, leaf_size=self.k)

        return self

    def get_neighbors(self, X_test, k=None):
        if k == None:
            k = self.k

        neighbors = self.kd_tree.query(X_test, k)

        num_test = X_test.shape[0]

        y_pred = numpy.zeros(num_test)
        return neighbors[1]

    def predict_labels(self, X_test, k=None):
        """
            Make prediction using kdtree
            Return array of predicted labels
        """
        if k == None:
            k = self.k

        neighbors = self.kd_tree.query(X_test, k)

        num_test = X_test.shape[0]

        y_pred = numpy.zeros(num_test)

        for i in range(num_test):
            closest_y = self.y_train[neighbors[1][i]]
            count = Counter(closest_y)
            # print(count.most_common(1))
            y_pred[i] = count.most_common(1)[0][0]

        return y_pred
