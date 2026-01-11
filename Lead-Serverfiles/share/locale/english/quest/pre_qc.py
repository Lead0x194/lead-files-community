# -*- coding: 949 -*-
# 말 그대로 pre qc.
# 우리 퀘스트 언어에는 지역 변수만이 있고,
# state나, 심지어 when, function을 아우르는 전역 변수를 사용할 수 없다.
# 전역 '변수'의 사용은 언어의 구조상 사용이 불가하고, 별 의미가 없다.
# 하지만 전역 '상수'의 사용은 퀘스트 view 상으로 꼭 필요하기 때문에,
# fuction setting () 과 같은 함수를 이용하여, 
# 매번 테이블을 생성하여 전역 상수를 흉내내어 사용하였다.
# 이는 매우 비효율적이므로,
# c의 preprocesser와 같이 pre qc를 만들어 전역 상수를 사용할 수 있도록 하였다.
# 퀘스트를 qc로 컴파일 하기 전에 pre_qc.py를 통과하면,
# pre_qc.py는 define 구문을 처리하고, 그 결과를
# pre_qc/filename에 저장한다.

import sys

def split_by_quat (buf):
	p = False
	l = list (buf)
	l.reverse()
	s = ""
	res = []
	while l:
		c = l.pop()
		if c == '"':
			if p == True:
				s += c
				res += [s]
				s = ""
			else:
				if len (s) != 0:
					res += [s]
				s = '"'
			p = not p
		elif c == "\\" and l[0] == '"':
			s += c
			s += l.pop()
		else:
			s += c
	
	if len (s) != 0:
		res += [s]
	return res

def AddSepMiddleOfElement (l, sep):
	l.reverse()
	new_list = [l.pop()]
	while l:
		new_list.append (sep)
		new_list.append (l.pop())
	return new_list

def my_split_with_seps(s, seps):
	res = [s]
	for sep in seps:
		new_res = []
		for r in res:
			sp = r.split (sep)
			sp = AddSepMiddleOfElement (sp, sep)
			new_res += sp
		res = new_res
	new_res = []
	for r in res:
		if r != '':
			new_res.append (r)
	return new_res

def my_split(s, seps):
	res = [s]
	for sep in seps:
		new_res = []
		for r in res:
			sp = r.split (sep)
			new_res += sp
		res = new_res
	new_res = []
	for r in res:
		if r != '':
			new_res.append (r)
	return new_res
def MultiIndex (list, key):
	l = []
	i = 0
	for s in list:
		if s == key:
			l.append (i)
		i = i + 1
	return l

def Replace (lines, parameter_table, keys):
	r = []
	for string in lines:
		l = split_by_quat (string)
		for s in l:
			if s[0] == '"':
				r += [s]
			else:
				tokens = my_split_with_seps (s, ["\t", ",", " ", "=", "[", "]",'-','<','>','~','!','.','(',')'])
				for key in keys:
					try:
						idices = MultiIndex(tokens, key)
						for i in idices:
							tokens[i] = parameter_table[key][0]
					except:
						pass
				r += tokens
	return r

def MakeParameterTable(lines, parameter_table, keys):
	names = []
	values = []
	group_names = []
	group_values = []
	idx = 0
	for line in lines:
		idx += 1
		line = line.strip("\n")
		if (-1 != line.find("--")):
			line = line[0:line.find("--")]
		
		tokens = my_split(line, ["\t", ",", " ", "=", "[", "]", "\r", "\n"])
		if len(tokens) == 0:
			continue
		if cmp (tokens[0], "quest") == 0:
			start = idx
			break
		if cmp (tokens[0], "define") == 0:
			if cmp (tokens[1],  "group") == 0:
				group_value = []
				for value in tokens[3:]:
					if parameter_table.get(value, 0) != 0:
						value = prameter_table[value]
					group_value.append (value)
				parameter_table [tokens[2]] = group_value
				keys.append(tokens[2])
			elif len(tokens) > 5:
				print "%d %s" % (idx, "Invalid syntax")
				print "define [name] = [value]"
				print "define group [name] = \"[\"[v0],[v1], ... \"]\""
			else :
				if tokens[1] == "rgd_kill_num":
					print "fucking"
				value = tokens[2]
				if parameter_table.get(value, 0) != 0:
					value = prameter_table[value]
				parameter_table[tokens[1]] = [value]
				keys.append (tokens[1])
	parameter_table = dict (zip (group_names, group_values))
	return start

def run(filename):
	parameter_table = dict()
	keys = []

	filename = filename.strip("\n")
	if filename == "":
		return
	lines = open (filename).readlines()
	start = MakeParameterTable (lines, parameter_table, keys)
	if len (keys) == 0:
		return False

	lines = lines [start-1:]
	r = Replace (lines, parameter_table, keys)
	f = file ("pre_qc/"+filename, "w")
	for s in r:
		 f.write(s)
	return True
