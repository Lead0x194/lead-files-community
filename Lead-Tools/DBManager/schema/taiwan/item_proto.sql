-- MySQL dump 10.13  Distrib 5.1.35, for portbld-freebsd7.2 (i386)
--
-- Host: 202.31.212.73    Database: japan_player
-- ------------------------------------------------------
-- Server version	5.1.40-log

/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES big5 */;
/*!40103 SET @OLD_TIME_ZONE=@@TIME_ZONE */;
/*!40103 SET TIME_ZONE='+00:00' */;
/*!40014 SET @OLD_UNIQUE_CHECKS=@@UNIQUE_CHECKS, UNIQUE_CHECKS=0 */;
/*!40014 SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0 */;
/*!40101 SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='NO_AUTO_VALUE_ON_ZERO' */;
/*!40111 SET @OLD_SQL_NOTES=@@SQL_NOTES, SQL_NOTES=0 */;

--
-- Table structure for table `item_proto`
--

DROP TABLE IF EXISTS `item_proto`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `item_proto` (
  `vnum` int(11) unsigned NOT NULL DEFAULT '0',
  `name` varbinary(24) NOT NULL DEFAULT 'Noname',
  `locale_name` varbinary(24) NOT NULL DEFAULT 'Noname',
  `type` tinyint(2) NOT NULL DEFAULT '0',
  `subtype` tinyint(2) NOT NULL DEFAULT '0',
  `weight` tinyint(3) DEFAULT '0',
  `size` tinyint(3) DEFAULT '0',
  `antiflag` int(11) DEFAULT '0',
  `flag` int(11) DEFAULT '0',
  `wearflag` int(11) DEFAULT '0',
  `immuneflag` set('PARA','CURSE','STUN','SLEEP','SLOW','POISON','TERROR') NOT NULL DEFAULT '',
  `gold` int(11) DEFAULT '0',
  `shop_buy_price` int(10) unsigned NOT NULL DEFAULT '0',
  `refined_vnum` int(10) unsigned NOT NULL DEFAULT '0',
  `refine_set` smallint(11) unsigned NOT NULL DEFAULT '0',
  `refine_set2` smallint(5) unsigned NOT NULL DEFAULT '0',
  `magic_pct` tinyint(4) NOT NULL DEFAULT '0',
  `limittype0` tinyint(4) DEFAULT '0',
  `limitvalue0` int(11) DEFAULT '0',
  `limittype1` tinyint(4) DEFAULT '0',
  `limitvalue1` int(11) DEFAULT '0',
  `applytype0` tinyint(4) DEFAULT '0',
  `applyvalue0` int(11) DEFAULT '0',
  `applytype1` tinyint(4) DEFAULT '0',
  `applyvalue1` int(11) DEFAULT '0',
  `applytype2` tinyint(4) DEFAULT '0',
  `applyvalue2` int(11) DEFAULT '0',
  `value0` int(11) DEFAULT '0',
  `value1` int(11) DEFAULT '0',
  `value2` int(11) DEFAULT '0',
  `value3` int(11) DEFAULT '0',
  `value4` int(11) DEFAULT '0',
  `value5` int(11) DEFAULT '0',
  `socket0` tinyint(4) DEFAULT '-1',
  `socket1` tinyint(4) DEFAULT '-1',
  `socket2` tinyint(4) DEFAULT '-1',
  `socket3` tinyint(4) DEFAULT '-1',
  `socket4` tinyint(4) DEFAULT '-1',
  `socket5` tinyint(4) DEFAULT '-1',
  `specular` tinyint(4) NOT NULL DEFAULT '0',
  `socket_pct` tinyint(4) NOT NULL DEFAULT '0',
  `addon_type` smallint(6) NOT NULL DEFAULT '0',
  PRIMARY KEY (`vnum`)
) ENGINE=MyISAM DEFAULT CHARSET=big5;
/*!40101 SET character_set_client = @saved_cs_client */;
/*!40103 SET TIME_ZONE=@OLD_TIME_ZONE */;

/*!40101 SET SQL_MODE=@OLD_SQL_MODE */;
/*!40014 SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS */;
/*!40014 SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
/*!40111 SET SQL_NOTES=@OLD_SQL_NOTES */;

-- Dump completed
