#!/bin/bash

# Print HTTP headers
echo "Content-Type: text/html"
echo ""

# Get webserv process info
WEBSERV_PID=$(pgrep webserv)
CPU_USAGE=$(ps -p $WEBSERV_PID -o %cpu | tail -n 1)
MEM_USAGE=$(ps -p $WEBSERV_PID -o %mem | tail -n 1)
UPTIME=$(ps -p $WEBSERV_PID -o etime | tail -n 1)
THREADS=$(ps -p $WEBSERV_PID -o nlwp | tail -n 1)
RSS=$(ps -p $WEBSERV_PID -o rss | tail -n 1)
VSZ=$(ps -p $WEBSERV_PID -o vsz | tail -n 1)

# Get system info
TOTAL_MEM=$(free -m | awk '/Mem:/ {print $2}')
FREE_MEM=$(free -m | awk '/Mem:/ {print $4}')
LOAD_AVG=$(uptime | awk -F'load average:' '{print $2}')

# Create HTML page with modern styling
cat << HTML
<!DOCTYPE html>
<html>
<head>
    <title>Webserv System Status</title>
    <style>
        body {
            font-family: Arial, sans-serif;
            max-width: 800px;
            margin: 20px auto;
            padding: 20px;
            background-color: #f5f5f5;
        }
        .dashboard {
            display: grid;
            grid-template-columns: repeat(auto-fit, minmax(300px, 1fr));
            gap: 20px;
            margin-top: 20px;
        }
        .card {
            background: white;
            padding: 20px;
            border-radius: 10px;
            box-shadow: 0 2px 5px rgba(0,0,0,0.1);
        }
        .metric {
            margin: 10px 0;
        }
        .metric-value {
            font-size: 24px;
            font-weight: bold;
            color: #2196F3;
        }
        .metric-label {
            color: #666;
            font-size: 14px;
        }
        h1 {
            color: #333;
            text-align: center;
        }
        .refresh-button {
            display: block;
            margin: 20px auto;
            padding: 10px 20px;
            background-color: #2196F3;
            color: white;
            border: none;
            border-radius: 5px;
            cursor: pointer;
            text-decoration: none;
            text-align: center;
            width: fit-content;
        }
        .refresh-button:hover {
            background-color: #1976D2;
        }
        .status-good {
            color: #4CAF50;
        }
        .status-warning {
            color: #FFC107;
        }
        .status-critical {
            color: #F44336;
        }
    </style>
</head>
<body>
    <h1>Webserv System Status</h1>
    
    <div class="dashboard">
        <div class="card">
            <h2>Process Information</h2>
            <div class="metric">
                <div class="metric-label">Process ID</div>
                <div class="metric-value">$WEBSERV_PID</div>
            </div>
            <div class="metric">
                <div class="metric-label">CPU Usage</div>
                <div class="metric-value ${CPU_USAGE%.*} > 80 ? 'status-critical' : ${CPU_USAGE%.*} > 50 ? 'status-warning' : 'status-good'">
                    $CPU_USAGE%
                </div>
            </div>
            <div class="metric">
                <div class="metric-label">Memory Usage</div>
                <div class="metric-value ${MEM_USAGE%.*} > 80 ? 'status-critical' : ${MEM_USAGE%.*} > 50 ? 'status-warning' : 'status-good'">
                    $MEM_USAGE%
                </div>
            </div>
            <div class="metric">
                <div class="metric-label">Uptime</div>
                <div class="metric-value">$UPTIME</div>
            </div>
        </div>

        <div class="card">
            <h2>Memory Details</h2>
            <div class="metric">
                <div class="metric-label">RSS (Resident Set Size)</div>
                <div class="metric-value">$(($RSS / 1024)) MB</div>
            </div>
            <div class="metric">
                <div class="metric-label">VSZ (Virtual Memory Size)</div>
                <div class="metric-value">$(($VSZ / 1024)) MB</div>
            </div>
            <div class="metric">
                <div class="metric-label">System Free Memory</div>
                <div class="metric-value">$FREE_MEM MB</div>
            </div>
            <div class="metric">
                <div class="metric-label">System Total Memory</div>
                <div class="metric-value">$TOTAL_MEM MB</div>
            </div>
        </div>

        <div class="card">
            <h2>System Load</h2>
            <div class="metric">
                <div class="metric-label">Load Average</div>
                <div class="metric-value">$LOAD_AVG</div>
            </div>
            <div class="metric">
                <div class="metric-label">Thread Count</div>
                <div class="metric-value">$THREADS</div>
            </div>
        </div>
    </div>

    <a href="system_status.sh" class="refresh-button">Refresh</a>

    <script>
        // Add color classes based on values
        document.querySelectorAll('.metric-value').forEach(el => {
            const value = parseFloat(el.textContent);
            if (value > 80) el.classList.add('status-critical');
            else if (value > 50) el.classList.add('status-warning');
            else el.classList.add('status-good');
        });
    </script>
</body>
</html>
HTML