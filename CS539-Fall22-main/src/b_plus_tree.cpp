#include "include/b_plus_tree.h"
#include <iostream>

/*
 * Helper function to decide whether current b+tree is empty
 */
bool BPlusTree::IsEmpty() const { 
	if(!root || root ==NULL){
		return true;
	}
	return false; 
}

/*****************************************************************************
 * SEARCH
 *****************************************************************************/
/*
 * Return the only value that associated with input key
 * This method is used for point query
 * @return : true means key exists
 */
bool BPlusTree::GetValue(const KeyType &key, RecordPointer &result) {
        
	//If tree is empty there is no value to search
	
	if(!root || root ==NULL){          
		//cout<<"Root is empty";
		return false;
           }

	//If tree is not empty
	
	else{
		//cout<<"starting else block";
		InternalNode *cursor = (InternalNode*)root;
		
		while(cursor->is_leaf == false){
			for(int i=0;i<cursor->key_num;i++){
				if(key<cursor->keys[i]){
					cursor = (InternalNode*)cursor->children[i];
				//	cursor = cursor->children[i];
					break;
				}
				if(i==cursor->key_num - 1){
					cursor = (InternalNode*)cursor->children[i+1];
					//cursor = cursor->children[i+1];
					break;
				}
			
			}
		}
		//cout<<"came out of while \n";
		LeafNode *leafCursor = (LeafNode*)cursor;
		for(int i=0;i<leafCursor->key_num;i++){
			if(leafCursor->keys[i]==key){
				result.page_id=leafCursor->pointers[i].page_id;
				//cout<<result.page_id;
				return true;
				}
			}
		
		return false;	
	}
	return false; 
}

/*****************************************************************************
 * INSERTION
 *****************************************************************************/
/*
 * Insert constant key & value pair into b+ tree
 * If current tree is empty, start new tree, otherwise insert into leaf Node.
 * @return: since we only support unique key, if user try to insert duplicate
 * keys return false, otherwise return true.
 */
