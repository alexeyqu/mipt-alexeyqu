
class Node:
    def __init__(self, my_index, my_depth, my_data_subset, my_labels_subset):
        self.index = my_index
        self.node_depth = my_depth
        self.left = None
        self.right = None
        self.is_leaf = True

        self.data_subset = my_data_subset
        self.class_subset = my_labels_subset
        self.splitting_feature = None
        self.splitting_value = None
        self.impurity_gain = None
        self.impurity = None

    def print_node(self):
        print("Node " + str(self.index) + ", " + str(self.node_depth))
        if(self.is_leaf):
            print("LEAF")
        else:
            print("left: " + str(self.left.index) + ", right: " + str(self.right.index))
        print("feature: " + str(self.splitting_feature) + ", splitter by: " + str(self.splitting_value))
        print("impurity: " + str(self.impurity) + ", gain: " + str(self.impurity_gain))
        # print("Values: " + str(self.class_subset.count(0)) + " - " + str(self.class_subset.count(1)))
