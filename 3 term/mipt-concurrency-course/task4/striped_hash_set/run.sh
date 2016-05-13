
#!/bin/bash

g++ -pthread -fsanitize=thread -std=gnu++14 striped_hash_set.cpp -o ht ; ./ht

