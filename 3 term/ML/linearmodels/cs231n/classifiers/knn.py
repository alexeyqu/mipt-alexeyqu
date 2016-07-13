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

    def fit(self, X_train, y_train):
        """
            Only save the training data.
        """
        self.X_train = X_train
        self.y_train = y_train
        return self

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

        for i in range(num_test_objects):
            for j in range(num_train_objects):
                self.dist_mt[i][j] = numpy.sqrt(numpy.sum((X_test[i, :] - self.X_train[j, :]) ** 2))

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

        for i in range(num_test):
            closest_y = self.y_train[numpy.argsort(self.dist_mt[i])[0:k]]
            count = Counter(closest_y)
            # print(count.most_common(1))
            y_pred[i] = count.most_common   (1)[0][0]

        return y_pred