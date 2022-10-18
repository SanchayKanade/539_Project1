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
		cout<<"Root is empty";
		return false;
           }

	//If tree is not empty
	
	else{
		cout<<"starting else block";
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
		cout<<"came out of while \n";
		LeafNode *leafCursor = (LeafNode*)cursor;
		for(int i=0;i<leafCursor->key_num;i++){
			cout<<leafCursor->key_num<<"\n";
			cout<<"Target value is "<<key<<" page_id "<<result.page_id<<"\n";
			cout<<"search value is "<<leafCursor->keys[i]<<" page_id "<<leafCursor->pointers[i].page_id<<"\n";
				
		//	if(leafCursor->keys[i]==key && leafCursor->pointers[i].page_id==result.page_id && leafCursor->pointers[i].record_id==result.record_id){
			if(leafCursor->keys[i]==key){
				result.page_id=leafCursor->pointers[i].page_id;
				cout<<result.page_id;
				return true;
				}
			}
		
	        cout<<"Returning false";	
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



return false;
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
