create database project default character set utf8;

use project;

create table project_data(
	`idx` int primary key auto_increment not null,
    `eng` varchar(500),
    `kor` varchar(500),
    `press` int default '0'
);

create table dictionary(
	`idx` int primary key auto_increment not null,
    `eng` varchar(11000),
    `kor` varchar(11000),
    `star` int default 0 not null
);
