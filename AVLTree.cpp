// AVLTree.cpp
// Jacob Reppeto

#include "AVLTree.hpp"

AVLTree::AVLTree() {
	_root = nullptr;
	_count = 0;
}
AVLTree::AVLTree(const AVLTree& source) {
	_root = _copyNodes(source._root);
	_count = source._count;
}

AVLTree& AVLTree::operator=(const AVLTree& source) {
	if (this != &source) {
		clear();
		_root = _copyNodes(source._root);
		_count = source._count;
	}
	return *this;
}

void AVLTree::clear() {
	_root = nullptr;
	_count = 0;
}

void AVLTree::insert(const ItemType& item) {
	_insertHelp(_root, item);
}

std::shared_ptr<BinaryTreeNode> AVLTree::find(const ItemType& item) const {
	return _findHelp(_root, item);
}

std::shared_ptr<BinaryTreeNode> AVLTree::minimumNode() const {
	return _minimumNodeHelp(_root);
}

std::shared_ptr<BinaryTreeNode> AVLTree::maximumNode() const {
	return _maximumNodeHelp(_root);
}

std::shared_ptr<BinaryTreeNode> AVLTree::nextSmallestNode(std::shared_ptr<BinaryTreeNode> node) const {
	if (node == nullptr)
		return nullptr;
	// If there is a left subtree, the next smallest node is the maximum node in that subtree
	if (node->_leftNode) {
		return _maximumNodeHelp(node->_leftNode);
	}
	// Otherwise, traverse up the tree until we find a node that is the right child of its parent
	auto current = node;
	auto parent = current->_parentNode.lock();
	// while parent is not null and current is the left child of parent
	while (parent && current == parent->_leftNode) {
		current = parent;
		parent = parent->_parentNode.lock();
	}
	return parent; // This could be nullptr if we reached the root without finding a larger ancestor
}

std::shared_ptr<BinaryTreeNode> AVLTree::nextLargestNode(std::shared_ptr<BinaryTreeNode> node) const {
	if (node == nullptr)
		return nullptr;
	// If there is a right subtree, the next largest node is the minimum node in that subtree
	if (node->_rightNode) {
		return _minimumNodeHelp(node->_rightNode);
	}
	// Otherwise, traverse up the tree until we find a node that is the left child of its parent
	auto current = node;
	auto parent = current->_parentNode.lock();
	// while parent is not null and current is the right child of parent
	while (parent && current == parent->_rightNode) {
		current = parent;
		parent = parent->_parentNode.lock();
	}
	return parent; // This could be nullptr if we reached the root without finding a larger ancestor
}

std::vector<ItemType> AVLTree::inorder() const {
	return _inorderHelp(_root);
}

std::vector<ItemType> AVLTree::preorder() const {
	return _preorderHelp(_root);
}

std::vector<ItemType> AVLTree::postorder() const {
	return _postorderHelp(_root);
}

std::shared_ptr<BinaryTreeNode> AVLTree::_copyNodes(const std::shared_ptr<BinaryTreeNode>& rootNode) const {
	if (!rootNode) {
		return nullptr;
	}
	// create a new node with the same item as the root node
	auto newNode = std::make_shared<BinaryTreeNode>(rootNode->_item);
	// recursively copy the left and right subtrees
	newNode->_leftNode = _copyNodes(rootNode->_leftNode);
	if (newNode->_leftNode) {
		newNode->_leftNode->_parentNode = newNode;
	}
	newNode->_rightNode = _copyNodes(rootNode->_rightNode);
	if (newNode->_rightNode) {
		newNode->_rightNode->_parentNode = newNode;
	}
	// set the height of the new node
	newNode->_height = rootNode->_height;
	return newNode;
}

std::shared_ptr<BinaryTreeNode> AVLTree::_findHelp(const std::shared_ptr<BinaryTreeNode> rootNode, const ItemType& item) const {
	// if the tree is empty, return nullptr
	if (!rootNode) {
		return nullptr;
	}
	// if the item is found, return the node
	if (rootNode->_item == item) {
		return rootNode;
	}
	// if the item is less than the root node's item, search the left subtree
	if (item < rootNode->_item) {
		return _findHelp(rootNode->_leftNode, item);
	}
	// if the item is greater than the root node's item, search the right subtree
	return _findHelp(rootNode->_rightNode, item);
}

std::shared_ptr<BinaryTreeNode> AVLTree::_minimumNodeHelp(const std::shared_ptr<BinaryTreeNode> rootNode) const {
	// if the tree is empty, return nullptr
	if (!rootNode) {
		return nullptr;
	}
	// if there is no left child, return the root node
	if (!rootNode->_leftNode) {
		return rootNode;
	}

	// otherwise, recurse on the left child
	return _minimumNodeHelp(rootNode->_leftNode);

}

std::shared_ptr<BinaryTreeNode> AVLTree::_maximumNodeHelp(const std::shared_ptr<BinaryTreeNode> rootNode) const {
	// if the tree is empty, return nullptr
	if (!rootNode) {
		return nullptr;
	}
	// if there is no right child, return the root node
	if (!rootNode->_rightNode) {
		return rootNode;
	}
	// otherwise, recurse on the right child
	return _maximumNodeHelp(rootNode->_rightNode);
}

