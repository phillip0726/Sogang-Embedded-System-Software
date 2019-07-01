const express = require('express');
const mysql = require('mysql');

const app = express();


app.get('/send',function(req, res){
    var connection = mysql.createConnection({    
		host : 'IP',
		user : 'root',
		password: 'password',
		port : 3306,
		database : 'project'
	});
	connection.connect();
    var eng = req.query.eng;
    var sql = "select * from dictionary where eng>='" + eng + "'";

    var aaa = connection.query(sql, function(err, rows, fields, callback){
        if(err)
            console.log('Error while performing Query', err);
        
        var result = "";
        var count = 0;
        if(rows.length<4){
            var read_last_col_sql = "select * from dictionary where idx >= 51867";
            connection.query(read_last_col_sql, function(new_err, new_rows, new_fields){
                if(err)
                    console.log("error", new_err);
                var result = "" + new_rows[0].eng + "///" + new_rows[0].kor + new_rows[0].star + "///" + new_rows[0].idx + "!!!"
                    + new_rows[1].eng + "///" + new_rows[1].kor + "///" + new_rows[1].star + "///" + new_rows[1].idx + "!!!"
                    + new_rows[2].eng + "///" + new_rows[2].kor + "///" + new_rows[2].star + "///" + new_rows[2].idx + "!!!"
                    + new_rows[3].eng + "///" + new_rows[3].kor + "///" + new_rows[3].star + "///" + new_rows[3].idx;
                console.log(result);
                res.send(result);

            });
            connection.end();
        }

        else{
            var result = "" + rows[0].eng + "///" + rows[0].kor + "///" + rows[0].star + "///" + rows[0].idx + "!!!"
                + rows[1].eng + "///" + rows[1].kor + "///" + rows[1].star + "///" + rows[1].idx + "!!!"
                + rows[2].eng + "///" + rows[2].kor + "///" + rows[2].star + "///" + rows[2].idx + "!!!"
                + rows[3].eng + "///" + rows[3].kor + "///" + rows[3].star + "///" + rows[3].idx; 
            console.log(result);
            res.send(result);
            connection.end();
        }
    });
});

app.get('/star',function(req, res){
	var connection = mysql.createConnection({
		host : 'IP',
		user : 'root',
		password: 'password',
		port : 3306,
		database : 'project'
	});
    connection.connect();
    var idx = req.query.idx;
    var star = req.query.star;
    var sql = "update dictionary set star=" + star + " where idx=" + idx;
    connection.query(sql, function(err, rows, fields){
        if(err)
            console.log("The solution", err);
        console.log("" + idx + "  " + star);
        res.send("1");
    });

    connection.end();

});

app.get('/bookmark', function(req, res){
	var connection = mysql.createConnection({
		host : 'IP',
		user : 'root',
		password: 'password',
		port : 3306,
		database : 'project'
	});
    connection.connect();

    var sql = "select * from dictionary where star=1";
    connection.query(sql, function(err, rows, fields){
        var result = "";
        if(err)
            console.log("The solution");

        console.log("Bookmark " + rows.length);
        
        for(var i =0;i<rows.length;i++){
            result += rows[i].eng + "///" + rows[i].kor + "!!!";
        }

        res.send(result);
    });
});
app.get('/press',function(req, res){
	var connection = mysql.createConnection({
		host : 'IP',
		user : 'root',
		password: 'password',
		port : 3306,
		database : 'project'
	});
    connection.connect();
    connection.query("insert into project_data (press) values (1)", function(err, rows, fields){
        if(!err)
            console.log("The solution");
        else
            console.log("Error", err);
        res.send('1///null///null');
    });
    connection.end();
});

app.get('/',function(req, res){
	var connection = mysql.createConnection({
		host : 'IP',
		user : 'root',
		password: 'password',
		port : 3306,
		database : 'project'
	});
	connection.connect();

	connection.query('select * from project_data order by idx desc limit 1', function(err, rows, fields){
		if(!err)
			console.log('The solution is : ',rows);
		else
			console.log('Error while performing Query.', err);
		var result = rows[0].press + "///" + rows[0].eng + "///" + rows[0].kor;
		res.send(result);
		
	});
	connection.end();
});

app.listen(3000, function(){
	console.log("Start");
});
