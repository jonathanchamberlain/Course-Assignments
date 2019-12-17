import json
import sys
from pyfiglet import Figlet
from colorama import Fore, Back, Style 
import os

# define custom app font
custom_fig = Figlet(font='doom')

# Benchmark check strings
Fp1 = "CIS 4.1: Create a user for the conatiner: "
Fp2 = "CIS 4.2: Use trusted base images for containers: "
Fp5 = "CIS 4.5: Enable Content Trust for Docker: "
Fp6 = "CIS 4.6: Add HEALTHCHECK instruction to the container image: "
Fp9 = "CIS 4.9: Use COPY instead of ADD in Dockerfile: "

# Define functions to execute each check
def userid_check(userid):
	# Perform check 4.1, create a user
	if userid == 'NOT SET!' or userid == 0:
		print(f"{Fp1} " + Fore.RED + f"FAILURE, USER IS ROOT")
	else:
		print(f"{Fp1} " + Fore.GREEN + f"PASS, USER ID = {userid}")
	print(Style.RESET_ALL + '')

def trusted_image_check(imageid):
	'''
	Perform check 4.2, use trusted base images for containers
	Check depends on validation by sysadmin, thus check against
	predefined approvedlist and blocklist. If image on neither, 
	check neither passes nor fails, but is flagged for followup

	For testing purposes, image_approvedlist.txt and image_blocklist.txt
	assumed to be created and popualted. Need either additional utilitiy 
	to manage this or the ability to pull from an alternate data structure
	'''
	blocked = False # flag to determine if in blocklist
	approved = False # flag to determine if in approved list

	# check if image in approved list
	fa = open("image_approvedlist.txt","r")
	fa1 = fa.readlines()
	for line in fa1:
		if imageid in line:
			approved = True
	fa.close()

	if approved:
		print(f"{Fp2}" + Fore.GREEN + f'PASSED: IMAGE {imageid} ON APPROVED LIST')
	else:
		# check if image in blocked list
		fb = open("image_blocklist.txt","r")
		fb1 = fb.readlines()
		for line in fb1:
			if line == imageid:
				blocked = True
		fb.close()
		if blocked:
			print(f"{Fp2}" + Fore.RED + f'FAILURE: IMAGE {imageid} ON BLOCKLIST')
		else:
			print(f"{Fp2} " + Fore.YELLOW + f'REQUIRES FOLLOW UP: UNIDENTIFIED SOURCE. PLEASE VALIDATE SOURCE OF IMAGE {imageid} WITH SYSADMIN')
	print(Style.RESET_ALL + '')

def content_trust_check():
	trust_key = 'DOCKER_CONTENT_TRUST' # key for enviornment variable
	#Perform check 4.5, enable trust - tied to enviornment variable
	content_trust = os.environ.get(trust_key,'')
	if content_trust == '1':
		print(f"{Fp5} " + Fore.GREEN + f'PASS, {trust_key} IS 1')
	else:
		print(f'{Fp5}' + Fore.RED + f'FAILURE, {trust_key} IS NOT 1')
	print(Style.RESET_ALL + '')

def healthcheck_check(healthcheck):
	# Perform check 4.6, add HELATHCHECK instruction
	followup = False
	if healthcheck == 'NOT SET!':
		print(f"{Fp6} " + Fore.RED + f"FAILURE, HEALTHCHECK INSTRUCTIONS {healthcheck}")
	'''
	Add check for curl/iwr, which are not necessarily recommended due to dependencies not necessarily
	existing in the image
		'''
	elif 'curl' in healthcheck or 'iwr' in healthcheck:
		print(f"{Fp6} " + Style.YELLOW + f"PARTIAL PASS, HEALTHCHECK INSTRUCTION PRESENT: {healthcheck}")
		print(f"USE OF CURL/IWR REQUIRE DEPENDENCIES IN IMAGE, VALIDATE PROPER INSTRUCTIONS WITH SYSADMIN")
	else: 
		print(f"{Fp6} " + Style.GREEN + f"PASS, HEALTHCHECK INSTRUCTION PRESENT: {healthcheck}")
	print(Style.RESET_ALL + '')

def history_check_docker(history):
	# Perform check 4.9, use COPY not ADD
	print(f'{Fp9}')
	success = False # flag to mark if check passes
	failure = False # flag to mark if check fails
	created_key = "created" # timestamp of instruction
	created_by_key = "created_by" # instruction that caused the update
	add_times = [] # list to hold timestamps of add instructions
	for line in history:
		if 'ADD' in line[created_by_key]:
			add_times.append(line[created_key])
			failure = True
		elif 'COPY' in line[created_key]:
			success = True
	
	if failure:
		print(Fore.RED + f'FAILURE, ADD INSTRUCTIONS FOUND IN HISTORY AT:')
		for t in add_times: print(t)
	elif success:
		print(Fore.GREEN + f'PASS, COPY AND NO ADD INSTRUCTIONS FOUND IN HISTORY')
	else: 
		print(Fore.YELLOW + f'N/A, NO COPY OR ADD INSTRUCTIONS IN HISTORY')
	print(Style.RESET_ALL + '')

