#!/usr/bin/env python3

# Infinite loop test - should trigger timeout
import time

print("Content-Type: text/html\r")
print("\r")

while True:
    time.sleep(1)
