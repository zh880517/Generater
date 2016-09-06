CREATE TABLE `monster_base` (
	`Key` bigint(20) NOT NULL AUTO_INCREMENT,
	PRIMARY KEY (`Key`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

alter table monster_base add Level INT(11) UNSIGNED NOT NULL DEFAULT '0'/* COMMENT '����ȼ�'*/;


CREATE TABLE `player_base` (
	`Key` bigint(20) NOT NULL AUTO_INCREMENT,
	PRIMARY KEY (`Key`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

alter table player_base add Level INT(11) UNSIGNED NOT NULL DEFAULT '0'/* COMMENT '��ҵȼ�'*/;
alter table player_base add Nation INT(11) UNSIGNED NOT NULL DEFAULT '0'/* COMMENT '��ҹ���'*/;
alter table player_base add Offical INT(11) UNSIGNED NOT NULL DEFAULT '0'/* COMMENT '��ҹ�ְ'*/;


CREATE TABLE `player_bagequip` (
	`Key` bigint(20) NOT NULL,
	`Index` bigint(20) NOT NULL,
	PRIMARY KEY (`Key`,`Index`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

alter table player_bagequip add ConfigId INT(11) UNSIGNED NOT NULL DEFAULT '0' /*COMMENT '���ñ�ID'*/;
alter table player_bagequip add Level INT(11) UNSIGNED NOT NULL DEFAULT '0' /*COMMENT 'ǿ���ȼ�'*/;


