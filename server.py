
from flask import Flask
from flask import request
import time

app = Flask(__name__)

prev_messages = []
@app.route("/")
def hello():
    var = request.args.get("var")
    current_time = time.strftime("%Y-%m-%d %H:%M:%S")
    message = ""

    if var == "1":
        message = "Motion detected"
    elif var == "2":
        message = "Bell's ringing"

    if message:
        prev_messages.append((message, current_time))

    response_str = ""
    for message, timestamp in prev_messages:
        response_str += message + " at " + timestamp + "<br>"
    if message:
        response_str += message + " at " + current_time

    return response_str