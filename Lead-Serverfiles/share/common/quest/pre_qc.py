import os
import sys

def split_by_quat(buf):
    p = False
    l = list(buf)
    l.reverse()
    s = ""
    res = []
    while l:
        c = l.pop()
        if c == '"':
            if p:
                s += c
                res.append(s)
                s = ""
            else:
                if len(s) != 0:
                    res.append(s)
                s = '"'
            p = not p
        elif c == "\\" and l and l[0] == '"':
            s += c
            s += l.pop()
        else:
            s += c
    
    if len(s) != 0:
        res.append(s)
    return res

def AddSepMiddleOfElement(l, sep):
    if not l: return l
    l.reverse()
    new_list = [l.pop()]
    while l:
        new_list.append(sep)
        new_list.append(l.pop())
    return new_list

def my_split_with_seps(s, seps):
    res = [s]
    for sep in seps:
        new_res = []
        for r in res:
            sp = r.split(sep)
            sp = AddSepMiddleOfElement(sp, sep)
            new_res += sp
        res = new_res
    return [r for r in res if r != '']

def my_split(s, seps):
    res = [s]
    for sep in seps:
        new_res = []
        for r in res:
            sp = r.split(sep)
            new_res += sp
        res = new_res
    return [r for r in res if r != '']

def MultiIndex(lst, key):
    return [i for i, s in enumerate(lst) if s == key]

def Replace(lines, parameter_table, keys):
    r = []
    for string in lines:
        l = split_by_quat(string)
        for s in l:
            if s.startswith('"'):
                r.append(s)
            else:
                tokens = my_split_with_seps(s, ["\t", ",", " ", "=", "[", "]", '-', '<', '>', '~', '!', '.', '(', ')'])
                for key in keys:
                    indices = MultiIndex(tokens, key)
                    for i in indices:
                        tokens[i] = str(parameter_table[key][0])
                r.extend(tokens)
    return r

def MakeParameterTable(lines, parameter_table, keys):
    start = 0
    for idx, line in enumerate(lines):
        line = line.strip("\n")
        if "--" in line:
            line = line[:line.find("--")]
        
        tokens = my_split(line, ["\t", ",", " ", "=", "[", "]", "\r", "\n"])
        if not tokens:
            continue
            
        if tokens[0] == "quest":
            return idx + 1
            
        if tokens[0] == "define":
            if tokens[1] == "group":
                group_name = tokens[2]
                group_value = []
                for val in tokens[3:]:
                    val = parameter_table.get(val, [val])[0]
                    group_value.append(val)
                parameter_table[group_name] = group_value
                keys.append(group_name)
            elif len(tokens) > 5:
                print(f"Line {idx+1}: Syntax error")
            else:
                name = tokens[1]
                value = tokens[2]
                if value in parameter_table:
                    value = parameter_table[value][0]
                parameter_table[name] = [value]
                keys.append(name)
    return start

def run(filename):
    parameter_table = {}
    keys = []

    filename = filename.strip()
    if not filename or not os.path.exists(filename):
        return False

    try:
        with open(filename, 'r', encoding='cp949', errors='ignore') as f:
            lines = f.readlines()
    except Exception as e:
        print(f"Error while reading {filename}: {e}")
        return False

    start = MakeParameterTable(lines, parameter_table, keys)
    
    if not keys:
        return False

    quest_lines = lines[start-1:]
    processed_content = Replace(quest_lines, parameter_table, keys)

    os.makedirs("pre_qc", exist_ok=True)
    
    with open(f"pre_qc/{filename}", "w", encoding='cp949') as f:
        for s in processed_content:
            f.write(s)
            
    return True