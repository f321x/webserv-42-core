#!/usr/bin/env python3

import os
import urllib.parse

def parse_query_string(query_string):
    parsed_query = urllib.parse.parse_qs(query_string)
    return parsed_query

def main():
    # Get the QUERY_STRING from environment variables
    query_string = os.environ.get("QUERY_STRING", "")

    # Parse the QUERY_STRING
    parsed_query = parse_query_string(query_string)

    # Extract user_name and password parameters
    user_name = parsed_query.get("user_name", [""])[0]
    password = parsed_query.get("password", [""])[0]

    #create content
    str = '<html>\n<head><title>User Authentication</title></head>\n<body>\n<h1>User Authentication</h1>\n'
    if user_name and password:
      str += "<p>Welcome, {}! Your password is: {}!</p>".format(user_name, password)
    else:
      str += "<p>No user name or password provided.</p>"
    str += "</body>\n</html>\n"
    # Print HTML content
    print("Content-type: text/html")
    if user_name == "admin" and password == "42":
      print("Set-Cookie: user=admin; domain=127.0.0.1; path=/; max-age=30")
    print("Content-length: {}\r\n\r\n".format(len(str)))
    print(str)

if __name__ == "__main__":
  main()