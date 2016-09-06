CREATE TABLE `monster_base` (
	`Key` bigint(20) NOT NULL AUTO_INCREMENT,
	PRIMARY KEY (`Key`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

alter table monster_base add Level INT(11) UNSIGNED NOT NULL DEFAULT '0'/* COMMENT '怪物等级'*/;


CREATE TABLE `player_base` (
	`Key` bigint(20) NOT NULL AUTO_INCREMENT,
	PRIMARY KEY (`Key`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

alter table player_base add Level INT(11) UNSIGNED NOT NULL DEFAULT '0'/* COMMENT '玩家等级'*/;
alter table player_base add Nation INT(11) UNSIGNED NOT NULL DEFAULT '0'/* COMMENT '玩家国籍'*/;
alter table player_base add Offical INT(11) UNSIGNED NOT NULL DEFAULT '0'/* COMMENT '玩家官职'*/;


CREATE TABLE `player_bagequip` (
	`Key` bigint(20) NOT NULL,
	`Index` bigint(20) NOT NULL,
	PRIMARY KEY (`Key`,`Index`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

alter table player_bagequip add ConfigId INT(11) UNSIGNED NOT NULL DEFAULT '0' /*COMMENT '配置表ID'*/;
alter table player_bagequip add Level INT(11) UNSIGNED NOT NULL DEFAULT '0' /*COMMENT '强化等级'*/;


