import json
import sys
from pyfiglet import Figlet
from colorama import Fore, Back, Style 

docker_congifg_v2_path = "/home/jonathan/BUGOOGDRIVE/Course_work/EC_528_Cloud_Computing/Docker_09bd4a/config.v2.json" #Docker config file, hardcode for testing purposes on local disk
docker_image_path = "/home/jonathan/BUGOOGDRIVE/Course_work/EC_528_Cloud_Computing/Docker_09bd4a/imagefile.json" # image file, hardcode for testing purposes on local disk

# config file attributes
image_hash = "Image"

# image file attributes
config_key = "config"
user_key = "User"
health_key = "Healthcheck"
history_key = "history"

# open config.v2 file to get image id associated with container
with open(docker_congifg_v2_path) as json_file:
	data = json.load(json_file) 
	image_id = data[image_hash] 
	# hash includes 'sha256:' to indicate hashtype, split off since image folder only includes ID
	image_id = image_id.split(':')[1]
	# open file corresponding to the image file
	with open(docker_image_path) as image_file:
		imdata = json.load(image_file)
		image_config = imdata[config_key]
		userid = image_config[user_key] if image_config[user_key] is not '' else 'NOT SET!'
		healthcheck = image_config[health_key] if health_key in image_config else 'NOT SET!'
		history = imdata[history_key]
		print(f'CIS 4.1: Create a user for the conatiner {userid}') # 4.1 check, user created
		print(f'CIS 4.6: Add HEALTHCHECK instruction to the container image {healthcheck}')
		# 4 benchmarks rely on reviewing the image history, but require manual review by nature
		print(f'Image history for review against following CIS benchmarks:')
		print(f'CIS 4.7: Do not use update instructions alone in Dockerfile')
		print(f'CIS 4.9: Use COPY instead of ADD in Dockerfile')
		print(f'CIS 4.10: Do not store secrets in Dockerfile')
		print(f'CIS 4.11: Install verified packages only')
		print(f'Review history against each bench mark to verify benchmarks pass:')
		for x in history: print(f'{x}')
	