bool BPlusTree::Insert(const KeyType &key, const RecordPointer &value) { 
	//To check if duplicate value is being inserted.
	RecordPointer one_record;
	if(GetValue(key, one_record)){
		return false;
	}
	
	/*if tree is empty*/
	if(IsEmpty()){

		LeafNode *leafNode = new LeafNode();
		leafNode->keys[0] = key;
		leafNode->pointers[0] = value;
		leafNode->key_num=1;
		leafNode->is_leaf == true;
		root = leafNode;

		return true;

	}
	else{
		//Leaf is present.Could be root.
		InternalNode *cursor = (InternalNode*)root;
		InternalNode *parent;
		while(cursor->is_leaf == false){
			//InternalNode *temp = (InternalNode*)cursor;
			parent = cursor;
			for(int i = 0;i<cursor->key_num;i++){
				if(key<cursor->keys[i]){
					cursor = (InternalNode*)cursor->children[i];
					break;
				}
				if(i == cursor->key_num-1){
					cursor = (InternalNode*)cursor->children[i+1];
					break;
				}
			}
		}
		//Case where leaf node has space left
		LeafNode *cursorl = (LeafNode*)cursor;
		if(cursorl->key_num < MAX_FANOUT-1){
			int i =0;
			while(key > cursorl->keys[i] && i < cursorl->key_num){
				i++;
			}
			for(int j = cursorl->key_num;j > i;j--){
				cursorl->keys[j] = cursorl->keys[j-1]; //shifting keys to right to make space for new key
				cursorl->pointers[j] = cursorl->pointers[j-1]; //shifting records to make space for new record

			}
			cursorl->keys[i] = key;
			cursorl->pointers[i] = value;
			cursorl->key_num++;
			//InternalNode *cursor = (InternalNode*)cursor;
			//cursor->children[cursor->key_num] = cursor->children[cursor->key_num-1];
			//cursor->children[cursor->key_num-1] = NULL;
		}
		//Leaf node has no space left and the key lies between them
		else{
			LeafNode *newLeaf = new LeafNode();
			KeyType tempNode[MAX_FANOUT]; //temp array to store keys from previous leaf
			RecordPointer tempPtr[MAX_FANOUT]; //temp array to store values from previous leaf

			//Copying all keys of that current node into temp node
			for(int i=0;i<MAX_FANOUT-1;i++){
				tempNode[i] = cursorl->keys[i];
				tempPtr[i] = cursorl->pointers[i];
			}
			int i=0, j;
			while(i < MAX_FANOUT-1 && key > tempNode[i]){  //finding the position for key
				i++;	
			}
			//Shifting keys and records 
			for(int j = MAX_FANOUT-1;j>i;j--){ //segmentation fault fixed
				tempNode[j] = tempNode[j-1];
				tempPtr[j] = tempPtr[j-1];
			}
			//Assigning key and value to correct position
			tempNode[i] = key;
			tempPtr[i] = value;
			//Populating new leaf node
			newLeaf->is_leaf = true;
			cursorl->key_num = (MAX_FANOUT)/2; //Finding the middle of the current leaf for spliting
			newLeaf->key_num = MAX_FANOUT - (MAX_FANOUT)/2; //Size of new leaf
			//Copying data from temp to cursor and newLeaf
			for(i = 0;i<cursorl->key_num;i++){
				cursorl->keys[i] = tempNode[i];
				cursorl->pointers[i] = tempPtr[i];
			}
			for(i=0, j = cursorl->key_num; i<newLeaf->key_num; i++,j++){
				newLeaf->keys[i] = tempNode[j];
				newLeaf->pointers[i] = tempPtr[j];
			}
			//To link two leaf nodes together
			cursorl->next_leaf = newLeaf;
			newLeaf->prev_leaf = cursorl;

			//If cursor is the leaf and root node
			//Node *cursorR = (Node*)cursorl;
			if(cursorl == root){
				
				InternalNode *newRoot = new InternalNode();
				newRoot->keys[0] = newLeaf->keys[0];
				newRoot->children[0] = cursorl;
				newRoot->children[1] = newLeaf;
				newRoot->is_leaf = false;
				newRoot->key_num = 1;
				root = newRoot;
				
			}
			// If there are multiple internal nodes and leafs
			else{
				FillInternal(newLeaf->keys[0], parent, (InternalNode*)newLeaf);
			}
		}
	
	}
for(int i=0;i<root->key_num;i++){
	cout<<root->keys[i]<<" ";
}
cout<<endl;
return true;
}

/****************************************************************************
 * FILLINTERNAL HELPER FUNCTION
 * **************************************************************************/
/* Helps to copy leaf node keys to leaf nodes above
 * Author : Sanchay Kanade (sk2656)
 * Date Created : 10/19/2022*/
