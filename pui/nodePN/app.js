var express = require('express');

var app = express();

app.set('view engine', 'ejs');

const sqlite3 = require('sqlite3').verbose();

let db = new sqlite3.Database('./assets/PhotoNetwork.db', sqlite3.OPEN_READWRITE, (err) => {
	if (err){
		console.log(err.message);
	}
	console.log('Connected to the Photo Network database');

});
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
})

app.listen(3000);
