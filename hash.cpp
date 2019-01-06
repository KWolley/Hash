// hash.cpp

#include "hash.h"
#include <iostream>

using namespace std;

// init_table creates and initializes a hash_table structure and
// returns a pointer to it. 
hash_table* init_table(unsigned int cap) {
  hash_table* tbl = new hash_table;
  tbl -> capacity = cap;
  tbl -> size = 0;
  tbl -> occupied = 0;
  tbl -> hash_func = djb2;
  tbl -> table = new hash_node*[cap]; // causes segment fault
  for (int i = 0 ; i < cap ; i++){
    tbl -> table[i] = nullptr;
  }
  tbl -> bucket_func = modulo_bucket_func;
  return tbl;
}

// init_node creates and initializes a hash_node that will occupy a
// hash table bucket.
hash_node* init_node(std::string key, unsigned int hashcode, std::string val) {
  hash_node* nd = new hash_node;
  nd -> deleted = false;
  nd -> key = key;
  nd -> value = val;
  nd -> hashcode = hashcode;
  return nd; // TODO
}

// set_kvp establishes a mapping between the given key and value pair
// in the provided hash table. if the key (as identified by its hash
// code) is already in the hash table, its value is potentially
// updated. otherwise a new key/value pair is added to the hash table.
//
// on exit, the size and occupancy of the hash table is increased if a
// new key/value pair was added.
//
// if there is no room in the hash table, return false. do not resize
// the table.
//
// the return value should be true unless there was no room in the
// hash table.
bool set_kvp(hash_table* tbl, string key, string value) {
  unsigned int hash_code = djb2(key);  // get hash for key
  unsigned int bucket = modulo_bucket_func(hash_code,tbl-> capacity); // calc bucket
  
  unsigned int i = bucket;  // create counter to traverse through linked list
  while (tbl -> table[i] != nullptr){ 
    if (tbl -> table[i] -> hashcode == hash_code){
      tbl -> table[i] -> value = value;
      return true;
    }
    i = (i+1) % tbl -> capacity; // reset hash_val to next bucket and to wrap around list
    if (i == bucket){
      return false;
    }
  }
  tbl -> table[i] = init_node(key,hash_code,value);
  tbl -> size = tbl -> size + 1;
  tbl -> occupied = tbl -> occupied + 1;
  return true; 
}

// load returns a load factor describing how 'full' the table
// is. because we are using linear probing, which leaves 'deleted'
// hash nodes sitting around, it is reasonable to use occupied divided
// by capacity. you can alternately use size divided by capacity,
// which is a more canonical definition of hashtable load. either will
// work with the unit tests.
float load(hash_table* tbl) {
  float sz = tbl -> size;
  float cp = tbl -> capacity;
  return sz/cp; 
}

// get_val returns the value associated with the provded key in the
// table, or the empty string "" if no such mapping exists (or if
// there is a mapping but it is deleted).
// contains - except return value
string get_val(hash_table* tbl, std::string key) {
  unsigned int hash_code = djb2(key);  // get hash for key
  unsigned int bucket = modulo_bucket_func(hash_code,tbl-> capacity); // calc bucket

  unsigned int i = bucket;
  while(tbl -> table[i] != nullptr){
    if(tbl -> table[i] -> hashcode = hash_code){
      return tbl -> table[i] -> value;
    }
    i = (i+1) % tbl -> capacity; // reset hash_val to next bucket and to wrap around list
    if (i == bucket){
      return "";
    }
  }
  return "";
}

// contains returns true if the table has a non-deleted node whose
// hashcode matches the given key's hashcode.
bool contains(hash_table* tbl, std::string key) {
  unsigned int hash_code = djb2(key);  // get hash for key
  unsigned int bucket = modulo_bucket_func(hash_code,tbl-> capacity); // calc bucket

  unsigned int i = bucket;
  while(tbl -> table[i] != nullptr){
    if(tbl -> table[i] -> hashcode = hash_code){
      return true;
    }
    i = (i+1) % tbl -> capacity; // reset hash_val to next bucket and to wrap around list
    if (i == bucket){
      return false;
    }
  }
  return false;
  }

