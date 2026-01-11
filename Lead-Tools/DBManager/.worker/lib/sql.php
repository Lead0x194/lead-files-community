<?
class mysql
{
	var             $connection;
	var             $data;

	function mysql($host, $user, $pwd, $database, $charset)
	{
		$this->connection       = NULL;
		$this->data             = NULL;

		$this->connection       = mysql_connect($host, $user, $pwd) or $this->error("mysql_connect error");
		mysql_select_db($database, $this->connection) or $this->error("mysql_select_db error");
		mysql_set_charset($charset, $this->connection);
	}

	function restart($host, $user, $pwd, $database)
	{
		if ($this->data != NULL && $this->data != 1)
			mysql_free_result($this->data);

		if ($this->connection != NULL)
			mysql_close($this->connection);

		$this->connection       = mysql_connect($host, $user, $pwd) or $this->error("mysql_connect error");
		mysql_select_db($database, $this->connection) or $this->error("mysql_select_db error");
	}

	function fetch_row()
	{
		return mysql_fetch_row($this->data);
	}

	function fetch_array()
	{
		return mysql_fetch_array($this->data);
	}

	function result($row_num, $value_name)
	{
		return mysql_result($this->data, $row_num, $value_name);
	}

	function disconnect()
	{
		if ($this->connection != NULL)
		{
			mysql_close($this->connection);
			$this->connection = NULL;
		}
	}

	function num_rows()
	{
		return (mysql_num_rows($this->data));
	}

	function destroy()
	{
		if ($this->data != NULL && $this->data != 1)
			mysql_free_result($this->data);

		$this->disconnect();
	}

	function query($query_string)
	{
		if ($this->data != NULL)
		{
			if ($this->data != 1)
				mysql_free_result($this->data);

			$this->data = NULL;
		}

		$this->data = mysql_query($query_string, $this->connection) or $this->error("mysql_query error");

		if ($this->data)
			return TRUE;
		else
			return FALSE;
	}

	function query_nodie($query_string)
	{
		if ($this->data != NULL && $this->data != 1)
		{
			if ($this->data != 1)
				mysql_free_result($this->data);

			$this->data = NULL;
		}

		$this->data = mysql_query($query_string, $this->connection);

		if ($this->data)
			return TRUE;
		else
			return FALSE;
	}

	function error($header)
	{
		die($header.": ".mysql_error($this->connection));
	}

	function lock($rule)
	{
		$this->query("LOCK TABLES $rule") or $this->error("mysql LOCK TABLES error");
	}

	function unlock()
	{
		$this->query("UNLOCK TABLES") or $this->error("mysql UNLOCK TABLES error");
	}
};
?>