/***************************************************************************/
void BPlusTree::FillInternal(const KeyType &key, InternalNode *parent, InternalNode *newLeaf){

	//If intrenal node has space then just add the key
	InternalNode *cursor = parent;
	if(cursor->key_num < MAX_FANOUT-1){
		int i = 0;
		while(key > cursor->keys[i] && i<cursor->key_num){
			i++;
		}
		for(int j = cursor->key_num; j>i; j--){
			cursor->keys[j] = cursor->keys[j-1];   //shift element to make space

		}
		for(int j = cursor->key_num+1;j>i+1;j--){
			cursor->children[j] = (InternalNode*)cursor->children[j-1]; ////assign last current pointer to previous pointer
		}
		cursor->keys[i] = key;
		cursor->key_num++;
		cursor->children[i+1] = (InternalNode*)newLeaf;
	}
	//If internal node does not have space then we nee dto split internal node and create a new root
	else{
		InternalNode *internalNode = new InternalNode();
		KeyType tempKey[MAX_FANOUT]; //temp array to store keys of parent
		Node *tempPtr[MAX_FANOUT+1]; //temp array to store pointers of parents
		for(int i=0; i<MAX_FANOUT-1;i++){
			tempKey[i] = cursor->keys[i]; // copying keys
		}
		for(int i=0; i<MAX_FANOUT;i++){
                        tempPtr[i] = (InternalNode*)cursor->children[i]; // copying keys
                }
		int i=0,j;
		while(key > tempKey[i] && i<MAX_FANOUT-1){
			i++;           //finding place for child key to go
		}
		for(int j = MAX_FANOUT-1; j>i; j--){
			tempKey[j] = tempKey[j-1]; //shifting other keys to make value for new key

		}
		tempKey[i] = key; //Inserting new key in its place
		for(int j = MAX_FANOUT;j>i+1; j--){
			tempPtr[j] = tempPtr[j-1]; //shifting pointers to make space for child pointer
		}
		//Temp node is ready
		tempPtr[i+1] = (InternalNode*)newLeaf; //inserting child pointer
		internalNode->is_leaf = false;
		cursor->key_num = (MAX_FANOUT)/2; //creating split point
		internalNode->key_num = MAX_FANOUT - (MAX_FANOUT)/2 - 1; 
		for(i = 0, j = cursor->key_num+1; i<internalNode->key_num && j<MAX_FANOUT;i++, j++){
			internalNode->keys[i] = tempKey[j]; //populating internalNode->keys[i] = tempKeys[j] from split point
		} 
		for(i=0, j=cursor->key_num+1;i<internalNode->key_num+1;i++, j++){
			internalNode->children[i]=(InternalNode*)tempPtr[j];
		}

		if(cursor == (InternalNode*)root){
			// if leaf is root node now we will copy leaf values to root
			InternalNode *cursorR = (InternalNode*)cursor;
                        InternalNode *newInternalNode = new InternalNode();
                        newInternalNode->keys[0] = tempKey[MAX_FANOUT/2];
                        newInternalNode->children[0] = cursorR;
                        newInternalNode->children[1] = internalNode;
                        newInternalNode->is_leaf = false;
                        newInternalNode->key_num = 1;
                        root = (Node*)newInternalNode;
			//cout<<root->keys[0]<<endl;
	
		}
		else{
			FillInternal(cursor->keys[cursor->key_num], FindParent(root,cursor), internalNode);
		}

	
	}

}

/*****************************************************************************
 * FIND PARENT
 *****************************************************************************/
/*Finds parent node for a given child node with a particular key
 *Author : Sanchay Kanade (sk2656)
 *Date Created : 10/19/2022*/


InternalNode* BPlusTree::FindParent(Node *root, InternalNode *cursor){
	InternalNode *parent;
	InternalNode *temp = (InternalNode*)root;
	if(temp->is_leaf || temp->children[0]->is_leaf){
		return NULL;
	}
	for(int i=0;i<temp->key_num+1;i++){
		if(temp->children[i] == cursor){
			parent = temp;
			return parent;
		}
		else{
			parent = FindParent(temp->children[i], cursor);
			if(parent != NULL){
				return parent;
			}
		}
	}
	return parent;
 
 } 






/*****************************************************************************
 * REMOVE
 *****************************************************************************/
/*
 * Delete key & value pair associated with input key
 * If current tree is empty, return immdiately.
 * If not, User needs to first find the right leaf node as deletion target, then
 * delete entry from leaf node. Remember to deal with redistribute or merge if
 * necessary.
 */
