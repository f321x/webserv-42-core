#!/usr/bin/env python3
import os
import cgi
import html

print("Content-Type: text/html\n")

upload_dir = "www/upload/"

html_content = """
<!DOCTYPE html>
<html>
<head>
    <title>File Manager</title>
    <style>
        table { border-collapse: collapse; width: 100%; }
        th, td { padding: 8px; text-align: left; border: 1px solid #ddd; }
        th { background-color: #f2f2f2; }
        .delete-btn { color: red; cursor: pointer; }
        .upload-form { margin: 20px 0; padding: 20px; background-color: #f8f8f8; }
    </style>
    <script>
        async function deleteFile(filename) {
            if (confirm('Are you sure you want to delete ' + filename + '?')) {
                try {
                    const response = await fetch('/upload/' + filename, {
                        method: 'DELETE'
                    });
                    if (response.ok) {
                        location.reload();
                    } else {
                        alert('Error deleting file: ' + response.statusText);
                    }
                } catch (error) {
                    alert('Error: ' + error);
                }
            }
        }

        async function uploadFile(event) {
            event.preventDefault();
            const formData = new FormData(event.target);
            
            try {
                const response = await fetch('/file_upload', {
                    method: 'POST',
                    body: formData
                });
                
                if (response.ok) {
                    location.reload();
                } else {
                    alert('Error uploading file: ' + response.statusText);
                }
            } catch (error) {
                alert('Error: ' + error);
            }
        }
    </script>
</head>
<body>
    <h1>File Manager</h1>
    
    <div class="upload-form">
        <h2>Upload File</h2>
        <form onsubmit="uploadFile(event)">
            <input type="file" name="file" required>
            <button type="submit">Upload</button>
        </form>
    </div>

    <h2>Files</h2>
    <table>
        <tr>
            <th>Filename</th>
            <th>Size</th>
            <th>Last Modified</th>
            <th>Action</th>
        </tr>
"""

# List files in the directory
try:
    # create upload dir if it doesn't exist
    if not os.path.exists(upload_dir):
        os.makedirs(upload_dir)

    files = os.listdir(upload_dir)
    for filename in files:
        if filename.startswith('.'):
            continue
            
        filepath = os.path.join(upload_dir, filename)
        stats = os.stat(filepath)
        
        # Format file size
        size = stats.st_size
        if size < 1024:
            size_str = f"{size} B"
        elif size < 1024 * 1024:
            size_str = f"{size/1024:.1f} KB"
        else:
            size_str = f"{size/(1024*1024):.1f} MB"
        
        # Format last modified time
        modified = os.path.getmtime(filepath)
        modified_str = os.path.getctime(filepath)
        
        html_content += f"""
        <tr>
            <td>
                <a href="/upload/{html.escape(filename)}" class="filename-link" target="_blank">
                    {html.escape(filename)}
                </a>
            </td>
            <td>{size_str}</td>
            <td>{modified_str}</td>
            <td><span class="delete-btn" onclick="deleteFile('{html.escape(filename)}')">Delete</span></td>
        </tr>
        """
except Exception as e:
    html_content += f"<tr><td colspan='4'>Error listing files: {html.escape(str(e))}</td></tr>"

# Close HTML
html_content += """
    </table>
</body>
</html>
"""

print(html_content)