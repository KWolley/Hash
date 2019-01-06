//
// hash_test.cpp
//

#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include <iostream>
#include "hash.h"
#include <string>
#include <cstdio>
#include <cstdlib>
#include <ctime>

using namespace std;

// forward declaration of helpers
string mkstring(string label, int num);
string random_string(size_t length);
void print_short(hash_table* tbl, string msg);
  
TEST_CASE("Hash: sanity check", "[sanity]") {
  REQUIRE(true);
}

TEST_CASE("Hash: init node", "[init node]") {
  hash_node* n = init_node("some key", 1234, "some value");
  REQUIRE(n != NULL);
  REQUIRE(n->key == "some key");
  REQUIRE(n->hashcode == 1234);
  REQUIRE(n->value == "some value");
  REQUIRE(n->deleted == false);
}

TEST_CASE("Hash: init table", "[init table]") {
  hash_table* tbl = init_table(16);
  REQUIRE(tbl != NULL);
  REQUIRE(tbl->capacity == 16);
  REQUIRE(tbl->size == 0);
  REQUIRE(tbl->occupied == 0);
  REQUIRE(tbl->table != NULL);
  REQUIRE(tbl->hash_func != NULL);
  REQUIRE(tbl->bucket_func != NULL);
  for (unsigned int i=0; i < tbl->capacity; i++) {
    REQUIRE(tbl->table[i] == NULL);
  }
}

TEST_CASE("Hash: hash func", "[hash func]") {
    // requires init_table
  hash_table* tbl = init_table(16);
  REQUIRE(tbl->hash_func("") == 5381);
  REQUIRE(tbl->hash_func("sample text") == 3112731660);
}

TEST_CASE("Hash: bucket func", "[bucket func]") {
  // requires init_table
  hash_table* tbl = init_table(16);
  REQUIRE(tbl->bucket_func(40, tbl->capacity) == 8);  
}

// ************* GOOD TO HERE ***********
TEST_CASE("Hash: set key/value pair", "[set]") {
  // requires init_table, hash_func, bucket_func
  bool ret_val;
  hash_table* tbl = init_table(16);
  ret_val = set_kvp(tbl, "fantastic key", "value for fantastic key");
  REQUIRE(ret_val);
  unsigned int hashcode = tbl->hash_func("fantastic key");
  unsigned int bucket_idx = tbl->bucket_func(hashcode, tbl->capacity);
  REQUIRE(bucket_idx >= 0);
  REQUIRE(bucket_idx < tbl->capacity);
  REQUIRE(tbl->table[bucket_idx] != NULL);
  REQUIRE(tbl->table[bucket_idx]->value == "value for fantastic key");
  
  // same string means same hashcode; should place it in same spot
  ret_val = set_kvp(tbl, "fantastic key", "value for the second fantastic key");
  REQUIRE(ret_val);
  REQUIRE(tbl->table[bucket_idx] != NULL);
  REQUIRE(tbl->table[bucket_idx]->value == "value for the second fantastic key");

  // now fill the thing up. there should be room for all of this.
  for (int i=0; i < 15; i++) {
    string k = mkstring("some key", i);
    string v = mkstring("some value", i);
    ret_val = set_kvp(tbl, k, v);
    REQUIRE(ret_val);
  }

  // the next set_kvp should be rejected because it is 100% full.
  ret_val = set_kvp(tbl, "wafer", "just one more little waferrrrr");
  REQUIRE_FALSE(ret_val);
}

TEST_CASE("Hash: load", "[load]") {
  // requires init_table and set_kvp
  hash_table* tbl = init_table(16);
  float last_load = 0;
  float this_load = 0;
  REQUIRE(load(tbl) == last_load); // sanity check
  for (unsigned int i=0; i < tbl->capacity; i++) {
    string k = mkstring("some key", i);
    string v = mkstring("some value", i);
    set_kvp(tbl, k, v);
    this_load = load(tbl);
    REQUIRE(last_load < this_load); // load should go up every time
    last_load = this_load;
  }
  set_kvp(tbl, "last string", "first string");
  this_load = load(tbl);
  REQUIRE(last_load == this_load); // load stays same since tbl is full
}

TEST_CASE("Hash: get", "[get]") {
  // requires init_table and set_kvp
  hash_table* tbl = init_table(16);
  string response;
  
  // first ensure getting a value that isn't there returns empty string
  response = get_val(tbl, "favorite color");
  REQUIRE(response == "");
  
  // now set that key/value pair so it has a value
  set_kvp(tbl, "favorite color", "green");
  response = get_val(tbl, "favorite color");
  REQUIRE(response == "green");
  
  // now overwrite that key/value pair and be sure it sticks.
  set_kvp(tbl, "favorite color", "puce");
  response = get_val(tbl, "favorite color");
  REQUIRE(response == "puce");
}

