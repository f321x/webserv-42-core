#!/usr/bin/env python3

import os
import sys

def main():
    # Output the required headers first (Content-Type)
    print("Content-Type: text/html\n")

    # Optionally print out CGI environment variables (helpful for debugging)
    print("<html><body>")
    print("<h1>CGI Script Output</h1>")

    print("<h2>Environment Variables</h2>")
    print("<ul>")
    for key, value in os.environ.items():
        print(f"<li>{key}: {value}</li>")
    print("</ul>")

    # Get CONTENT_LENGTH environment variable to determine how much data to read from stdin
    content_length = os.environ.get('CONTENT_LENGTH')
    
    if content_length:
        try:
            content_length = int(content_length)  # Convert to integer
        except ValueError:
            content_length = 0  # If it's invalid, treat it as 0

    # Read and print stdin content only if content_length is greater than 0
    print("<h2>Standard Input</h2>")
    print("<pre>")
    
    if content_length > 0:
        stdin_content = sys.stdin.read(content_length)
        print(stdin_content)
    else:
        print("(No input data)")
    
    print("</pre>")

    print("<h2>Script Execution Finished</h2>")
    print("</body></html>")

if __name__ == "__main__":
    main()