void BPlusTree::Remove(const KeyType &key) {
	RecordPointer one_record;
	//If tree is empty
	if(IsEmpty()){
		return;
	}
	//If the given key is not present
	else if(!IsEmpty() && !GetValue(key, one_record)){
		return;
	}
	//If the key is present
	else{
		InternalNode *cursor = (InternalNode*) root;
		InternalNode *parent;
		KeyType leftSib, rightSib;  //declaring left and right siblings
		//Implementing search logic
		while(cursor->is_leaf == false){
			for(int i =0; i<cursor->key_num; i++){
				parent = cursor;
				leftSib = i-1;
				rightSib = i+1;
				if(key<cursor->keys[i]){
					cursor = (InternalNode*)cursor->children[i];
					break;
				}
				if(i == cursor->key_num-1){
					leftSib = i;
					rightSib = i+2;
					cursor = (InternalNode*)cursor->children[i+1];
					break;
				}
			}
		}
		//we have reached leaf
		LeafNode* cursorl = (LeafNode*)cursor;
		//Find the position for the key in that node
		bool found = false;
		int pos;
		//Find position of search key in the keys of leaf node
		for(pos=0; pos<cursorl->key_num; pos++){
			if(cursorl->keys[pos] == key){
				found = true;
				break;
			}
		}
		for(int i=pos; i<cursorl->key_num; i++){
			cursorl->keys[i] = cursorl->keys[i+1];
			cursorl->pointers[i] = cursorl->pointers[i+1];
		}
		cursorl->key_num--;
		if(cursorl == (LeafNode*)root){
			//for(int i=0; i<MAX_FANOUT;i++){
			//	cursor->children[i] = NULL;
			//}
			//No nodes left in the tree after removal
			if(cursorl->key_num == 0){
				delete[] cursorl->keys;
				delete[] cursorl->pointers;
				delete cursorl;
				root = NULL;
			}
			return;
		}
		//This is the normal case.
		//cursorl->prev_leaf = cursor->children[cursor->key_num+1];
		//cursor->children[cursor->key_num] = NULL;
		if(cursorl->key_num >= (MAX_FANOUT)/2){
				return; //becasue half the keys should be full
		}
		if(leftSib >= 0){
			LeafNode *leftNode = (LeafNode*)parent->children[leftSib]; //this is a pointer to the left left node of cursorl. could be optimized
			if(leftNode->key_num >= (MAX_FANOUT)/2 + 1){
				for(int i=cursorl->key_num; i>0; i--){
					cursorl->keys[i] = cursorl->keys[i-1]; //copying key to right
					cursorl->pointers[i] = cursorl->pointers[i-1];
				}
				cursorl->key_num++;
				//cursorl->children[cursor->key_num] = cursor->children[cursor->key_num - 1];
				//cursorl->children[cursor->key_num -1] = NULL;
				//Pushing greatest value of left sibling
				cursorl->keys[0] = leftNode->keys[leftNode->key_num -1];
				leftNode->key_num--;
				leftNode->next_leaf = cursorl;
				//leftNode->children[leftNode->key_num + 1] = NULL;
				parent->keys[leftSib] = cursorl->keys[0];
				return;
			}
		}
		if(rightSib <= parent->key_num){
			LeafNode *rightNode = (LeafNode*)parent->children[rightSib];
			if(rightNode->key_num >= (MAX_FANOUT)/2 + 1){
				cursorl->key_num++;
				cursorl->keys[cursorl->key_num-1] = rightNode->keys[0];
				cursorl->pointers[cursor->key_num-1] = rightNode->pointers[0];
				rightNode->key_num--;
				rightNode->prev_leaf = cursorl;
				//pointer condition
				for(int i = 0;i<rightNode->key_num; i++){
					rightNode->keys[i] = rightNode->keys[i+1];
				}
				parent->keys[rightSib-1] = rightNode->keys[0];
				return;
			}

			
		}
		if(leftSib >=0){
			LeafNode *leftNode = (LeafNode*)parent->children[leftSib];
			for(int i = leftNode->key_num, j=0; j< cursorl->key_num; i++,j++){
				leftNode->keys[i] = cursorl->keys[j];
				leftNode->pointers[i] = cursorl->pointers[j];
			}
			leftNode->key_num += cursorl->key_num;
			leftNode->next_leaf = cursorl;
			/*DeleteInternal(parent->keys[leftSib], parent, (InternalNode*)cursorl);
			delete[] cursorl->keys;
			delete[] cursorl->pointers;
			delete cursor;*/
		}
		else if(rightSib<=parent->key_num){
			LeafNode *rightNode = (LeafNode*)parent->children[rightSib];
			for(int i = cursorl->key_num, j=0; j<rightNode->key_num; i++,j++){
				cursorl->keys[i] = rightNode->keys[j];
				cursorl->pointers[i] = rightNode->pointers[j];
			}
			cursorl->key_num += rightNode->key_num;
			cursorl->next_leaf = rightNode;
			/*DeleteInternal(parent->keys[rightSib -1], parent, (InternalNode*)rightNode);
			delete [] rightNode->keys;
			delete [] rightNode->pointers;
			delete rightNode;*/	
		}

	}
}
/*****************************************************************************
 * DELETE INTERNAL
 *****************************************************************************/