TEST_CASE("Hash: contains", "[contains]") {
  // requires init_table and set_kvp
  hash_table* tbl = init_table(16);

  REQUIRE_FALSE(contains(tbl, "say what"));
  set_kvp(tbl, "say what", "oh hi there's a value now");
  REQUIRE(contains(tbl, "say what"));
}

TEST_CASE("Hash: remove", "[remove]") {
  // requires init_table and set_kvp
  hash_table* tbl = init_table(16);
  REQUIRE_FALSE(remove(tbl, "quay")); // ensure removing non-existing key yields false
  set_kvp(tbl, "quay", "on down yon in temple bar"); // add kvp
  REQUIRE(tbl->size == 1); // ensure size is now 1
  REQUIRE(tbl->occupied == 1); // occupied also 1
  REQUIRE_FALSE(remove(tbl, "no such key")); // remove a key that's not there
  REQUIRE(tbl->size == 1); // don't touch size if nothing was removed
  REQUIRE(tbl->occupied == 1); // same with occupied
  REQUIRE(remove(tbl, "quay")); // now remove the key that we know is there
  REQUIRE(tbl->size == 0); // be sure to reduce map size when removing a key
  REQUIRE(tbl->occupied == 1); // the removed node is still taking up space  
}

TEST_CASE("Hash: resize", "[resize]") {
  // requires:
  //   - init_table
  //   - set_kvp
  //   - remove
  //   - resize
  hash_table* tbl = init_table(16);
  string keys[10];
  for (unsigned int i=0; i < 10; i++) {
    keys[i] = random_string(8);
    set_kvp(tbl, keys[i], random_string(16));
  }
  // these two just establish a baseline
  REQUIRE(tbl->capacity == 16);
  REQUIRE(tbl->size == 10);

  // Now resize the table by doubling the capacity
  resize(tbl, tbl->capacity * 2);
  REQUIRE(tbl->capacity == 32); // capacity should double
  REQUIRE(tbl->size == 10); // but number of items should stay same

  // Now remove four items, then resize back to 16
  remove(tbl, keys[2]);
  remove(tbl, keys[4]);
  remove(tbl, keys[6]);
  remove(tbl, keys[8]);
  REQUIRE(tbl->capacity == 32);
  REQUIRE(tbl->size == 6);
  resize(tbl, tbl->capacity / 2);
  REQUIRE(tbl->capacity == 16);
  REQUIRE(tbl->size == 6);
}

TEST_CASE("Hash: stress test", "[stress]") {
  // this test case assumes all the non-optional hash.h functions are
  // implemented. it will add and remove items at a furious pace,
  // resizing when needed. it uses a vector to keep track of what
  // should be in the map.
  //
  // you might have to run this explicity by running:
  // ./hash_test "[stress]"
  hash_table* tbl = init_table(8);
  vector<string> keys;
  srand (static_cast <unsigned> (time(0)));
  int expansion = 100;
  int contraction = 800;
  for (int i=0; i < 2000; i++) {
    if (i % 100 == 0) {
      print_short(tbl, "report");      
    }
    if (i < expansion) {
      // add things.
      keys.push_back(random_string(20));
      set_kvp(tbl, keys.back(), random_string(10));
      REQUIRE(contains(tbl, keys.back()));
    } else if (i < contraction) {
      // randomly add or remove things
      float r = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
      if (r > 0.3) {
        // add
        keys.push_back(random_string(20));
        set_kvp(tbl, keys.back(), random_string(10));
        REQUIRE(contains(tbl, keys.back()));
      } else {
        // rm if safe
        if (keys.size() > 0) {
          string doomed = keys.back();
          keys.pop_back();
          REQUIRE(contains(tbl, doomed));
          remove(tbl, doomed);
          REQUIRE_FALSE(contains(tbl, doomed));
        }
      }
    } else {
      // rm if safe
      if (keys.size() > 0) {
        remove(tbl, keys.back());
        REQUIRE_FALSE(contains(tbl, keys.back()));
        keys.pop_back();
      }
    }
    if (load(tbl) > 0.75) {
      print_short(tbl, "grow");
      resize(tbl, tbl->capacity * 2);
    } else if (load(tbl) < 0.1) {
      print_short(tbl, "shrink");
      resize(tbl, tbl->capacity / 2);
    }
  }
}

// 'accident' implies there's nobody to blame

string mkstring(string label, int num) {
  std::ostringstream k;
  k << label << " " << num;
  return k.str();
}

std::string random_string(size_t length) {
  auto randchar = []() -> char {
    const char charset[] =
    "0123456789"
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "abcdefghijklmnopqrstuvwxyz";
    const size_t max_index = (sizeof(charset) - 1);
    return charset[rand() % max_index];
  };
  std::string str(length,0);
  std::generate_n(str.begin(), length, randchar);
  return str;
}

void print_short(hash_table* tbl, string msg) {
  printf("Load: %4.2f, size: %8d, capacity: %8d, occupied: %8d (%s)\n", load(tbl), tbl->size, tbl->capacity, tbl->occupied, msg.c_str());
}
