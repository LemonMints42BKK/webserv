import cgi
import io
import sys
import os

def parse_multipart_form_data(request_string):
    # Split the request string into headers and body
    try:
        # print(len(request_string))
        byte_stream = io.BytesIO(request_string.encode('utf-8', 'surrogateescape'))
        print(len(byte_stream.getvalue()))
        # print(request_string.earse(0, 2))
        # print(type(byte_stream))
        form = cgi.FieldStorage(
            fp=byte_stream,
            environ={
                'REQUEST_METHOD': sys.argv[1],
                'CONTENT_TYPE': sys.argv[2],
            },
            keep_blank_values=True
        )
        filename = form['file'].filename
        full_path = os.path.join(sys.argv[3], filename)
        print(full_path)
        file = form['file'].value
        with open(full_path, 'wb') as f:
            f.write(file)
        # print(form['file'].filename)
        # print("-------------------")
        # print(form['file'].value)
        
    except Exception as e:
        print(f"Error: {e}")
        sys.exit(1)
def receive_data():
    input_data = sys.stdin.read()
    # print(sys.argv)
    # print(input_data)
    # print(sys.getsizeof(input_data.encode('utf-8', 'surrogateescape')))
    # print(len(input_data))
    parse_multipart_form_data(input_data)

receive_data()