/*Deletes internal nodes
* Author : Sanchay Kanade (sk2656)
* Date Created : 10/20/2022*/
/***************************************************************************/
void BPlusTree::DeleteInternal(const KeyType &key, InternalNode *parent, InternalNode *child){
	//start here
	if(parent == (InternalNode*)root){
		if(parent->key_num ==1){
			if(parent->children[1] ==child){
				delete[] child->keys;
				delete[] child->children;
				delete child;
				root = (Node*)parent->children[0];
				delete[] parent->keys;
				delete[] parent->children;
				delete parent;
				return;
			}
			else if(parent->children[0]==child){
				delete[] child->keys;
				delete[] child->children;
				delete child;
				root = (Node*)parent->children[1];
				delete[] parent->keys;
				delete[] parent->children;
				delete parent;
				return;
				
			}
		}
	}
	int pos;
	for(pos =0; pos<parent->key_num;pos++){
		if(parent->keys[pos] == key){
			break;
		}
	}
	for(int i = pos; i<parent->key_num; i++){
		parent->keys[i] = parent->keys[i+1];
	}
	for(pos = 0; pos<parent->key_num+1; pos++){
		if(parent->children[pos]==child){
			break;
		}
		
	}
	for(int i=pos; i<parent->key_num+1;i++){
		parent->children[i] = parent->children[i+1];
	}
	parent->key_num--;
	if(parent->key_num >= (MAX_FANOUT)/2 - 1){
		return;
	}
	if(parent == (InternalNode*)root){
		return;
	}
	InternalNode *parentN = FindParent(root, parent);
	int leftSib, rightSib;
	for(pos=0; pos<parentN->key_num+1; pos++){
		if(parentN->children[pos] == parent){
			leftSib = pos-1;
			rightSib = pos+1;
			break;
		}
	}
	if(leftSib>=0){
		InternalNode* leftNode = (InternalNode*)parentN->children[leftSib];
		if(leftNode->key_num >=(MAX_FANOUT)/2){
			for(int i=parent->key_num;i>0;i--){
				parent->keys[i] = parent->keys[i-1];
			}
			parent->keys[0] = parentN->keys[leftSib];
			parentN->keys[leftSib] = leftNode->keys[leftNode->key_num - 1];
			for (int i = parent->key_num + 1; i > 0; i--) {
				parent->children[i] = parent->children[i - 1];
			}
			parent->children[0] = leftNode->children[leftNode->key_num];
			parent->key_num++;
			leftNode->key_num--;
			return;
		}
	}
	if(rightSib <=parentN->key_num){
		InternalNode *rightNode = (InternalNode*)parentN->children[rightSib];
		if(rightNode->key_num >=(MAX_FANOUT)/2){
			parent->keys[parent->key_num] = parentN->keys[pos];
			parentN->keys[pos] = rightNode->keys[0];
			for(int i =0;i<rightNode->key_num - 1;i++){
				rightNode->keys[i] = rightNode->keys[i+1];
			}
			parent->children[parent->key_num + 1] = rightNode->children[0];
			for(int i=0;i>rightNode->key_num;++i){
				rightNode->children[i] = rightNode->children[i+1];
			}
			parent->key_num++;
			rightNode->key_num--;
			return;
			
		}	
	}
	if(leftSib>=0){
		InternalNode* leftNode = (InternalNode*)parentN->children[leftSib];
		leftNode->keys[leftNode->key_num] = parentN->keys[leftSib];
	        for (int i = leftNode->key_num + 1, j = 0; j < parent->key_num; j++){
	            leftNode->keys[i] = parent->keys[j];
        	}
        	for (int i = leftNode->key_num + 1, j = 0; j < parent->key_num + 1; j++){
           		 leftNode->children[i] = parent->children[j];
            		 parent->children[j] = NULL;
        	}
        	leftNode->key_num += parent->key_num + 1;
        	parent->key_num = 0;
        	DeleteInternal(parentN->keys[leftSib], parentN, parent);
	}
	else if(rightSib <= parentN->key_num){
		InternalNode *rightNode = (InternalNode*)parentN->children[rightSib];
		parent->keys[parent->key_num] = parentN->keys[rightSib - 1];
		for (int i = parent->key_num + 1, j = 0; j < rightNode->key_num; j++){
    		parent->keys[i] = rightNode->keys[j];
		}
		for (int i = parent->key_num + 1, j = 0; j < rightNode->key_num + 1; j++){
		    parent->children[i] = rightNode->children[j];
		    rightNode->children[j] = NULL;
		}
		parent->key_num += rightNode->key_num + 1;
		rightNode->key_num = 0;
		DeleteInternal(parentN->keys[rightSib - 1], parentN, rightNode);
	}


}




