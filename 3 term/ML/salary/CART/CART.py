from __future__ import division
import sklearn
import numpy
from itertools import chain, combinations

from Node import Node

class CART(sklearn.base.BaseEstimator, sklearn.base.ClassifierMixin):
    def __init__(self, leaf_min_size, tree_depth, my_ord, my_cont, my_cat, my_cat_slow, criterion="gini", barrier_improvement = 0.0):
        self.leaf_min_size = leaf_min_size
        self.tree_depth = tree_depth
        self.criterion = criterion
        self.barrier_improvement = barrier_improvement
        
        self.ORDINAL_COLUMNS = my_ord
        self.CONTINUOUS_COLUMNS = my_cont
        self.CATEGORICAL_COLUMNS = my_cat
        self.CATEGORICAL_COLUMNS_SLOW = my_cat_slow

        self.ALL_COLUMNS = self.ORDINAL_COLUMNS.copy()
        self.ALL_COLUMNS.update(self.CONTINUOUS_COLUMNS)
        self.ALL_COLUMNS.update(self.CATEGORICAL_COLUMNS_SLOW)
        self.ALL_COLUMNS.update(self.CATEGORICAL_COLUMNS)

        self.tree = None
        self.tree_root = None

    def gini(self, left_subset, right_subset):

        pr_left = left_subset.__len__()/(left_subset.__len__() + right_subset.__len__())
        pr_right = right_subset.__len__()/(left_subset.__len__() + right_subset.__len__())

        return self.gini_impurity(left_subset + right_subset) - pr_left*self.gini_impurity(left_subset) - pr_right*self.gini_impurity(right_subset)

    def gini_impurity(self, node_subset):
        impurity = 0

        subset_size = node_subset.__len__()

        for node_class_k in self.classes:
            for node_class_l in self.classes:
                if(node_class_k != node_class_l):
                    class_k_count = [node_entry for node_entry in node_subset if node_entry == node_class_k].__len__()
                    class_l_count = [node_entry for node_entry in node_subset if node_entry == node_class_l].__len__()
                    impurity += class_k_count*class_l_count / subset_size**2

        return impurity


    def twoing(self, left_subset, right_subset):

        pr_left = left_subset.__len__()/(left_subset.__len__() + right_subset.__len__())
        pr_right = right_subset.__len__()/(left_subset.__len__() + right_subset.__len__())

        return pr_left * pr_right / 4 * (self.twoing_impurity(left_subset, right_subset)) ** 2

    def twoing_impurity(self, left_subset, right_subset):
        impurity = 0

        total_size = left_subset.__len__() + right_subset.__len__()

        for class_k in self.classes:
            impurity += abs([node_entry for node_entry in left_subset if node_entry == class_k].__len__()/total_size - \
                            [node_entry for node_entry in right_subset if node_entry == class_k].__len__()/total_size)

        return impurity

    def ordered_twoing_impurity(self, left_subset, right_subset):
        impurity = 0

        total_size = left_subset.__len__() + right_subset.__len__()

        for class_k in self.classes:
            if class_k == 1:
                continue
            impurity += abs([node_entry for node_entry in left_subset if node_entry == class_k].__len__()/total_size - \
                            [node_entry for node_entry in right_subset if node_entry == class_k].__len__()/total_size)

        return impurity

    def ordered_twoing(self, left_subset, right_subset):

        pr_left = left_subset.__len__()/(left_subset.__len__() + right_subset.__len__())
        pr_right = right_subset.__len__()/(left_subset.__len__() + right_subset.__len__())

        return pr_left * pr_right / 4 * (self.ordered_twoing_impurity(left_subset, right_subset)) ** 2


    def impurity_change(self, left_subset, right_subset):
        if self.criterion == "gini":
            return self.gini(left_subset, right_subset)
        elif self.criterion == "twoing":
            return self.twoing(left_subset, right_subset)
        elif self.criterion == "ordered_twoing":
            return self.ordered_twoing(left_subset, right_subset)
        else:
            #You could insert here call for ordered Twoing
            raise ValueError("Unknown criterion type!")

    def print_tree(self):
        for node in self.tree:
            node.print_node()

    def dot_print_tree(self, filename):
        out_file = open(filename, "w")

        out_file.write("strict digraph G {")

        for node in self.tree:
            if node.left != None and node.right != None:
                out_file.write("%d [label=\"ID = %d \n X[%d (%s)] : %s \n gini = %0.3f \n samples = %d\"];" \
                    % (node.index, node.index, node.splitting_feature, self.ALL_COLUMNS[node.splitting_feature], str(node.splitting_value), node.impurity, node.class_subset.__len__()))
            else:
                out_file.write("%d [label=\"ID = %d \n samples = %d\"];" % (node.index, node.index, node.class_subset.__len__()))

            if node.left != None:
                out_file.write("%d -> %d;" % (node.index, node.left.index))

            if node.right != None:
                out_file.write("%d -> %d;" % (node.index, node.right.index))

        out_file.write("}")

        out_file.close()

    def get_all_subsets(self, set):
        return chain.from_iterable(combinations(set, r) for r in range(len(set)+1))
    #
    # def split_ordered_twoing(self, node):
    #     splits = []
    #
    #     for featureID in self.ORDINAL_COLUMNS.keys():
    #         current_feature = [row[featureID] for row in node.data_subset]
    #         current_feature.sort()
    #
    #         max_impurity_change_point = None
    #         max_impurity_change = float("-inf")
    #
    #         for split_point in current_feature:
    #             cur_left_subset = []
    #             cur_right_subset = []
    #
    #             for row_index in range(node.data_subset.__len__()):
    #                 if node.data_subset[row_index][featureID] <= split_point:
    #                     cur_left_subset.append(node.class_subset[row_index])
    #                 else:
    #                     cur_right_subset.append(node.class_subset[row_index])
    #
    #             if(cur_left_subset.__len__() == 0 or cur_right_subset.__len__() == 0):
    #                 continue
    #
    #             cur_impurity_change = self.ordered_twoing(cur_left_subset, cur_right_subset, self.ORDINAL_COLUMNS)
    #
    #             print("AAA")
    #             print(cur_impurity_change)
    #
    #             if cur_impurity_change > max_impurity_change:
    #                 max_impurity_change = cur_impurity_change
    #                 max_impurity_change_point = split_point
    #
    #         splits.append({"ID" : featureID, "max" : max_impurity_change, "point" : max_impurity_change_point})
    #
    #     final_max_impurity_feature = None
    #     final_max_impurity_change = float("-inf")
    #     final_max_impurity_change_point = None
    #
    #     for split in splits:
    #         print("MAXSPLIT " + str(split["max"]))
    #         if split["max"] > final_max_impurity_change:
    #             final_max_impurity_feature = split["ID"]
    #             final_max_impurity_change = split["max"]
    #             final_max_impurity_change_point = split["point"]
    #
    #     left_subset = []
    #     left_classes = []
    #     right_subset = []
    #     right_classes = []
    #
    #     if final_max_impurity_feature in self.CATEGORICAL_COLUMNS:
    #         for row_index in range(node.data_subset.__len__()):
    #             if node.data_subset[row_index][final_max_impurity_feature] in final_max_impurity_change_point:
    #                 left_subset.append(node.data_subset[row_index])
    #                 left_classes.append(node.class_subset[row_index])
    #             else:
    #                 right_subset.append(node.data_subset[row_index])
    #                 right_classes.append(node.class_subset[row_index])
    #     else:
    #         for row_index in range(node.data_subset.__len__()):
    #             print(final_max_impurity_feature)
    #             print(node.data_subset[row_index][final_max_impurity_feature])
    #             if node.data_subset[row_index][final_max_impurity_feature] <= final_max_impurity_change_point:
    #                 left_subset.append(node.data_subset[row_index])
    #                 left_classes.append(node.class_subset[row_index])
    #             else:
    #                 right_subset.append(node.data_subset[row_index])
    #                 right_classes.append(node.class_subset[row_index])
    #
    #     left_node = Node(node.index * 2 + 1, node.node_depth + 1, left_subset, left_classes)
    #     right_node = Node(node.index * 2 + 2, node.node_depth + 1, right_subset, right_classes)
    #
    #     node.left = left_node
    #     node.right = right_node
    #     node.splitting_feature = final_max_impurity_feature
    #     node.splitting_value = final_max_impurity_change_point
    #     node.impurity_gain = final_max_impurity_change
    #
    #     node.impurity = 0.0
    #
    #     node.is_leaf = False
    #
    #     return left_node, right_node

    def split_node(self, node):

        # if self.criterion == "ordered_twoing":
        #     return self.split_ordered_twoing(node)

        # node.print_node()
        # print(node.data_subset)
        # raw_input()

        print("Current node: " + str(node.index))

        splits = []

        for featureID in self.CONTINUOUS_COLUMNS.keys() + self.ORDINAL_COLUMNS.keys():
            # print(featureID)
            # print(node.data_subset)
            current_feature = [row[featureID] for row in node.data_subset]
            current_feature.sort()

            max_impurity_change_point = None
            max_impurity_change = float("-inf")

            for split_point in current_feature:
                cur_left_subset = []
                cur_right_subset = []

                for row_index in range(node.data_subset.__len__()):
                    if node.data_subset[row_index][featureID] <= split_point:
                        cur_left_subset.append(node.class_subset[row_index])
                    else:
                        cur_right_subset.append(node.class_subset[row_index])

                # print(split_point)
                # print(cur_left_subset.__len__())

                if(cur_left_subset.__len__() == 0 or cur_right_subset.__len__() == 0):
                    continue
                # raw_input()

                cur_impurity_change = self.impurity_change(cur_left_subset, cur_right_subset)

                # print("CURI " + str(cur_impurity_change))

                # print(cur_impurity)
                #
                # raw_input()

                if cur_impurity_change > max_impurity_change:
                    max_impurity_change = cur_impurity_change
                    max_impurity_change_point = split_point

            # print(max_criterion_point)

            splits.append({"ID" : featureID, "max" : max_impurity_change, "point" : max_impurity_change_point})

        for featureID in self.CATEGORICAL_COLUMNS.keys():
            # print(featureID)
            current_feature = [row[featureID] for row in node.data_subset]
            current_feature_set = set(current_feature)
            current_feature_subsets = self.get_all_subsets(current_feature_set)

            max_impurity_change_point = None
            max_impurity_change = float("-inf")

            for subset in current_feature_subsets:
                if(len(subset) != 0 and len(subset) != len(current_feature_set)):
                    # print(subset)
                    cur_left_subset = []
                    cur_right_subset = []

                    for row_index in range(node.data_subset.__len__()):
                        if node.data_subset[row_index][featureID] in subset:
                            cur_left_subset.append(node.class_subset[row_index])
                        else:
                            cur_right_subset.append(node.class_subset[row_index])

                    # print(split_point)
                    # print(cur_left_subset.__len__())

                    if(cur_left_subset.__len__() == 0 or cur_right_subset.__len__() == 0):
                        continue
                    # raw_input()

                    cur_impurity_change = self.impurity_change(cur_left_subset, cur_right_subset)

                    # print("CURI " + str(cur_impurity_change) + str(max_impurity_change))

                    if cur_impurity_change > max_impurity_change:
                        max_impurity_change = cur_impurity_change
                        max_impurity_change_point = subset

            splits.append({"ID" : featureID, "max" : max_impurity_change, "point" : max_impurity_change_point})

        # print(splits)

        final_max_impurity_feature = None
        final_max_impurity_change = float("-inf")
        final_max_impurity_change_point = None

        for split in splits:
            # print("MAXSPLIT " + str(split["max"]))
            if split["max"] > final_max_impurity_change:
                final_max_impurity_feature = split["ID"]
                final_max_impurity_change = split["max"]
                final_max_impurity_change_point = split["point"]

        # raw_input()

        # print(final_max_impurity_feature)
        # print(final_max_impurity_change)
        # print(final_max_impurity_change_point)

        left_subset = []
        left_classes = []
        right_subset = []
        right_classes = []

        if final_max_impurity_feature in self.CATEGORICAL_COLUMNS:
            for row_index in range(node.data_subset.__len__()):
                if node.data_subset[row_index][final_max_impurity_feature] in final_max_impurity_change_point:
                    left_subset.append(node.data_subset[row_index])
                    left_classes.append(node.class_subset[row_index])
                else:
                    right_subset.append(node.data_subset[row_index])
                    right_classes.append(node.class_subset[row_index])
        else:
            for row_index in range(node.data_subset.__len__()):
                # print(final_max_impurity_feature)
                # print(node.data_subset[row_index][final_max_impurity_feature])
                if node.data_subset[row_index][final_max_impurity_feature] <= final_max_impurity_change_point:
                    left_subset.append(node.data_subset[row_index])
                    left_classes.append(node.class_subset[row_index])
                else:
                    right_subset.append(node.data_subset[row_index])
                    right_classes.append(node.class_subset[row_index])

        left_node = Node(node.index * 2 + 1, node.node_depth + 1, left_subset, left_classes)
        right_node = Node(node.index * 2 + 2, node.node_depth + 1, right_subset, right_classes)

        node.left = left_node
        node.right = right_node
        node.splitting_feature = final_max_impurity_feature
        node.splitting_value = final_max_impurity_change_point
        node.impurity_gain = final_max_impurity_change
        if self.criterion == "gini":
            node.impurity = self.gini_impurity(node.class_subset)
        elif self.criterion == "twoing" or self.criterion == "ordered_twoing":
            node.impurity = 0.0
        node.is_leaf = False

        return left_node, right_node


    def pre_stopping_criterion(self, node):
        """
            There are a few stopping criteria. You should implement them here.

            + If a node becomes pure; that is, all cases in a node have identical values of the dependent
            variable, the node will not be split.
            + If all cases in a node have identical values for each predictor, the node will not be split.
            + If the current tree depth reaches the user-specified maximum tree depth limit value, the
            tree growing process will stop.
            + If the size of a node is less than the user-specified minimum node size value, the node
            will not be split.

            Some sources call some of the criteria (e.g. tree depth) a pre-pruning, so you could check sources
            about it before implementation.
        """

        if node.node_depth >= self.tree_depth:
            print(str(node.index) + " not split because of max depth")
            return True

        if len(set(node.class_subset)) == 1:
            print(str(node.index) + " not split because of 1 class")
            return True

        if len(node.data_subset) <= self.leaf_min_size:
            print(str(node.index) + " not split because of small size\n Size: ")
            print(len(node.data_subset))
            return True

        identical = True

        for index in range(node.data_subset.__len__() - 1):
            for column in range(node.data_subset[index].__len__()):
                if node.data_subset[index][column] != node.data_subset[index + 1][column]:
                    identical = False

        if identical == True:
            print(str(node.index) + " not split because of identical cases")
            return True

        return False


    def post_stopping_criterion(self, node, lchild, rchild):
        """
            There are a few stopping criteria. You should implement them here.

            + If the split of a node results in a child node whose node size is less than the user-specified
            minimum child node size value, the node will not be split.
            + If for the best split, the barrier_improvement is smaller than the user-specified minimum barrier_improvement,
            the node will not be split.

            Some sources call some of the criteria (e.g. tree depth) a pre-pruning, so you could check sources
            about it before implementation.
        """


        if node.impurity_gain < self.barrier_improvement:
            print(str(node.index) + " not split because of low gain")
            return True

        if len(lchild.data_subset) < self.leaf_min_size or \
            len(rchild.data_subset) < self.leaf_min_size:
            print(str(node.index) + " not split because of small child\n Sizes: ")
            print(len(lchild.data_subset))
            print(len(rchild.data_subset))
            return True

        return False

    def fit(self, X_train, y_train):
        """
            1. Find each predictor's best split.
            For each continuous and ordinal predictor, sort its values from the smallest to the largest.
            For the sorted predictor, go through each value from top to examine each candidate split
            point (call it v, if x \leq v, the case goes to the left child node, otherwise, goes to the right.)
            to determine the best. The best split point is the one that maximize the splitting criterion
            the most when the node is split according to it. The definition of splitting criterion is in
            later section.
            For each nominal predictor, examine each possible subset of categories (call it A, if
            x \in A, the case goes to the left child node, otherwise, goes to the right.) to find the best
            split.
            2. Find the node's best split.
            Among the best splits found in step 1, choose the one that maximizes the splitting
            criterion.
            3. Split the node using its best split found in step 2 if the stopping rules are not satisfied.

            Write your growing tree code here. You should use stopping_criterion while growing it.
        """
        self.X_train = X_train
        self.y_train = y_train
        self.classes = set(y_train)

        self.tree = self.build_tree()

        # self.print_tree()

        # root.print_node()
        #
        # lchild.print_node()
        #
        # rchild.print_node()

        return self

    def build_tree(self):

        tree = []

        root = Node(0, 0, self.X_train, self.y_train)

        tree.append(root)

        self.tree_root = root

        for node in tree:

            # node.print_node()

            if self.pre_stopping_criterion(node) == True:
                continue

            lchild, rchild = self.split_node(node)

            if self.post_stopping_criterion(node, lchild, rchild) == True:
                node.left = None
                node.right = None
                continue

            tree.append(lchild)
            tree.append(rchild)

        return tree

    def get_node_by_id(self, ID):
        for node in self.tree:
            if node.index == ID:
                return node

    def find_best_leaf(self, case):
        # print(case)
        cur_node = self.tree_root

        # cur_node.print_node()
        # print(cur_node.is_leaf)

        while not cur_node.is_leaf:
            # print(case[cur_node.splitting_feature])
            if cur_node.splitting_feature in self.CATEGORICAL_COLUMNS:
                # print(cur_node.splitting_value)
                # print(cur_node.splitting_feature)
                if case[cur_node.splitting_feature] in cur_node.splitting_value:
                    cur_node = cur_node.left
                else:
                    cur_node = cur_node.right
            else:
                if case[cur_node.splitting_feature] <= cur_node.splitting_value:
                    cur_node = cur_node.left
                else:
                    cur_node = cur_node.right

            # cur_node.print_node()

        return cur_node

    
    def predict(self, X_test):
        """
            Predict labels for test data using this classifier. Take the tree growned in the fit stage.
            Don't forget to handle missing values: the label you choose should be from the sub-tree
            which has less enthropy.

            Inputs:
            - X_test: A numpy array of shape (num_test, D) containing test data consisting
                      of num_test samples each of dimension D.

            Returns:
            - y: A numpy array of shape (num_test,) containing predicted labels for the
                 test data, where y[i] is the predicted label for the test point X[i].
        """
        from collections import Counter

        size = X_test.shape[0]

        y_pred = numpy.zeros(size)

        for case in range(size):
            best_leaf = self.find_best_leaf(X_test[case])
            count = Counter(best_leaf.class_subset)
            if case % 1000 == 0:
                print(case)
            # print(count)
            y_pred[case] = int(count.most_common(1)[0][0])

        return y_pred, best_leaf

    def find_internal_nodes(self, node, internal_nodes):
        if node.left != None or node.right != None:
            internal_nodes.append(node)

            if not node.left.is_leaf:
                self.find_internal_nodes(node.left, internal_nodes)

            if not node.right.is_leaf:
                self.find_internal_nodes(node.right, internal_nodes)

    def remove_subtree(self, node):
        if node.left != None or node.right != None:
            self.remove_subtree(node.left)
            self.remove_subtree(node.right)

        self.tree.remove(node)


    def prune(self, target_id, alpha = 0.1):
        internal_nodes = []

        self.find_internal_nodes(self.tree_root, internal_nodes)
        while len(internal_nodes) > 0:
            min_inc = float("inf")
            pnode = None
            for node in internal_nodes:
                inc_loss = self.loss(node.data_subset, target_id) - self.loss(node.left.data_subset, target_id) - self.loss(node.right.data_subset, target_id)
                if inc_loss < min_inc:
                    min_inc = inc_loss
                    pnode = node
            if min_inc < alpha:
                self.remove_subtree(pnode.left)
                self.remove_subtree(pnode.right)
                pnode.is_leaf = True
                pnode.left = None
                pnode.right = None
            else:
                return
            internal_nodes = []
            self.find_internal_nodes(self.tree_root, internal_nodes)

    def loss(self, train, target_id):
        avg = sum([row[target_id] for row in train]) / float(len(train))
        ret = 0.0
        for row in train:
            ret += (row[target_id] - avg)**2
        return ret