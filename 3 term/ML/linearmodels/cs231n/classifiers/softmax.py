import numpy

def softmax_loss_naive(W, X, y, reg):
  """
  Softmax loss function, naive implementation (with loops)
  Inputs:
  - W: C x D array of weights
  - X: D x N array of data. Data are D-dimensional columns
  - y: 1-dimensional array of length N with labels 0...K-1, for K classes
  - reg: (float) regularization strength
  Returns:
  a tuple of:
  - loss as single float
  - gradient with respect to weights W, an array of same size as W
  """
  # Initialize the loss and gradient to zero.
  loss = 0.0
  dW = numpy.zeros_like(W)
  #############################################################################
  # TODO: Compute the softmax loss and its gradient using explicit loops.     #
  # Store the loss in loss and the gradient in dW. If you are not careful     #
  # here, it is easy to run into numeric instability. Don't forget the        #
  # regularization!                                                           #
  #############################################################################
  
  # Right now the loss is a sum over all training examples, but we want it
  # to be an average instead so we divide by num_train.

  # Add regularization to the loss.

  height = W.shape[0]
  N = X.shape[1]

  for i in range(0, N):
    scores = W.dot(X[:, i])
    scores -= numpy.max(scores)

    loss -= scores[y[i]]

    sum_exp = 0.0
    for score in scores:
      sum_exp += numpy.exp(score)

    for j in range(0, height):
      dW[j, :] += 1.0 / sum_exp * numpy.exp(scores[j]) * X[:, i]

      if j == y[i]:
        dW[j, :] -= X[:, i]

    loss += numpy.log(sum_exp)

  loss /= N
  dW /= N

  loss += reg * numpy.sum(W * W) /2
  dW += reg * W

  #############################################################################
  #                          END OF YOUR CODE                                 #
  #############################################################################
  return loss, dW


def softmax_loss_vectorized(W, X, y, reg):
  """
  Softmax loss function, vectorized version.

  Inputs and outputs are the same as softmax_loss_naive.
  """
  #############################################################################
  # TODO: Compute the softmax loss and its gradient using no explicit loops.  #
  # Store the loss in loss and the gradient in dW. If you are not careful     #
  # here, it is easy to run into numeric instability. Don't forget the        #
  # regularization!                                                           #
  #############################################################################

  loss = 0.0
  dW = numpy.zeros_like(W)
  num_train = X.shape[1]
  num_classes = W.shape[0]

  scores = numpy.dot(W, X)
  scores -= numpy.max(scores)

  y_tmp = numpy.zeros(shape = (num_classes, num_train))
  y_tmp[y, range(num_train)] = 1

  exp_scores = numpy.exp(scores)
  sums_exp = numpy.sum(exp_scores, axis=0)

  result = numpy.log(sums_exp) - numpy.sum(numpy.multiply(y_tmp, scores), axis=0)

  sum_exp_scores = numpy.sum(exp_scores, axis=0)
  sum_exp_scores = 1.0 / (sum_exp_scores + 1e-8)

  dW = exp_scores * sum_exp_scores
  dW = numpy.dot(dW, X.T) - numpy.dot(y_tmp, X.T)

  dW = dW / float(num_train) + reg * W

  loss = numpy.sum(result) / float(num_train) + 0.5 * reg * numpy.sum(W * W)
  
  #############################################################################
  #                          END OF YOUR CODE                                 #
  #############################################################################

  return loss, dW
