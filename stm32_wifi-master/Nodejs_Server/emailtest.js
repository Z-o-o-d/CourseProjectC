var email = require("emailjs");
var mysql = require('mysql');
var schedule = require('node-schedule');
//���ݿ�����
var conn = mysql.createConnection({
	host:'localhost',
	user:'root',
	password:'root',
	database:'nodemysql',
	port:3306
})
//�������ݿ�
conn.connect();

//��������
var server = email.server.connect({
	user: "xxx@xxx.com",//�û���
	password:"xxx",//����
	host:"smtp.xxx.com",//smtp���������
	ssl:true
});
//ÿ���ӵ�ʮ���ʱ����
var rule = new schedule.RecurrenceRule();
rule.second = 10;
var j = schedule.scheduleJob(rule,function(){
    conn.query('SELECT * FROM pet',function(err,rows,fields){
	 	var tem = rows[rows.length-1].tem;
	 	var temmsg = {
	 		text:"tempreture is " + tem + ",please  be careful",
	 		from: "xxxxxxx@xxxx.com",//���ͷ�
	 		to:"xxx@xxx.com",//���շ�
	 		subject:"PetHose tem"
	 	};	 		
	 	if(tem>=30){
	 		server.send(temmsg,function(err,message){
	 			console.log(err || "ok");
	 		});
	 	}
	});
});