/*****************************************************************************
 * RANGE_SCAN
 *****************************************************************************/
/*
 * Return the values that within the given key range
 * First find the node large or equal to the key_start, then traverse the leaf
 * nodes until meet the key_end position, fetch all the records.
 */
void BPlusTree::RangeScan(const KeyType &key_start, const KeyType &key_end,
                          std::vector<RecordPointer> &result) {
	//Start Here
	       if(!root || root ==NULL){
                return;
           }

        //If tree is not empty

        else{
                InternalNode *cursor = (InternalNode*)root;
		//Search the leaf node where to  closest key to key_start is.
                while(cursor->is_leaf == false){
                        for(int i=0;i<cursor->key_num;i++){
                                if(key_start<cursor->keys[i]){
                                        cursor = (InternalNode*)cursor->children[i];
                                //      cursor = cursor->children[i];
                                        break;
                                }
                                if(i==cursor->key_num - 1){
                                        cursor = (InternalNode*)cursor->children[i+1];
                                        //cursor = cursor->children[i+1];
                                        break;
                                }

                        }
                }
		int pos;  //postion to store index of key_start or element closest to key_start.
		//Finding the position of key_start or element closest to key_start in leaf node.
                LeafNode *leafCursor = (LeafNode*)cursor;
                for(int i=0;i<leafCursor->key_num;i++){
                        if(leafCursor->keys[i]>=key_start){
                                pos = i;
				break;
                                }
                }
		//Storing all the records in the result vector till we reach key_end or element just lee than key_end
		while(leafCursor!=NULL && leafCursor->keys[pos]<=key_end){
			int j;
			for(j=pos; j<leafCursor->key_num && leafCursor->keys[j]<=key_end;j++){
				result.push_back(leafCursor->pointers[j]);
			}
			//If end of the leaf node is reached, jump to next node if key_end is not reched
			if(j==leafCursor->key_num || leafCursor->keys[j]>=key_end){
				pos = 0;
				leafCursor = leafCursor->next_leaf;
			
			}
		}
				
        }

}
