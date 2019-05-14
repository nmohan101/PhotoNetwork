#!/usr/bin/env python3
import sqlite3
from flask import Flask, render_template
app = Flask(__name__)


@app.route("/")
def hello():
    db = sqlite3.connect("PhotoNetwork.db").cursor()
    rows = db.execute("SELECT id, client_ip, total_hb, time FROM Active_Clients;")

    users=[]
    for row in rows:
        user_id, client_ip, total_hb, time = row
        users.append({
            "user_id" : user_id,
            "client_ip" : client_ip,
            "total_hb" : total_hb,
            "time" : time,
            })
   
    return render_template("template.html", users=users)

app.jinja_env.auto_reload = True
app.config['TEMPLATES_AUTO_RELOAD'] = True
if __name__ == "__main__":
    app.run(debug=True, host='127.0.0.1')
