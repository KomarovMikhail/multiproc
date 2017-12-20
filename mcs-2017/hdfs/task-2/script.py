import sys
import subprocess

url = sys.argv[1][:-1]
sp = subprocess.Popen(["curl", "-s", url])
sp.communicate()

exit(0)
