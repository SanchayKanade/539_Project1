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
			//cout<<leafCursor->key_num<<"\n";
			//cout<<"Target value is "<<key<<" page_id "<<result.page_id<<"\n";
			//cout<<"search value is "<<leafCursor->keys[i]<<" page_id "<<leafCursor->pointers[i].page_id<<"\n";
				
		//	if(leafCursor->keys[i]==key && leafCursor->pointers[i].page_id==result.page_id && leafCursor->pointers[i].record_id==result.record_id){
			if(leafCursor->keys[i]==key){
				result.page_id=leafCursor->pointers[i].page_id;
				//cout<<result.page_id;
				return true;
				}
			}
		
	        //cout<<"Returning false";	
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
RecordPointer one_record;
if(GetValue(key, one_record)){
		return false;
		}
	
	/*if tree is empty*/
	if(IsEmpty()){
		/*root = new Node(false);
		LeafNode *temp;
		temp = new LeafNode;
		temp->is_leaf = true;
		temp->keys[0] = key;
		temp->pointers[0] = value;
		root->keys[0] = key;
		root->key_num = 1;*/

		LeafNode *leafNode = new LeafNode();
		leafNode->keys[0] = key;
		leafNode->pointers[0] = value;
		leafNode->key_num=1;
		leafNode->is_leaf == true;
		root = leafNode;

		return true;
		
		

	}
	else{
		//start here
		//Leaf is present.Could be root.
		//Node *cursor = root;
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
				cursorl->keys[j] = cursorl->keys[j-1]; //shifting keys to right to amke space for new key
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
			LeafNode *newLeaf = new LeafNode;
			KeyType tempNode[MAX_FANOUT]; //temp array to store keys from previous leaf
			RecordPointer tempPtr[MAX_FANOUT]; //temp array to store values from previous leaf

			//Copying all keys of that current node into temp node
			for(int i=0;i<MAX_FANOUT-1;i++){
				tempNode[i] = cursorl->keys[i];
				tempPtr[i] = cursorl->pointers[i];
			}
			int i=0, j;
			while(key > tempNode[i] && i < MAX_FANOUT-1){  //finding the position for key
				i++;	
			}
			//Shifting keys and records 
			for(int j = MAX_FANOUT;j>i;j--){
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
			InternalNode *cursorR = (InternalNode*)cursorl;
			InternalNode *temp = (InternalNode*)root;
			if(cursorR == temp){
				InternalNode *newInternalNode = new InternalNode;
				newInternalNode->keys[0] = newLeaf->keys[0];
				newInternalNode->children[0] = cursor;
				InternalNode *newLeaf = (InternalNode*)newLeaf;
				newInternalNode->children[1] = newLeaf;
				newInternalNode->is_leaf = false;
				newInternalNode->key_num = 1;
				Node *temp = (Node*)newInternalNode;
				root = newInternalNode;
				
			}
			// If there are multiple internal nodes and leafs
			else{
				FillInternal(newLeaf->keys[0], parent, (InternalNode*)newLeaf);
			}
		}
	
	}



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
	else{
		InternalNode *internalNode = new InternalNode;
		KeyType tempKey[MAX_FANOUT]; //temp array to store keys of parent
		InternalNode *tempPtr[MAX_FANOUT+1]; //temp array to store pointers of parents
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
		for(int j = MAX_FANOUT; j>i; j--){
			tempKey[j] = tempKey[j-1]; //shifting other keys to make value for new key

		}
		tempKey[i] = key; //Inserting new key in its place
		for(int j = MAX_FANOUT+1;j>i+1; j--){
			tempPtr[j] = tempPtr[j-1]; //shifting pointers to make space for child pointer
		}
		//Temp node is ready
		tempPtr[i+1] = (InternalNode*)newLeaf; //inserting child pointer
		internalNode->is_leaf = false;
		cursor->key_num = (MAX_FANOUT)/2; //creating split point
		internalNode->key_num = MAX_FANOUT - 1 + (MAX_FANOUT)/2; 
		for(i = 0, j = cursor->key_num+1; i<internalNode->key_num;i++, j++){
			internalNode->keys[i] = tempKey[j]; //populating internalNode->keys[i] = tempKeys[j] from split point
		} 
		for(i=0, j=cursor->key_num+1;i<internalNode->key_num+1;i++, j++){
			internalNode->children[i]=(InternalNode*)tempPtr[j];
		}

		if(cursor == (InternalNode*)root){
			// if leaf is root node now we will copy leaf values to root
			InternalNode *cursorR = (InternalNode*)cursor;
                        InternalNode *newInternalNode = new InternalNode;
                        newInternalNode->keys[0] = newLeaf->keys[cursorR->key_num];
                        newInternalNode->children[0] = cursorR;
                        newInternalNode->children[1] = internalNode;
                        newInternalNode->is_leaf = false;
                        newInternalNode->key_num = 1;
                        root = (Node*)newInternalNode;
	
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
void BPlusTree::Remove(const KeyType &key) {}

/*****************************************************************************
 * RANGE_SCAN
 *****************************************************************************/
/*
 * Return the values that within the given key range
 * First find the node large or equal to the key_start, then traverse the leaf
 * nodes until meet the key_end position, fetch all the records.
 */
void BPlusTree::RangeScan(const KeyType &key_start, const KeyType &key_end,
                          std::vector<RecordPointer> &result) {}
