#!/usr/bin/env python3
import os
import sys

# Test POST with body
print("Content-Type: text/html\r")
print("\r")
print("<!DOCTYPE html>")
print("<html>")
print("<head><title>POST Test</title></head>")
print("<body>")
print("<h1>POST Request Test</h1>")
print(f"<p>Method: {os.environ.get('REQUEST_METHOD', 'Not set')}</p>")
print(f"<p>Content-Type: {os.environ.get('CONTENT_TYPE', 'Not set')}</p>")
print(f"<p>Content-Length: {os.environ.get('CONTENT_LENGTH', 'Not set')}</p>")

# Read POST body from stdin
content_length = int(os.environ.get('CONTENT_LENGTH', 0))
if content_length > 0:
    body = sys.stdin.read(content_length)
    print(f"<p>Body received: <pre>{body}</pre></p>")
else:
    print("<p>No body data</p>")

print("</body>")
print("</html>")
