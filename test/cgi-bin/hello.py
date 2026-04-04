#!/usr/bin/env python3
import os
import sys

print("Content-Type: text/html\r")
print("\r")
print("<!DOCTYPE html>")
print("<html>")
print("<head><title>CGI Test</title></head>")
print("<body>")
print("<h1>CGI Script Working!</h1>")
print("<h2>Environment Variables:</h2>")
print("<ul>")
print(f"<li>REQUEST_METHOD: {os.environ.get('REQUEST_METHOD', 'Not set')}</li>")
print(f"<li>CONTENT_TYPE: {os.environ.get('CONTENT_TYPE', 'Not set')}</li>")
print(f"<li>CONTENT_LENGTH: {os.environ.get('CONTENT_LENGTH', 'Not set')}</li>")
print(f"<li>SCRIPT_FILENAME: {os.environ.get('SCRIPT_FILENAME', 'Not set')}</li>")
print("</ul>")
print("</body>")
print("</html>")
