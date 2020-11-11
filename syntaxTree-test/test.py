#!/usr/bin/python3

import os
import subprocess
import json

FNULL = open(os.devnull, 'w')
test_files = []
json_file = json.load(open("./list.json", "r"))
correct_count = 0
fail_count = 0

class bcolors:
    HEADER = '\033[95m'
    OKBLUE = '\033[94m'
    OKCYAN = '\033[96m'
    OKGREEN = '\033[92m'
    WARNING = '\033[93m'
    FAIL = '\033[91m'
    ENDC = '\033[0m'
    BOLD = '\033[1m'
    UNDERLINE = '\033[4m'

for i in os.listdir("./"):
    if not "test.py" in os.path.basename(i) and not "list.json" in os.path.basename(i):
        test_files.append(i)

for f in test_files:
    file = open(f, "r")
    res = subprocess.call(["../IFJ20"], stdin=file, stdout=FNULL)
    if json_file[f]['expected_output'] == res:
        print("\n------{}----------".format(f))
        print("result ==== OK")
        correct_count += 1
    else:
        print("{}\n------{}----------".format(bcolors.FAIL, f))
        print("result ==== FAILED")
        print("returned = {}".format(res))
        print("description: {}{}".format(json_file[f]['description'], bcolors.ENDC))
        fail_count += 1


print(f"{bcolors.OKBLUE}--------------------------")
print(f"--------------------------{bcolors.ENDC}")
print("{}OK:{}{}    {}FAIL:{}{}".format(bcolors.OKGREEN, correct_count, bcolors.ENDC, bcolors.FAIL, fail_count, bcolors.ENDC))
print(f"{bcolors.OKBLUE}--------------------------")
print(f"--------------------------{bcolors.ENDC}")
