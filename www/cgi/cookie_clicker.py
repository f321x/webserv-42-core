#!/usr/bin/env python3
import os
import http.cookies
import cgi

# Get existing cookies
cookie = http.cookies.SimpleCookie(os.environ.get("HTTP_COOKIE"))
counter = int(cookie["counter"].value) if "counter" in cookie else 0

# Increment counter if form was submitted
form = cgi.FieldStorage()
if "increment" in form:
    counter += 1

# Set the updated counter in the cookie
print("Content-Type: text/html")
print(f"Set-Cookie: counter={counter}; Path=/")
print()

# Generate HTML with the current counter value and button to increment
print(f"""
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <title>Counter Increment</title>
</head>
<body>
    <h1>Current Counter Value: {counter}</h1>
    <form method="post">
        <button type="submit" name="increment" value="true">Increment Counter</button>
    </form>
</body>
</html>
""")