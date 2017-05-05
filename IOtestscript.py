#!/usr/bin/python

import os, random, uuid

j=0
while(j<10000):
	file = open(str(uuid.uuid4()),"w+")
	file.write("Hello World")                                                    
	file.close()
	j=j+1

names = []

files = os.walk('/home/root')
for root, dirnames, filenames in files:
	for filename in filenames:
		names.append(os.path.join(root,filename))

i = 0
while (i<10000):
	fname = random.choice(names)
	f = open(fname,"r")
	f.read(1)
	f.close()
	i=i+1