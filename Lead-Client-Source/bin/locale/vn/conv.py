# -*- coding:cp949 -*-
import yconv
import glob
import os

names = glob.glob("*.utf8.txt")

for name in names:
	data = open(name, "rb").read()

	body = name[:name.find(".")]
	open(body + ".txt", "wb").write(yconv.conv(data, 65001, 1258))
