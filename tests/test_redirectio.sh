#!/bin/bash
echo "Testing I/O redirection..."
./myshell <<< "echo Hello > test.txt"
cat test.txt
rm test.txt
