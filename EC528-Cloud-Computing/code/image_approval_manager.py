import sys
import os

'''
Basic script to denote whether an image has been approved or blocked
Uses command line prompt.
Syntax when calling is is -a <Image ID> for approval; -b <Image ID> for block.
'''


if sys.argv[1] == '-a' or sys.argv[1] == '-A':
	f = open("image_approvedlist.txt","a")
	f.write(sys.argv[2] + "\n")
	f.close
elif sys.argv[1] == '-b' or sys.argv[1] == '-B':
	f = open("image_blocklist.txt", "a")
	f.write(sys.argv[2] + "\n")
	f.close
else:
	print("Second arugment must be -a to add to approved list, -b to add to block list")
