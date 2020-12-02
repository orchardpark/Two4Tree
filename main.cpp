#include <iostream>
#include <algorithm>
#include <vector>
#include <random>
using namespace std;
#include "Two4Tree.cpp"

int main(){
	string K[10] = {"A","B","C","D","E","F","G","H","I","K"};
	int V[10] = {10,9,8,7,6,5,4,3,2,1};
	
	Two4Tree<string,int> T1, T2(K,V,10);
	
	for(int i=0; i<10; i++) T1.insert(K[i],V[i]);
	// T1 and T2 should be identical trees

	cout << *(T2.search("C")) << endl;
	// Should output 8
	
	cout << T1.rank("C") << endl;
	//Should output 3
	
	cout << T1.select(3) << endl;
	//Should output C 
	
	T2.preorder();
	//Should output D B A C F H E G I K
	
	T2.inorder();
	//Should output	A B C D E F G H I K
	
	cout << T2.remove("C") << endl;
	//Should output 1
	
	T2.preorder();
	//Should output F B A D E H G I K
	
	T2.inorder();
	//Should output	A B D E F G H I K
	
	cout << T2.remove("J") << endl;
	//Should output 0
	
	cout <<T2.rank("G") << endl;
	//Should output 6
	
	cout <<T1.successor("C")<<endl;
	//Should output D
	
	cout <<T1.predecessor("C")<<endl;
	//Should output B		


    Two4Tree<int,int> X;
	for (int i=1;i<100000;i++) X.insert(i,i);
	for (int i=1;i<100000;i++) {
		if(X.rank(i) != i) cout << "Rank error" << endl;
		if(X.select(i) != i) cout << "Select error" << endl;
		if(*(X.search(i)) != i) cout << "Search error" << endl;
	}
	//Should be no output and should take seconds, not minutes

	// Extra test case
	int keys[] = {10,9,8,7,6,5,4,3,2,1};
	int values[] = {1,2,3,4,5,6,7,8,9,10};
	Two4Tree<int, int> T3(keys, values, 10);
	cout << T3.remove(5) << endl;
	// Should output 1
	T3.postorder();
	// Should output 10 9 8 7 6 4 3 1 2
	T3.inorder();
	// Should output 1 2 3 4 6 7 8 9 10
	cout << T3.size() << endl;
	// Should output 9

	// Extra test case

	int num_elements = 15;
	vector<int> keyvector;
	for(int i=0; i<num_elements; i++) keyvector.push_back(i);
	shuffle(keyvector.begin(), keyvector.end(), default_random_engine(1234L));
	Two4Tree<int, int> T4;
	for(int i=0; i<num_elements; i++) T4.insert(keyvector[i], 1);
	T4.print_tree();
	T4.remove(10);
	T4.postorder();
	// Shout output 12 13 14 11 9 8 5 6 7 4 2 3 1 0

	return 0;
}
