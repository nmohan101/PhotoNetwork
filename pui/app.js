// --- Imports -----
var express          = require('express');
var bodyParser       = require('body-parser');
const shell          = require('shelljs');
const sqlite3        = require('sqlite3').verbose();

// ----Definitions ----
var urlencodedParser = bodyParser.urlencoded({ extended: false });
var db_path          = "/srv/PhotoNetwork/PhotoNetwork.db";
var capture_path     = "/opt/PhotoNetwork/bin/srv_capture"
var app              = express();
app.set('view engine', 'ejs');

let db = new sqlite3.Database(db_path);
var dbData = [];

db.serialize(() => {
    db.each(`SELECT id, client_ip, total_hb, time FROM Active_Clients`, (err, row) => {
            if (err){
                console.error(err.message);
            }
            console.log(row.id + "\t" + row.client_ip);
            dbData.push({
            "id" : row.id,
            "client_ip" : row.client_ip,
            "hb" : row.total_hb,
            "time" : row.time
            });
        });
});

db.close((err) => {
    if (err){
        console.error(err.message);
    }
    console.log('Close the database conection');
});

// The line below is what gives access to the css file and serves it.
// lots of trouble with it.  Adding '__dirname' fixed it.
app.use(express.static(__dirname + '/assets'));

var testEjs = 'testSTR';

//This returns the html page
app.get('/', (req, res) =>{
    //res.sendFile(__dirname + '/template.html');
    //The command below is using 'ejs' and looks in the views folder for 'index'.
    res.render('index',{dbData});
});

app.post('/assets',urlencodedParser, (req, res) =>{
    
    //Get Capture Amount
    var no_of_captures = parseInt(req.body.capAmount, 10);

    //Proceed only if we recived a valid number of captures
    if (isNaN(no_of_captures) || no_of_captures > 100)
    {
        console.log("Invalid number of captures entered. Minimum is 1 and Maximum is 100");
    }
    else
    {
        var command = `${capture_path} -c ${no_of_captures}`;
        console.log(`Sending Commannd: ${command}`);
        shell.exec(command);
    }
    res.render('index',{dbData});
});


app.listen(3000);
