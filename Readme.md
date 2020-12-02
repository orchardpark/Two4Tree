# Two4Tree

Two4Tree is an implementation of a 2-3-4 tree (also called a 2-4 tree). A 2-4 tree is a self-balancing data structure that can be used to implement dictionairies (ordered key-value mappings).

In particular it supports the following operations:

- insert: Insert new key value pair. Complexity: O(log(n))
- search: Search for key. Complexity: O(log(n))
- delete: Delete key value pair. Complexity: O(log(n))
- rank: Get ordered rank of key. Complexity: O(log(n))
- select: Select the i-th ranked item. Complexity: O(log(n))
