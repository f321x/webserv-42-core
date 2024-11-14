#!/usr/bin/env python3

import os
import fcntl
import cgitb

cgitb.enable()  # Enable debugging

# Define the path to the count file
count_file = "./tmp/count.txt"

def increment_count():
    """Increment the count with an exclusive lock and save it back to the file."""
    # check if the file exists
    os.makedirs(os.path.dirname(count_file), exist_ok=True)

    # Open the file in read-write mode
    with open(count_file, "a+") as f:
        try:
            # Try to acquire an exclusive lock without blocking
            fcntl.flock(f, fcntl.LOCK_EX | fcntl.LOCK_NB)
        except BlockingIOError:
            # Handle the case where the lock is not available
            raise Exception("Could not acquire file lock")
        try:
            f.seek(0)
            data = f.read()
            try:
                count = int(data) if data else 0
            except ValueError:
                count = 0
            count += 1
            f.seek(0)
            f.truncate()
            f.write(str(count))
            f.flush()
        finally:
            # Always release the lock
            fcntl.flock(f, fcntl.LOCK_UN)
    return count

try:
    # Increment the count
    count = increment_count()
    
    # Output HTTP headers
    print("Content-Type: text/html")
    print()
    
    # Start of HTML output
    print(f"""
    <!DOCTYPE html>
    <html>
    <head>
        <title>Increment Counter</title>
        <style>
            body {{ font-family: Arial, sans-serif; text-align: center; margin-top: 50px; }}
            button {{ padding: 10px 20px; font-size: 16px; }}
            h1 {{ color: #333; }}
        </style>
    </head>
    <body>
        <h1>Button clicked {count} times.</h1>
        <form action="file_clicker.py" method="post">
            <button type="submit">Click Me</button>
        </form>
    </body>
    </html>
    """)
except Exception as e:
    # Output HTTP headers
    print("Content-Type: text/html")
    print()
    print("<h1>An error occurred</h1>")
    print(f"<p>{e}</p>")