def history_check_crio(history):
	# Perform check 4.9, use COPY not ADD
	print(f'{Fp9}')
	success = False # flag to mark if check passes
	failure = False # flag to mark if check fails
	history_compatible_key = "v1Compatibility"
	created_key = "created" # timestamp of instruction
	config_key = "container_config" # contains instruciton in sub attribute
	command_key = "Cmd" # contains the instructions
	add_times = [] # list to hold timestamps of add instructions

	for line in history:
		# json dict is itself a string stored in the v1Compatibility attribute, extract
		v1Compatibility = json.loads(line[history_compatible_key])
		if 'ADD' in v1Compatibility[config_key][command_key]:
			add_times.append(v1Compatibility[created_key])
			failure = True
		elif 'COPY' in v1Compatibility[config_key][command_key]:
			success = True

	if failure:
		print(Fore.RED + f'FAILURE, ADD INSTRUCTIONS FOUND IN HISTORY AT:')
		for t in add_times: print(t)
	elif success:
		print(Fore.GREEN + f'PASS, COPY AND NO ADD INSTRUCTIONS FOUND IN HISTORY')
	else: 
		print(Fore.YELLOW + f'N/A, NO COPY OR ADD INSTRUCTIONS IN HISTORY')
	print(Style.RESET_ALL + '')


# helper to print help function
def help_func():
	if sys.argv[1] and sys.argv[1] == "--help" or sys.argv[1] == "--h":
		print("Please enter command as --<runtime> <container_id> to apply benchmarks on image container is running")
		print("\nExample: interoperable_image_app --crio 123134\n")
		print("Supported runtimes are:")
		print("Docker (--docker or --d)")
		print("CRI-O (--crio or --c)")
		print("containerd (--containerd or --ctrd)")
		return True
	return False

# define checks in Docker
def docker_inspect_image():
	# Docker file paths
	#global config_path_docker
	#global image_file_path_docker
	config_path_docker = "/var/lib/docker/containers/{}/config.v2.json"
	image_file_path_docker = "/var/lib/docker/image/overlay2/imagedb/content/sha256/{}"
	# attribute keys - in image file except for the hash key, which is in the config file
	image_hash_key = "Image" # holds sha has of the image
	config_key = "config" # holds basic configuration attributes
	user_key = "User" # User id, if present, found in config
	health_key = "Healthcheck" # Healthcheck instruction, if present, found in config
	history_key = "history" # holds updates to the underlying image

	if sys.argv[1] and sys.argv[1] == "--docker" or sys.argv[1] == "--d":
		config_path_docker = config_path_docker.format(sys.argv[2])
		# open docker container configuration file to get image id
		with open(config_path_docker) as config_json:
			config_data = json.load(config_json)
			# hash includes 'sha256:' to indicate hashtype, split off since image folder only includes ID
			image_id = config_data[image_hash_key].split(':')[1]
			# format image file path
			image_file_path_docker = image_file_path_docker.format(image_id)
			with open(image_file_path_docker) as image_json:
				image_data = json.load(image_json)
				# userid and healthcheck are sub attributes of config attribute
				image_config = image_data[config_key]
				userid = image_config[user_key] if image_config[user_key] is not '' else 'NOT SET!'
				healthcheck = image_config[health_key] if health_key in image_config else 'NOT SET!'
				# history in its own attribute
				history = image_data[history_key]
				# print result of each check
				userid_check(userid)
				trusted_image_check(image_id)
				content_trust_check()
				healthcheck_check(healthcheck)
				history_check_docker(history)			
		return True
	return False

def crio_inspect_image():
	# paths to configuration files
	config_path_crio = "/var/lib/containers/storage/overlay-containers/{}/userdata/config.json"
	image_overlay_crio = "/var/lib/containers/storage/overlay-images/images.json"
	image_file_path_crio = "/var/lib/containers/storage/overlay-images/{}/manifest"
	# keys to attributes in the configuration file
	config_process_key = "process"
	user_key = "user"
	user_id_key = "uid" 
	container_annotation_crio = "annotations"
	container_image_ref_crio = "io.kubernetes.cri-o.ImageRef"
	# keys to attributes in the image overlay file
	image_overlay_digest = "digest"
	image_overlay_id = "id"
	# keys to attributes in image file
	history_key = "history"

	if sys.argv[1] and sys.argv[1] == "--crio" or sys.argv[1] == "--c":
		config_path_crio = config_path_crio.format(sys.argv[2])
		# open crio config file to get digest of associated image
		with open(config_path_crio) as config_json:
			config_data = json.load(config_json)
			# user id contained within process attribute
			userid = config_data[config_process_key][user_key][user_id_key]
			userid_check(userid)
			# configuration file stores digest reference, but this is not image id
			image_digest = config_data[container_annotation_crio][container_image_ref_crio].split('@')[1]
			# have digest hash, use this to indentify image id
			with open(image_overlay_crio) as image_overlay_json:
				images_overlay_data = json.load(image_overlay_json)
				# images stored as an unlabeled list
				num_images = len(images_overlay_data)
				i = 0
				image_id = None
				while (i < num_images) and (image_id == None):
					image = images_overlay_data[i]
					if image[image_overlay_digest] == image_digest:
						image_id = image[image_overlay_id]
					i += 1
				# have the id of the image, run trusted image check	
				trusted_image_check(image_id)
				# have image id, can now open relevant file
				image_file_path_crio = image_file_path_crio.format(image_id)
				with open(image_file_path_crio) as image_json:
					image_data = json.load(image_json)
					# history in its own attribute
					history = image_data[history_key]
					history_check_crio(history)
					# healthcheck possibly not component of CRI-O image?			
		return True
	return False

def ctrd_inspect_image():

	#image_file_ctrd = "/var/lib/containerd/io.containerd.content.v1.content/blobs/sha256/{}"

	if sys.argv[1] and sys.argv[1] == "--containerd" or sys.argv[1] == "--ctrd":
		print ("containerd is not currently supported")
		return True

	return False

def main():
	print(custom_fig.renderText('Interoperable Image!!'))
	if help_func():
		return
	elif crio_inspect_image():
	    return
	elif docker_inspect_image():
		return
	#elif ctrd_inspect_image():
	#   containerd_utils()
	else:
		print("Please use --help command for more information")

if __name__== "__main__":
	main()
	