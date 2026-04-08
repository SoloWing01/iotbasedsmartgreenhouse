from flask import Flask, request, jsonify
from pymongo import MongoClient
import urllib.parse
from datetime import datetime

app = Flask(__name__)
import os
from dotenv import load_dotenv

# load_dotenv()
# username = os.getenv('username')
# password = os.getenv('password')
username = os.environ.get("username")
password = os.environ.get("password")
# Escape the characters
escaped_user = urllib.parse.quote_plus(username)
escaped_pass = urllib.parse.quote_plus(password)

# -------- MongoDB Connection --------
client = MongoClient(f"mongodb+srv://{escaped_user}:{escaped_pass}@cluster0.s2d5y6z.mongodb.net/?appName=Cluster0")
db = client["greenhouse"]
collection = db["sensor_data"]

# -------- API Route --------
@app.route('/data', methods=['POST'])
def receive_data():
    data = request.json

    # Add timestamp
    data["timestamp"] = datetime.now()

    # Insert into MongoDB
    collection.insert_one(data)

    print("Saved:", data)

    return jsonify({"status": "stored"}), 200

## Run Server (Locally)

# app.run(host='0.0.0.0', port=5000)


if __name__ == "__main__":
    port = int(os.environ.get("PORT", 8080))
    print("Running on port:", port)
    app.run(host="0.0.0.0", port=port)