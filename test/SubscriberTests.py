#!/usr/bin/python3

import sys, os, signal, datetime, time

def signal_handler(sig, frame):
	print('Execution aborted by user')
	sys.exit(0)
signal.signal(signal.SIGINT, signal_handler)

folder = ""

def writeTest(filename, text):
        with open(filename, "a") as myfile:
                myfile.write(text + "\n")

if len(sys.argv) == 1:
    folder = os.getcwd()

if len(sys.argv) == 2:
    folder = os.path.abspath(sys.argv[1])

#if len(sys.argv) == 4:
#    user = sys.argv[1]
#    password = sys.argv[2]
#    folder = os.path.abspath(sys.argv[3])
#    if not os.path.ismount("/mnt/perfshare"):
#        os.system("mkdir -p /mnt/perfshare")
#        os.system("mount -t nfs 192.168.1.4:/var/lib/jenkins/perfshare /mnt/perfshare")
#        print("/mnt/perfshare mounted")
#    else:
#        print("/mnt/perfshare is already mounted")

if not folder:
    print("Error")
    sys.exit(-1)

now = datetime.datetime.now()

writeTest("/mnt/perfshare/output/sub.log", "---------------------------------")
writeTest("/mnt/perfshare/output/sub.log", "Start Subscriber Test - " + now.strftime("%Y-%m-%d_%H-%M-%S"))
os.system("mkdir -p /mnt/perfshare/output")
writeTest("/mnt/perfshare/output/sub.log", "Start Tests")
os.chdir(folder)
os.system("python3 GenerateTestsAndXMLs.py '" + folder + "'")
time.sleep(5)
writeTest("/mnt/perfshare/output/sub.log", "Generated XML files")
writeTest("/mnt/perfshare/output/sub.log", "Start Tests")
os.system("python3 SubscriberTestList.py /mnt/perfshare/ '" + folder + "'")
writeTest("/mnt/perfshare/output/sub.log", "Tests Completed")