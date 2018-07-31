CREATE TABLE IF NOT EXISTS `ribbit` (
  `Name` varchar(64) NOT NULL,
  `Seqn` int(16) NOT NULL,
  `Flag` varchar(64) NOT NULL DEFAULT '',
  `TimeDetected` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP,
  `TimeDownloaded` timestamp NULL DEFAULT NULL,
  `Downloaded` tinyint(1) NOT NULL DEFAULT '0',
  PRIMARY KEY (`Name`,`Seqn`,`Flag`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
