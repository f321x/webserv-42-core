#!/usr/bin/env python3

import os
import cgi
import cgitb

cgitb.enable()  # Enable debugging

# Define the path to the count file
count_file = "/tmp/count.txt"

def get_count():
    """Read the current count from the file."""
    if os.path.exists(count_file):
        with open(count_file, "r") as f:
            try:
                return int(f.read())
            except ValueError:
                return 0
    else:
        return 0

def increment_count():
    """Increment the count and save it back to the file."""
    count = get_count() + 1
    with open(count_file, "w") as f:
        f.write(str(count))
    return count

# Since we're only handling increment, no need to parse form data for routes

# Increment the count
count = increment_count()

# Output HTTP headers
print("Content-Type: text/html")
print()

# Start of HTML output
print("""
<!DOCTYPE html>
<html>
<head>
    <title>Increment Counter</title>
    <style>
        body { font-family: Arial, sans-serif; text-align: center; margin-top: 50px; }
        button { padding: 10px 20px; font-size: 16px; }
        h1 { color: #333; }
    </style>
</head>
<body>
    <h1>Button clicked {} times.</h1>
    <form action="script.py" method="post">
        <button type="submit">Click Me</button>
    </form>
</body>
</html>
""".format(count))