void AVLTree::_insertHelp(std::shared_ptr<BinaryTreeNode>& rootNode, const ItemType& item) {
	// Base case: if the current node is null, create a new node with the item
	if (!rootNode) {
		rootNode = std::make_shared<BinaryTreeNode>(item);
		_count++;
		return;
	}

	// Recursive case: traverse the tree to find the correct insertion point
	if (item < rootNode->_item) {
		_insertHelp(rootNode->_leftNode, item);
		if (rootNode->_leftNode) {
			rootNode->_leftNode->_parentNode = rootNode;
		}
	} else if (item > rootNode->_item) {
		_insertHelp(rootNode->_rightNode, item);
		if (rootNode->_rightNode) {
			rootNode->_rightNode->_parentNode = rootNode;
		}
	} else {
		// Item already exists in the tree; do not insert duplicates
		return;
	}

	// Update the height of the current node
	rootNode->setHeight(1 + std::max(getHeight(rootNode->_leftNode), getHeight(rootNode->_rightNode)));
	// Calculate the balance factor
	int balanceFactor = getHeight(rootNode->_leftNode) - getHeight(rootNode->_rightNode);
	// Perform rotations to maintain AVL property
	// Left heavy
	if (balanceFactor > 1) {
		if (item < rootNode->_leftNode->_item) {
			// Left-Left case
			_rightSingleRotate(rootNode);
		} else {
			// Left-Right case
			_leftRightRotate(rootNode);
		}
	}

	// Right heavy
	else if (balanceFactor < -1) {
		if (item > rootNode->_rightNode->_item) {
			// Right-Right case
			_leftSingleRotate(rootNode);
		} else {
			// Right-Left case
			_rightLeftRotate(rootNode);
		}
	}


}

std::vector<ItemType> AVLTree::_inorderHelp(const std::shared_ptr<BinaryTreeNode> rootNode) const {
	std::vector<ItemType> result;
	if (rootNode) {
		// traverse left subtree
		std::vector<ItemType> leftItems = _inorderHelp(rootNode->_leftNode);
		result.insert(result.end(), leftItems.begin(), leftItems.end());
		// goes back to the root to add it to result
		result.push_back(rootNode->_item);
		// traverse right subtree
		std::vector<ItemType> rightItems = _inorderHelp(rootNode->_rightNode);
		result.insert(result.end(), rightItems.begin(), rightItems.end());
	}
	return result;
}

std::vector<ItemType> AVLTree::_preorderHelp(const std::shared_ptr<BinaryTreeNode> rootNode) const {
	std::vector<ItemType> result;
	if (rootNode) {
		// goes to the root and adds it to result
		result.push_back(rootNode->_item);
		// traverse left subtree
		std::vector<ItemType> leftItems = _preorderHelp(rootNode->_leftNode);
		result.insert(result.end(), leftItems.begin(), leftItems.end());
		// traverse right subtree
		std::vector<ItemType> rightItems = _preorderHelp(rootNode->_rightNode);
		result.insert(result.end(), rightItems.begin(), rightItems.end());
	}
	return result;
}

std::vector<ItemType> AVLTree::_postorderHelp(const std::shared_ptr<BinaryTreeNode> rootNode) const {
	std::vector<ItemType> result;
	if (rootNode) {
		// traverse left subtree
		std::vector<ItemType> leftItems = _postorderHelp(rootNode->_leftNode);
		result.insert(result.end(), leftItems.begin(), leftItems.end());
		// traverse right subtree
		std::vector<ItemType> rightItems = _postorderHelp(rootNode->_rightNode);
		result.insert(result.end(), rightItems.begin(), rightItems.end());
		// goes back to the root to add it to result
		result.push_back(rootNode->_item);
	}
	return result;
}

void AVLTree::_leftSingleRotate(std::shared_ptr<BinaryTreeNode>& node) {\
// If the node or its right child is null, return
	if (!node || !node->_rightNode) return;
// Store the right child of the node
	auto right = node->_rightNode;
	// Update pointers to perform rotation
	node->_rightNode = right->_leftNode;
	// Update parent pointers if necessary
	if (node->_rightNode) node->_rightNode->_parentNode = node;
	// Update the node reference to point to the new root of the subtree
	right->_leftNode = node;
	// Update parent pointers
	right->_parentNode = node->_parentNode;
	// Update the original node's parent pointer
	node->_parentNode = right;
	// Update heights
	node->setHeight(1 + std::max(getHeight(node->_leftNode), getHeight(node->_rightNode)));
	// Set the new root of the subtree
	node = right;
	// Update height of the original node after rotation
	node->setHeight(1 + std::max(getHeight(node->_leftNode), getHeight(node->_rightNode)));
}

void AVLTree::_rightSingleRotate(std::shared_ptr<BinaryTreeNode>& node) {
	// If the node or its left child is null, return
	if (!node || !node->_leftNode) return;
	// Store the left child of the node
	auto left = node->_leftNode;
	// Update pointers to perform rotation
	node->_leftNode = left->_rightNode;
	// Update parent pointers if necessary
	if (node->_leftNode) node->_leftNode->_parentNode = node;
	// Update the node reference to point to the new root of the subtree
	left->_rightNode = node;
	// Update parent pointers
	left->_parentNode = node->_parentNode;
	// Update the original node's parent pointer
	node->_parentNode = left;
	// Update heights
	node->setHeight(1 + std::max(getHeight(node->_leftNode), getHeight(node->_rightNode)));
	// Set the new root of the subtree
	node = left;
	// Update height of the original node after rotation
	node->setHeight(1 + std::max(getHeight(node->_leftNode), getHeight(node->_rightNode)));
}

void AVLTree::_rightLeftRotate(std::shared_ptr<BinaryTreeNode>& node) {
	// If the node or its right child is null, return
	if (!node || !node->_rightNode) return;
	// perform right single rotation on the right child
	_rightSingleRotate(node->_rightNode);
	_leftSingleRotate(node);
}

void AVLTree::_leftRightRotate(std::shared_ptr<BinaryTreeNode>& node) {
	// If the node or its left child is null, return
	if (!node || !node->_leftNode) return;
	// perform left single rotation on the left child
	_leftSingleRotate(node->_leftNode);
	_rightSingleRotate(node);
}
