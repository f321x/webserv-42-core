#!/bin/bash

# Print HTTP headers
echo "Content-Type: text/html"
echo ""

# Array of fortune cookie messages
FORTUNES=(
    "A beautiful, smart, and loving person will be coming into your life."
    "A dubious friend may be an enemy in camouflage."
    "A faithful friend is a strong defense."
    "A fresh start will put you on your way."
    "A golden egg of opportunity falls into your lap this month."
    "A good time to finish up old tasks."
    "A hunch is creativity trying to tell you something."
    "A light heart carries you through all the hard times."
    "A new perspective will come with the new year."
    "A smile is your personal welcome mat."
)

# Get a random fortune
FORTUNE=${FORTUNES[$RANDOM % ${#FORTUNES[@]}]}

# Create simple HTML page
cat << HTML
<!DOCTYPE html>
<html>
<head>
    <title>Fortune Cookie</title>
    <style>
        body {
            font-family: Arial, sans-serif;
            display: flex;
            justify-content: center;
            align-items: center;
            height: 100vh;
            margin: 0;
            background-color: #f0f0f0;
        }
        .cookie {
            background-color: white;
            padding: 40px;
            border-radius: 10px;
            box-shadow: 0 0 10px rgba(0,0,0,0.1);
            text-align: center;
            max-width: 400px;
        }
        .message {
            font-size: 24px;
            color: #333;
            margin: 20px 0;
        }
        .refresh {
            display: inline-block;
            padding: 10px 20px;
            background-color: #4CAF50;
            color: white;
            text-decoration: none;
            border-radius: 5px;
            margin-top: 20px;
        }
        .time {
            font-size: 14px;
            color: #666;
            margin-top: 20px;
        }
    </style>
</head>
<body>
    <div class="cookie">
        <h1>🥠 Fortune Cookie</h1>
        <div class="message">
            "$FORTUNE"
        </div>
        <a href="fortune.sh" class="refresh">Get Another Fortune</a>
        <div class="time">
            Generated at: $(date)
        </div>
    </div>
</body>
</html>
HTML