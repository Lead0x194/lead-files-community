-- MySQL dump 10.13  Distrib 5.1.35, for portbld-freebsd7.2 (i386)
--
-- Host: 202.31.212.73    Database: canada_player
-- ------------------------------------------------------
-- Server version	5.1.40-log

/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES latin1 */;
/*!40103 SET @OLD_TIME_ZONE=@@TIME_ZONE */;
/*!40103 SET TIME_ZONE='+00:00' */;
/*!40014 SET @OLD_UNIQUE_CHECKS=@@UNIQUE_CHECKS, UNIQUE_CHECKS=0 */;
/*!40014 SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0 */;
/*!40101 SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='NO_AUTO_VALUE_ON_ZERO' */;
/*!40111 SET @OLD_SQL_NOTES=@@SQL_NOTES, SQL_NOTES=0 */;

--
-- Table structure for table `mob_proto`
--

DROP TABLE IF EXISTS `mob_proto`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `mob_proto` (
  `vnum` int(11) NOT NULL DEFAULT '0',
  `name` varchar(24) NOT NULL DEFAULT 'Noname',
  `gb2312name` varbinary(24) NOT NULL DEFAULT 'Noname',
  `rank` tinyint(2) NOT NULL DEFAULT '0',
  `type` tinyint(2) NOT NULL DEFAULT '0',
  `battle_type` tinyint(1) NOT NULL DEFAULT '0',
  `level` smallint(3) NOT NULL DEFAULT '1',
  `size` enum('SMALL','MEDIUM','BIG') NOT NULL DEFAULT 'SMALL',
  `ai_flag` set('AGGR','NOMOVE','COWARD','NOATTSHINSU','NOATTCHUNJO','NOATTJINNO','ATTMOB','BERSERK','STONESKIN','GODSPEED','DEATHBLOW','REVIVE') DEFAULT NULL,
  `mount_capacity` tinyint(2) NOT NULL DEFAULT '0',
  `setRaceFlag` set('ANIMAL','UNDEAD','DEVIL','HUMAN','ORC','MILGYO','INSECT','FIRE','ICE','DESERT') NOT NULL DEFAULT '',
  `setImmuneFlag` set('STUN','SLOW','FALL','CURSE','POISON','TERROR') NOT NULL DEFAULT '',
  `empire` tinyint(4) NOT NULL DEFAULT '0',
  `folder` varchar(100) NOT NULL DEFAULT '',
  `on_click` tinyint(4) NOT NULL DEFAULT '0',
  `st` smallint(5) unsigned NOT NULL DEFAULT '0',
  `dx` smallint(5) unsigned NOT NULL DEFAULT '0',
  `ht` smallint(5) unsigned NOT NULL DEFAULT '0',
  `iq` smallint(5) unsigned NOT NULL DEFAULT '0',
  `damage_min` smallint(5) unsigned NOT NULL DEFAULT '0',
  `damage_max` smallint(5) unsigned NOT NULL DEFAULT '0',
  `max_hp` int(10) unsigned NOT NULL DEFAULT '0',
  `regen_cycle` tinyint(3) unsigned NOT NULL DEFAULT '0',
  `regen_percent` tinyint(3) unsigned NOT NULL DEFAULT '0',
  `gold_min` int(11) NOT NULL DEFAULT '0',
  `gold_max` int(11) NOT NULL DEFAULT '0',
  `exp` int(10) unsigned NOT NULL DEFAULT '0',
  `def` smallint(5) unsigned NOT NULL DEFAULT '0',
  `attack_speed` smallint(6) unsigned NOT NULL DEFAULT '100',
  `move_speed` smallint(6) unsigned NOT NULL DEFAULT '100',
  `aggressive_hp_pct` tinyint(3) unsigned NOT NULL DEFAULT '0',
  `aggressive_sight` smallint(10) unsigned NOT NULL DEFAULT '0',
  `attack_range` smallint(5) unsigned NOT NULL DEFAULT '0',
  `drop_item` int(10) unsigned NOT NULL DEFAULT '0',
  `resurrection_vnum` int(10) unsigned NOT NULL DEFAULT '0',
  `enchant_curse` tinyint(4) unsigned NOT NULL DEFAULT '0',
  `enchant_slow` tinyint(4) unsigned NOT NULL DEFAULT '0',
  `enchant_poison` tinyint(4) unsigned NOT NULL DEFAULT '0',
  `enchant_stun` tinyint(3) unsigned NOT NULL DEFAULT '0',
  `enchant_critical` tinyint(3) unsigned NOT NULL DEFAULT '0',
  `enchant_penetrate` tinyint(3) unsigned NOT NULL DEFAULT '0',
  `resist_sword` tinyint(4) NOT NULL DEFAULT '0',
  `resist_twohand` tinyint(4) NOT NULL DEFAULT '0',
  `resist_dagger` tinyint(4) NOT NULL DEFAULT '0',
  `resist_bell` tinyint(4) NOT NULL DEFAULT '0',
  `resist_fan` tinyint(4) NOT NULL DEFAULT '0',
  `resist_bow` tinyint(4) NOT NULL DEFAULT '0',
  `resist_fire` tinyint(4) NOT NULL DEFAULT '0',
  `resist_elect` tinyint(4) NOT NULL DEFAULT '0',
  `resist_magic` tinyint(4) NOT NULL DEFAULT '0',
  `resist_wind` tinyint(4) NOT NULL DEFAULT '0',
  `resist_poison` tinyint(4) NOT NULL DEFAULT '0',
  `dam_multiply` float DEFAULT NULL,
  `summon` int(11) DEFAULT NULL,
  `drain_sp` int(11) DEFAULT NULL,
  `mob_color` int(10) unsigned DEFAULT NULL,
  `polymorph_item` int(10) unsigned NOT NULL DEFAULT '0',
  `skill_level0` tinyint(3) unsigned DEFAULT '0',
  `skill_vnum0` int(10) unsigned DEFAULT '0',
  `skill_level1` tinyint(3) unsigned DEFAULT '0',
  `skill_vnum1` int(10) unsigned DEFAULT '0',
  `skill_level2` tinyint(3) unsigned DEFAULT '0',
  `skill_vnum2` int(10) unsigned DEFAULT '0',
  `skill_level3` tinyint(3) unsigned DEFAULT '0',
  `skill_vnum3` int(10) unsigned DEFAULT '0',
  `skill_level4` tinyint(3) unsigned DEFAULT '0',
  `skill_vnum4` int(10) unsigned DEFAULT '0',
  `sp_berserk` tinyint(4) NOT NULL DEFAULT '0',
  `sp_stoneskin` tinyint(4) NOT NULL DEFAULT '0',
  `sp_godspeed` tinyint(4) NOT NULL DEFAULT '0',
  `sp_deathblow` tinyint(4) NOT NULL DEFAULT '0',
  `sp_revive` tinyint(4) NOT NULL DEFAULT '0',
  PRIMARY KEY (`vnum`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1;
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
