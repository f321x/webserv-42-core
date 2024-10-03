#!/usr/bin/env python3

import os
import sys

print("IN THE CGI SCRIPT")

# Set the output file
output_file = "tmp/cgi_output.txt"

# Open the file in write mode
with open(output_file, "w") as f:
    # Write environment variables
    f.write("Environment Variables:\n")
    for key, value in os.environ.items():
        f.write(f"{key}={value}\n")
    
    f.write("\nStandard Input:\n")
    
    # Read and write the content from standard input
    stdin_content = sys.stdin.read()
    f.write(stdin_content)

# Output CGI headers
print("Content-Type: text/plain\n")
print("Environment Variables and Standard Input logged to file.")
