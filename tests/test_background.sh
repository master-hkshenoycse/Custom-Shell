#!/bin/bash
echo "Running background process test..."
./myshell <<< "sleep 5 &"
ps aux | grep sleep
