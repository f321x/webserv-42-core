#!/usr/bin/env python3

import os
import cgi
# import cgitb
from urllib.parse import parse_qs
import html  # Import the html module for html.escape()

# Enable debugging (comment out in production)
# cgitb.enable()

# Retrieve the query string from the environment variable
query_string = os.environ.get('QUERY_STRING', '')

# Parse the query string into a dictionary
parsed_qs = parse_qs(query_string)

# Output the HTTP headers
print("Content-Type: text/html")
print()

# Start of HTML output
print(f"""<!DOCTYPE html>
<html>
<head>
    <title>CGI Query String Test</title>
    <style>
        body {{ font-family: Arial, sans-serif; margin: 20px; }}
        h1 {{ color: #333; }}
        table {{ border-collapse: collapse; width: 50%; }}
        th, td {{ border: 1px solid #ddd; padding: 8px; }}
        th {{ background-color: #f2f2f2; }}
    </style>
</head>
<body>
    <h1>CGI Query String Test</h1>
    <p><strong>Raw Query String:</strong> {html.escape(query_string)}</p>
""")

# Check if there are any query parameters
if parsed_qs:
    print("<h2>Parsed Query Parameters:</h2>")
    print("<table>")
    print("<tr><th>Parameter</th><th>Value(s)</th></tr>")
    for key, values in parsed_qs.items():
        # Use html.escape() to sanitize output
        safe_key = html.escape(key)
        safe_values = ', '.join(html.escape(value) for value in values)
        print(f"<tr><td>{safe_key}</td><td>{safe_values}</td></tr>")
    print("</table>")
else:
    print("<p>No query parameters were found.</p>")

# End of HTML output
print("""
</body>
</html>
""")
