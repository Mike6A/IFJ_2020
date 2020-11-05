#!/usr/bin/python3

import os
import subprocess
import json

FNULL = open(os.devnull, 'w')
test_files = []
json_file = json.load(open("./list.json", "r"))

for i in os.listdir("./"):
    if not "test.py" in os.path.basename(i) and not "list.json" in os.path.basename(i):
        test_files.append(i)

for f in test_files:
    print("\n------{}----------".format(f))
    file = open(f, "r")
    res = subprocess.call(["../IFJ20"], stdin=file, stdout=FNULL)
    if json_file[f]['expected_output'] == res:
        print("result ==== OK")
    else:
        print("result ==== FAILED")
        print(json_file[f]['description'])
