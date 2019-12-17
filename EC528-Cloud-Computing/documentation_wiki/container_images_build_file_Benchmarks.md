Should prioirtize the "Score" over "Not-Scored" Benchmarks, as Scored ones have more rigidly defined security consequences, and will likely be easier to implement, as remediation points to specific attributes in the target file.

CRI-O help was of no use, need to follow up to figure out how to inspect images.

containerd uses Docker images. Supposedly has method to "manage" images, but this appears to be limited to pulling images from the docker.io repository, rather than Build them (not that it matters for our purposes as long as can inspect); ctr does not appear provide method to inspect the image, so properties cannot be checked easily.


Next steps: 

Begin designing code for Docker case for 4.1, 4.5; begin prototyping based on local copies to ensure syntax. 
refer to https://github.com/nginxinc/docker-nginx/blob/master/stable/alpine/Dockerfile/ for sample Dockerfile - pull and make edits on copy to generate alterante images; determine how to identify changes in env variables for app testing. 


Determine how to find images in CRI-O  
Determine if possible to inspect image info in containerd; if not, may not be possible to have full implementation unless information exists elsewhere  
*Imagefiles should be in /var/lib/docker (or perhaps similar)*  
Determine if container trust applies in CRI-O case (arguably does not in containerd case, as containerd does not provide mechanism to build the images)  
*refer to https://github.com/nginxinc/docker-nginx/blob/master/stable/alpine/Dockerfile/ for sample Dockerfile - pull and make edits on local copy as necessary to run tests*  


***
Local image store locations:

Docker - image label noted in Config.Image when inspecting Container.  

Questions?  
a) what file does this get pulled from (to be able to grab without running Docker command directly)  
b) how to associate the image label (e.g. Centos) with the hash label?

Answers, in /var/lib/docker/containers/<Container ID>/config.v2.json, property Image contains sha hash

json stored in /var/lib/docker/image/overlay2/imagedb/content/sha256/<Image ID>  

CRI-O - json stored in /var/lib/containers/storage/overlay-images/<Image ID>/mainfest  
*again, how to get ID to reference to access the file?*
 multiple levels of indirection - the image is contained in the config.json file (though somehow not universally, need to follow up on how the containers were created in the first place), but the ID used in the CRI-O files does not match the id/hash in the container config, so must consult images.json in overlay-images. 
 
 Follow up question - because these files can be changed in future updates, is there an interface that can be exploited to get the necessary information?
 
containerd - per Justin, the container files are in /var/lib/containerd/io.containerd.content.v1.content/blobs/sha256/  
 
 open question - where does the container itself reference which image it is using?

files for containers generally appear to be stored in /var/lib/containerd, but information absent. 

***

**4.1 Create a user**

Am looking for USER <username/ID> or equivalent in the image file; if this is missing, container is running as root which is the default behavior in Docker at least.

Inspecting image file, appears to be formatted similar to JSON? setting found under "Container Config": "User"


**4.5 Enable Content trust**

Disabled by default; provides ability to use digital signatures

in Docker, this is DOCKER_CONTENT_TRUST=1 which is an enviornment variable.

Is this necessarily applicable to the other containers? Further infomration needed

**4.6 Add HEALTHCHECK instruction**

Am looking for HEALTHCHECK in the image file; if not present health check is disabled

Audit looks for Config.Healthcheck value - This is from the container file rather than the image, so may be different there. Also need to know where dockerfiles live for containers we are using so can test this and determine where in the corresponding image file this shows up, to avoid calling this via simply using script as a wrapper for the Docker commands.

***

**4.2 Use trusted base images**

"Ensure that the container image is written either from scratch or is based on another established and trusted base image downloaded over a secure channel"

No easy way to do this programatically, audit relies on inspecting the host and then obtaining "proof of evidence" that the images were obtained from trusted source from the SysAdmin.

Could simply implement means of listing the image, but can't be flagged as pass/fail

**4.3 Do not install unncessary packages in the container**

Intended to ensure that the container matches with the basic concept that containers should represent a slimmed down version of the OS. However, this also requires knowing what the container is doing in order to determine whether an installed package is legitimate. Could implement a means to simply list the packages present, but again can't really classify this as pass/fail by nature.

**4.4 Scan and rebuild the images to include security patches** 

Requires running same command as previous step, except this time evaluating packages to determine that the most up to date version is installed. 

**4.7 Do not use update instructions alone in the Dockerfile**

Requires inspection of history in the images, and looking for update instructions in a single line. Undesirable as "Adding the update instructions in a single line on the Dockerfile will cache the update layer. Thus, when you build any image later using the same instruction, previously cached update layer will be used. This could potentially deny any fresh updates to go in the later builds." 

While Not Scored, might be possible to flag a fail due to looking for update statements.

**4.8 Remove setuid and setgid permissions in the images**

Requires reviewing executables in the image with setuid/setgid permissions, and ensuring that they are "legitimate". THerefore, manual review is required, cannot flag as pass/fail by default

 **4.9 Use COPY instead of ADD in Dockerfile**
 
 Requires inspecting the file - could flagg as "fail" based on presence of ADD statements.
 
 **4.10 Do not store secrets in Dockerfiles** 
 
 Requires inspection of history, though "secrets" are illdefined in the Benchmark document.
 
 **4.11 Install verified packages only**
 
 Inspect history, use GPG keys or other secure mechanisms to validate authenticity of packages.
 
