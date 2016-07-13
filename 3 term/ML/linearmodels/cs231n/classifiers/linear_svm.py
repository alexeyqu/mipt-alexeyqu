import numpy

def svm_loss_naive(W, X, y, reg):
  """
  Structured SVM loss function, naive implementation (with loops)
  Inputs:
  - W: C x D array of weights
  - X: D x N array of data. Data are D-dimensional columns
  - y: 1-dimensional array of length N with labels 0...K-1, for K classes
  - reg: (float) regularization strength
  Returns:
  a tuple of:
  - loss as single float
  - gradient with respect to weights W; an array of same shape as W
  """
  #############################################################################
  # TODO:                                                                     #
  # Compute the gradient of the loss function and store it dW.                #
  # Rather that first computing the loss and then computing the derivative,   #
  # it may be simpler to compute the derivative at the same time that the     #
  # loss is being computed. As a result you may need to modify some of the    #
  # code above to compute the gradient.                                       #
  #############################################################################

  dW = numpy.zeros(W.shape)
  loss = 0.0

  num_classes = W.shape[0]
  num_train = X.shape[1]

  for i in range(num_train):
    scores = W.dot(X[:, i])
    correct_class_score = scores[y[i]]

    for j in range(num_classes):

      if j == y[i]:
        continue

      margin = scores[j] - correct_class_score + 1

      if margin > 0:
        loss += margin
        dW[j, :] += X[:, i].T
        dW[y[i], :] -= X[:, i].T

  loss /= num_train
  dW /= num_train

  loss += 0.5 * reg * numpy.sum(W * W)
  dW += reg * W

  return loss, dW


def svm_loss_vectorized(W, X, y, reg):
  """
  Structured SVM loss function, vectorized implementation.

  Inputs and outputs are the same as svm_loss_naive.
  """
  loss = 0.0
  dW = numpy.zeros(W.shape) # initialize the gradient as zero
  num_train = X.shape[1]
  
  #############################################################################
  # TODO:                                                                     #
  # Implement a vectorized version of the structured SVM loss, storing the    #
  # result in loss.                                                           #
  #############################################################################

  scores = numpy.dot(W, X)

  correct_scores = numpy.ones(scores.shape) * scores[y, numpy.arange(0, scores.shape[1])]
  deltas = numpy.ones(scores.shape)

  L = max(scores - correct_scores + deltas, 0)
  L[y, numpy.arange(0, scores.shape[1])] = 0

  loss = numpy.sum(L) / num_train + 0.5 * reg * numpy.sum(W * W)

  #############################################################################
  # TODO:                                                                     #
  # Implement a vectorized version of the gradient for the structured SVM     #
  # loss, storing the result in dW.                                           #
  #                                                                           #
  # Hint: Instead of computing the gradient from scratch, it may be easier    #
  # to reuse some of the intermediate values that you used to compute the     #
  # loss.                                                                     #
  #############################################################################

  L = max(scores - correct_scores + deltas, 0)
  L[L > 0] = 1
  L[y, numpy.arange(0, scores.shape[1])] = 0
  L[y, numpy.arange(0, scores.shape[1])] = -1 * numpy.sum(L, axis=0)

  dW = numpy.dot(L, X.T)

  num_train = X.shape[1]
  dW /= num_train
  
  #############################################################################
  #                             END OF YOUR CODE                              #
  #############################################################################
  return loss, dW














