#!/usr/bin/env python3
import os

# Test that CGI runs in correct directory for relative paths
print("Content-Type: text/html\r")
print("\r")
print("<!DOCTYPE html>")
print("<html>")
print("<head><title>Directory Test</title></head>")
print("<body>")
print("<h1>Working Directory Test</h1>")
print(f"<p>Current working directory: {os.getcwd()}</p>")
print(f"<p>Script location: {os.environ.get('SCRIPT_FILENAME', 'Not set')}</p>")

# Try to read a file in same directory
try:
    with open("hello.py", "r") as f:
        print("<p>✓ Can read hello.py (relative path works)</p>")
except Exception as e:
    print(f"<p>✗ Cannot read hello.py: {e}</p>")

print("</body>")
print("</html>")