// remove marks as 'deleted' any existing non-deleted hash_node with
// the same hashcode as the provided key's hashcode. if it finds such
// a hash node, it decrements the table's size (but not occupancy) and
// returns true. it returns false otherwise.
bool remove(hash_table* tbl, std::string key) {
  // overwrite value to null
  // set deleted to true
  unsigned int hash_code = djb2(key);  // get hash for key
  unsigned int bucket = modulo_bucket_func(hash_code,tbl-> capacity); // calc bucket

  unsigned int i = bucket;
  while(tbl -> table[i] != nullptr){
    if(tbl -> table[i] -> hashcode = hash_code){
      tbl -> size = tbl -> size - 1;
      //tbl -> table[i] = nullptr;
      tbl -> table[i] -> value = "";
      tbl -> table[i] -> key = "";
      //tbl -> table[i] -> hashcode = 0; 
      tbl -> table[i] -> deleted = true;
      return true;
    }
    i = (i+1) % tbl -> capacity; // reset hash_val to next bucket and to wrap around list
    if (i == bucket){
      return false;
    }
  }
  return false;
}

// resize creates a new underlying backing array (tbl->table) with the
// provided new capacity, rehashes the existing backing array into the
// new array. on exit, the hash table's fields and functions
// accurately reflect the hash table's current state.
//
// this is an extra-credit function.
void resize(hash_table* tbl, unsigned int new_capacity) {
  // init section
  int  num_vals = tbl -> size; // calc number of values in old table
  unsigned int transfer_hashcodes[num_vals]; // create array to store hashcodes
  std::string transfer_values[num_vals]; // create array to store values
  std::string transfer_key[num_vals]; // create array to store keys
  
  // save values in old table
  unsigned int a = 0;   // creeate counter for arrays
  for(int k = 0; k < tbl ->  capacity ; k++){ //for each value in the table
    if (tbl -> table[k] != nullptr && tbl ->table[k]-> deleted == false){ // if the current value of the table is not empty
      transfer_hashcodes[a] = tbl -> table[k] -> hashcode; // save hashcode
      transfer_values[a] = tbl -> table[k] -> value; // save value inside table
      transfer_key[a] = tbl -> table[k] -> key; // save key
      a++; // increment a
    }
  }
  // clear table and reset capacity - set all buckets to null
  tbl -> table = new hash_node*[new_capacity]; 
  tbl -> capacity = new_capacity; // set table to new_capacity
  for (int i = 0 ; i < new_capacity ; i++){ // change all table values to null
    tbl -> table[i] = nullptr;
  }
  tbl -> size = 0;
  tbl -> occupied = 0;
  // traverse through arrays and insert each value into new table
  for (int c = 0; c < num_vals ; c++){  // for each value of the arrays
    set_kvp(tbl, transfer_key[c],transfer_values[c]);
  }
  return;
}

// implemented for you - feel free to change this one
void print_table(hash_table* tbl) {
  cout << "Hashtable:" << endl;
  cout << "  capacity: " << tbl->capacity << endl;
  cout << "  size:     " << tbl->size << endl;
  cout << "  occupied: " << tbl->occupied << endl;
  cout << "  load:     " << load(tbl) << endl;
  if (tbl->capacity < 130) {
    for (unsigned int i=0; i < tbl->capacity; i++) {
      cout << "[" << i << "]    ";
      if (tbl->table[i] == NULL) {
        cout << "<empty>" << endl;
      } else if (tbl->table[i]->deleted) {
        cout << "<deleted>" << endl;
      } else {
        cout << "\"" << tbl->table[i]->key << "\" = \"" << tbl->table[i]->value << "\"" << endl;
      }
    }
  } else {
    cout << "    <hashtable too big to print out>" << endl;
  }
  return;
}

// implemented for you - don't change this one
unsigned int djb2(string key) {
  unsigned int hash = 5381;
  // Leaving the debugging stuff commented out, in case you want to
  // play. It will output the hash at each incremental step so you can
  // see how it takes shape.
  //
  //  cout << "Hashing string \"" << key << "\"" << endl;
  //  cout << bitset<32>(hash) << " " << hash << endl;
  for (size_t i=0; i < key.length(); i++) {
    char c = key[i]; 
    hash = ((hash << 5) + hash) + c;
    // cout << bitset<32>(hash) << " " << hash << endl;
  }
  // cout << "Hashed string \"" << key << "\" to " << hash << endl;
  //
  // for light reading on why djb2 might not be the best
  // production-grade hash function, go to
  // http://dmytry.blogspot.com/2009/11/horrible-hashes.html
  return hash;
}

// implemented for you - don't change this one
unsigned int modulo_bucket_func(unsigned int hashcode, unsigned int cap) {
  unsigned int b = hashcode % cap;
  return b;
}
