#!/bin/bash
echo "Testing pipes..."
./myshell <<< "ls | grep .c"
