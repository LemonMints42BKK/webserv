import os

def generate_html():
    html = """
    <!DOCTYPE html>
    <html lang="en">
    <head>
        <meta charset="UTF-8">
        <meta name="viewport" content="width=device-width, initial-scale=1.0">
        <title>File Upload</title>
        <script>
            async function deleteFile(file) {
                const formData = new FormData();
                formData.append('file', file);
                const response = await fetch('/delete', {
                    method: 'DELETE',
                    body: formData
                }).then(response => response.status == 200 ? location.reload() : alert('Error deleting file'));
            }
        </script>
    </head>
    <body>
        <h1>File Upload</h1>
        <form action="/upload" method="post" enctype="multipart/form-data">
            <input type="file" name="file">
           <!-- <input type="submit" value="Upload"> -->
            <input type="button" value="Upload" onclick="uploadFile()">
        </form>
        
        <h2>Uploaded Files:</h2>
        <ul>
    """

    for root, dirs, files in os.walk(os.path.abspath("www/upload")):
        if files.__len__() > 0:
            for file in files:
                html += f"<li><input type=\"button\" value=\"delete\" onclick=\"deleteFile('{file}')\" > <a href='./upload/{file}'> {file} </a></li>"
    # for file in file_list:
    #     html += f"<li>{file}</li>"
    
    html += """
        </ul>
    <script>
        async function uploadFile() {
            const formData = new FormData();
            const input = document.querySelector('input[type="file"]');
            formData.append('file', input.files[0]);
            const response = await fetch('/upload', {
                method: 'POST',
                body: formData
            }).then(response => response.status == 201 ? location.reload() : alert('Error uploading file'));
        }
    </script>
    </body>
    </html>
    """
    
    return html

def list_files(directory):
    files = os.listdir(directory)
    return files

def main():
    # upload_folder = 'uploads'
    # if not os.path.exists(upload_folder):
    #     os.makedirs(upload_folder)
    # upload_folder = '../../www/upload'
    # files = list_files(upload_folder)
    # html_content = generate_html(files)
    html_content = generate_html()
    print(html_content)
    # dir = os.path.abspath("www/cgi.html")
    # print(dir)
    # with open(dir, 'w') as f:
    #     f.write(html_content)

if __name__ == '__main__':
    main()
