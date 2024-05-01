import cgi
import io
import sys
import os

def parse_multipart_form_data(request_string):
    # Split the request string into headers and body
    try:

        byte_stream = io.BytesIO(request_string.encode())
        request_string = request_string.split("\n")
        request_string = request_string[1]
        request_string = request_string.removeprefix("--")
        byte_outerboundary = request_string
        print(byte_outerboundary)

        # print(request_string.earse(0, 2))
        print(type(byte_stream))
        form = cgi.FieldStorage(
            fp=byte_stream, 
            environ={'REQUEST_METHOD': 'POST',
                     'CONTENT_TYPE': 'multipart/form-data; boundary=' + byte_outerboundary,}
        )
        print(form)
    except Exception as e:
        print(f"Error: {e}")
        sys.exit(1)
def receive_data():
    input_data = sys.stdin.read()
    parse_multipart_form_data(input_data)

receive_data()

