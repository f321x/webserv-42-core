#!/usr/bin/env python3

import os

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

    print("<h2>Script Execution Finished</h2>")
    print("</body></html>")

if __name__ == "__main__":
    main()
