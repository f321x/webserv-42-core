#!/usr/bin/env python3
import cgi
import os
import csv
import uuid
from http.cookies import SimpleCookie

# CSV file to store session data
SESSION_FILE = "./tmp/sessions.csv"

# Function to load session data from CSV
def load_sessions():
    sessions = {}
    if os.path.exists(SESSION_FILE):
        with open(SESSION_FILE, "r") as file:
            reader = csv.reader(file)
            for row in reader:
                if len(row) == 2:
                    sessions[row[0]] = row[1]  # session_token: text
    return sessions

# Function to save session data to CSV
def save_sessions(sessions):
    with open(SESSION_FILE, "w") as file:
        writer = csv.writer(file)
        for session_token, text in sessions.items():
            writer.writerow([session_token, text])

# Main CGI processing
print("Content-type: text/html")

# Check for an existing session cookie
cookie = SimpleCookie(os.environ.get("HTTP_COOKIE"))
session_token = cookie.get("session_token")

sessions = load_sessions()

# If the session token doesn't exist or is invalid, create a new session
if not session_token or session_token.value not in sessions:
    session_token_value = str(uuid.uuid4())  # Generate a new UUID for session
    sessions[session_token_value] = ""  # Initialize with empty text
    save_sessions(sessions)
    # Set the cookie with the new session token
    print(f"Set-Cookie: session_token={session_token_value}")

else:
    # Use the existing session token
    session_token_value = session_token.value

# Load or update text based on form submission
form = cgi.FieldStorage()
text = form.getvalue("text")

if os.environ.get("REQUEST_METHOD") == "POST":
    # Update session text and save it
    sessions[session_token_value] = text
    save_sessions(sessions)

    print("Status: 303 See Other")
    print("Location: text.py")
    print()  # End headers here
    exit()  # Stop further processing to prevent double output

# Get the text associated with this session
stored_text = sessions.get(session_token_value, "")

print()

# Output HTML form with added styling
print(f"""
<html>
<head>
    <title>Session-based Text Storage</title>
    <style>
        body {{
            font-family: Arial, sans-serif;
            background-color: #f4f4f9;
            margin: 0;
            padding: 0;
            display: flex;
            justify-content: center;
            align-items: center;
            height: 100vh;
            color: #333;
        }}
        .container {{
            background-color: white;
            padding: 20px;
            border-radius: 8px;
            box-shadow: 0 4px 8px rgba(0, 0, 0, 0.1);
            width: 400px;
            max-width: 90%;
            text-align: center;
        }}
        h2 {{
            color: #5c6bc0;
        }}
        textarea {{
            width: 100%;
            padding: 10px;
            font-size: 16px;
            border: 1px solid #ddd;
            border-radius: 4px;
            margin: 10px 0;
            resize: vertical;
        }}
        input[type="submit"] {{
            background-color: #5c6bc0;
            color: white;
            border: none;
            padding: 10px 20px;
            font-size: 16px;
            border-radius: 4px;
            cursor: pointer;
            transition: background-color 0.3s;
        }}
        input[type="submit"]:hover {{
            background-color: #3f51b5;
        }}
        label {{
            font-size: 18px;
            margin-bottom: 10px;
            display: block;
        }}
    </style>
</head>
<body>
    <div class="container">
        <h2>Session-based Text Storage</h2>
        <p>Your session token: {session_token_value}</p>
        <form method="Post" action="text.py">
            <label for="text">Enter text:</label><br>
            <textarea name="text" rows="4" cols="50">{stored_text}</textarea><br><br>
            <input type="submit" value="Save Text">
        </form>
    </div>
</body>
</html>
""")
