# Interoperable_Container_Runtime
** **
## Presentations

Final Demo: [Video](https://youtu.be/25ImmuQUdC4)

Project Report: [PDF](https://github.com/BU-NU-CLOUD-F19/Interoperable_Container_Runtime/blob/master/report/cloud-course-final-report/Article.pdf) -- **Please take a look at our report!

Demo-1. [Slides](https://docs.google.com/presentation/d/1bMloLDt2xd2_FndwoQxTObwoqmAbbAUSy2fq8pvovYQ/edit#slide=id.g61835f440d_2_260)

Demo-2. [Slides](https://docs.google.com/presentation/d/13oyb9Et6FaSwatqjOO1UZ1DuA2F75J4_1z9GpLquNSw/edit#slide=id.g61835f440d_2_260)

Demo-3. [Slides](https://docs.google.com/presentation/d/11xGzlamKRxR4OkDQJ18vTwfvxp4XdIXRqlegtRix78o/edit?usp=sharing)

Demo-4. [Slides](https://docs.google.com/presentation/d/1SLH9yXW7gE4jdjYU8HPlak5v57JpW7kvapLHjoONeQU/edit#slide=id.g73cf6cf64a_0_9)

Demo-5. [Slides](https://docs.google.com/presentation/d/1tij8mnxX2RUFK7nxExzcri_sR3nQiFUgGrjuB862RYM/edit?usp=sharing)

Kafka Paper Presetation [Slides](https://docs.google.com/presentation/d/1s-PE9H6SfO_vgoV0IfJc345ZzA3LWJY5YoIdYANDnd0/edit?ts=5db0948e#slide=id.p)

## Installation and Usage Instructions

Currently, the interoperable_app, interoperable_image_app, and image_approval_manager exist as separate scripts written in Python3. To install, simply copy the python code to the server the containers are running on. In addition, files image_approvedlist.txt and image_blocklist.txt must be added in the same location. These files are used to denote image IDs that are known to be trusted images (image_approvedlist), and images which have been evaluated and lack approval (image_blocklist).

The modules sys, os, json, pyfiglet, colorama must all be present for the applications to run properly.

To run checks on the containers, invoke the interoperable_app from the commandline as follows:
python3 interoperable_app \<container type\> \<container ID\>
 
Where container types are:  
Docker: --docker or --d  
cir-o: --crio or --c  
containerd: --containerd or --ctr  

To run checks on the image file a container is running, invoke the interoperable_image_app from the commandline as follows:
python3 interoperable_image_app \<container type\> \<container ID\>

Where container types are:   
Docker: --docker or --d    
cir-o: --crio or --c  

interoperable_image_app can be called containerd for forward compatibility purposes, however at present this merely displays a message that containerd is not supported.

image_approval_manager writes image ids to approved and block lists which are checked against when the interoperable_image_app is called; part of the check is against whether the image is approved. To add an image id to a list, invoke from the commandline as follows:
python3 image_approval_manager \<action\> \<image ID\>
 
Where action is:  
Add to approved list: -a  
Add to block list: -b  

** **

## Abstract

Virtualization of resources has emerged in recent decades as a means to run multiple OSes on the same hardware. This particularly serves a useful function as this allows multiple applications to coexist on the same server, enabling efficencies in computing such as server consolidation.

Traditional VMs virtualize hardware resources, which results in the VMs taking up more resources. As such, OS-level virtualization, or Containers, have been developed. By sharing OS resources, containers are lightweight and can be spun up quickly while taking up fewer resources. Docker, introduced in 2013, is a popular runtime to manage containers as it addresses end-to-end management. However, Docker was initially a monolith with features not inherently dependent on each other being bundled together. As a result, alternative runtimes such as CRI-O and contianerd exist which implement container management at varying levels. [1]

The Open Container Initiative (OCI, https://www.opencontainers.org) has been established to create a open standard for container use regardless of the runtime being used to manage the container. However, the OCI only specifies downloading image then unpacking that image into an OCI Runtime filesystem bundle. 

It does not standardize lifecycle management of the containers, thus each container implements lifecycle functionality in a different manner. It also does not ensure that consistent standards for the security of containers are present. 


In this project, we will study the differences in popular runtimes Docker, containerd, and crio. Our focus will be on developing a service focused on ensuring that Center for Internet Security (CIS) Benchmarks for Docker are satisfied across other runtimes to ensure consistent application of security principles irrespective of container runtime differeces. Long term, the goal is to implement a lifecycle mangement solution that enables a common management framework for controlling containers across enviornments. By implementing a service to validate standard secuirty checks across runtimes, we intend to provide a Proof of Concept that such a common lifecycle management is possible.


** **

## 1.   Vision and Goals Of The Project:

Currently if someone wishes to launch an image in a container or perform any other lifecycle management functions on it, they must be sure that the scripts are configured correctly for the target container. For instance, launching images in Docker differs from doing so in CRI-O or containerd. This locks individuals and businesses into whichever container runtime they started with unless they invest the time required to edit the configuration and their scripts which holds the commands for target container. 

Our short term goal for this project is to enable the set of Container Runtime tests run in the CIS Docker 1.13.0 Benchmark across any container runtime. These tests are specified in Chapter 5 of the document; example checks include restricting Linux Kernel capabilities within containers, limiting memory usage, and avoiding directly exposing the host devices to the containers. Publishing a minumum viable framework for this purpose will enable users to run their security checks using **a single script across the most popular containers.** 

The ultimate goal is to develop an interoperable container runtime tool that allows user to perform common container lifecycle management functions among different runtimes using a single framework (e.g. start/stop execution, ps). 

## 2. Users/Personas Of The Project:

The intended user is the Chief Information Security Officer or their designee of a medium- or large-scale cloud organization, who is tasked with ensuring that containers running their clusters meet requirements for secure containers. 

This in particular represents a change from our origional proposal due to a shift to primarily considering the security aspects of containers - further information is detailed in the final report PDF.

## 3.   Scope and Features Of The Project:

The runtimes in scope for capatibility for this project will be Docker, and CRI-O. containerd is considered a runtime in scope as a stretch goal.

This project aims to esnure that the framework implements commands that satisfy the CIS Docker 1.13.0 Benchmark related to Container Runtimes across our in-scope runtimes. In doing so, users will be enabled to run their security checks with a single script rather than requiring separate suites for each runtime. The MVP will be considered to be implementing select benchmarks in consultation with our mentor. Benchmarks in Bold type indicate successful impelmentation in all runtimes. Italics indicate a benchmark implemented in some but not all runtimes. We were able to implement the majority of the benchmarks related to the container runtimes, but did not meet our stretch goal of implementing the full suite.

These benchmarks are specified in pp 126-180 of the Benchmark documentation, and consists of the following checks:

* **Do not disable AppArmor Profile** 
* **Verify SELinux security options, if applicable**
* **Restrict Linux Kernel Capabilities within containers** 
* **Do not use privileged containers** 
* **Do not mount sensitive host system directories on containers** 
* **Do not run ssh within containers** 
* **Do not map privileged ports within containers** 
* *Open only needed ports on container* 
* **Do not share the host's network namespace**
* **Limit memory usage for container** 
* **Set container CPU priority appropriately** 
* **Mount container's root filesystem as read only** 
* **Bind incoming container traffic to a specific host interface**
* *Set the 'on-failure' container restart policy to 5* 
* **Do not share the host's process namespace** 
* **Do not share the host's IPC namespace** 
* **Do not directly expose host devices to containers**
* *Override default ulimit at runtime only if needed* 
* Do not set mount propagation mode to shared 
* **Do not share the host's UTS namespace** 
* **Do not disable default seccomp profile** 
* Do not docker exec commands with privileged option 
* Do not docker exec commands with user option 
* **Confirm cgroup usage** 
* **Restrict container from acquiring additional privileges**
* Check container health at runtime 
* Ensure docker commands always get the latest version of the image 
* **Use PIDs cgroup limit** 
* Do not use Docker's default bridge docker0 
* **Do not share the host's user namespaces**
* Do not mount the Docker socket inside any containers 

We have an additional stretch goal to implement checks on Container Images and Build Files from the same CIS Docker 1.13.0 Benchmark document. The checks will only be implemented in Docker in CRI-O at present due to the linkages between container and image file in containerd requiring additional investigation. These checks are specified on pp 105-125 of the Doucmentation, and consist of the following (Bold: full implementation; Italics: partial impelementation):

* **Create a user for the container**  
* **Use trusted base images for containers** 
* Do not install unnecessary packages in the container 
* Scan and rebuild the images to include security patches 
* *Enable Content trust for Docker* 
* *Add HEALTHCHECK instruction to the container image* 
* Do not use update instructions alone in the Dockerfile 
* Remove setuid and setgid permissions in the images
* **Use COPY instead of ADD in Dockerfile** 
* Do not store secrets in Dockerfiles 
* Install verified packages only 

[See https://www.cisecurity.org/benchmark/docker/]

** **

## 4. Solution Concept

Global Architectural Structure Of the Project:

![alt text](https://github.com/BU-NU-CLOUD-F19/Interoperable_Container_Runtime/blob/master/figures/cloud-architecture.png "Hover text")

Interoperable framework will be a service which exists between developer and container runtimes. Our project aims to implement security checks across runtimes. The ultimate end goal will be to provide a means of lifecycle management of container runtimes in interoperable way. That is, a single script would be able to execute lifecyle commands independent of underlying container runtime. 

Design Implications and Discussion:

* The underlying structure of the configuration files per runtime will be evaluated, as this is what we use in making each of these checks.
* The interoperable framework will consist of scripts to be developed in Python. 
* The scripts serve as wrappers for functionality so that the end-user can run commands without caring which container runtime is the target container is running on.

Architecture of the Docker runtime:

![alt text](https://github.com/BU-NU-CLOUD-F19/Interoperable_Container_Runtime/blob/master/figures/Docker-architecture.png "Hover text")

Per the OCI standard, Docker, CRI-O, and containerd all use runc to implement low-level functionality. Thus, our intention is to leverage runc to create our interoperable framework. As seen in the above diagram, Docker implements its own image management and APIs for high level functionality on top of the container. Thus, understanding the interaction between the high level functionality and operations in runc is necessary in order to be able to implement the functionality in other frameworks.

## 5. Acceptance criteria

The mininum acceptance criteria is to enable at least 5 benchmark checks on two different container runtimes. We will target Docker and CRI-O as our primary focus. Our stretch goal criteria include ensuring the commands also work with containerd and be able to run the full suite of CIS Docker Benchmark checks from Chapters 5 (Container Runtime checks) and Chapter 4 (Container Images and Build File) on those runtimes.


## 6.  Release Planning:

Release #1 (Completed): 

* Set up containers within Docker, CRI-O, and containerd 
* Start experimentation with life-cycle functions on runtimes
* Give a detailed report about implementation of requested functionalities 

Release #2 (Completed): 

* Analysis of structure of runtime configuration files to be able to target our application.
* Emprical analysis/evaluation of the overhead in high-level runtimes (e.g., Docker) as opposed to using only runc. 

Release #3 (Completed): 

* Begin development on Application framework
* Implement and Demo check on Benchmark 5.1: Do not disable AppArmor Profile, across all in-scope Runtimes
* Implement and Demo check on Benchmark 5.3: Restrict Linux Kernel Capabilities within containers, across all in-scope Runtimes 

Release #4 (Completed): 

* Implement and Demo check on Benchmark 5.10: Limit memory usage for container (Scored)
* Implement and Demo check on Benchmark 5.11: Set container CPU priority appropriately (Scored)
* Implement and Demo check on Benchmark 5.24: Confirm cgroup usage (Scored)
* Begin evaluating documentation for Container Images and Build File checks

Release #5 (Completed): 

* Implement and demo check on Benchmark 5.6: Do not run ssh within containers
* Implement and demo check on Benchmark 5.28: Use PIDs cgroup limit
* Implement and demonstrate check on Benchmark 4.1: Create a user for the container
* Implement and demonstrate check on Benchmark 4.2: Use trusted base images for containers
* Implement and demonstrate check on Benchmark 4.9: Use COPY instead of ADD in Dockerfile


Release #6 (due by Week 13): 

* Finalize report and demo end-to-end
* Demo with remaining Benchmarks from Chapters 4 and 5. [Stretch goal] - Failed to implemenent full suite, see report for further details.


** **
## 7. Concerns

The main questions and concerns we had when inititally starting the project was regarding understanding how the various runtimes operate. The differences in implementations were not necessarily clear. In particular, understanding how CRI-O works, and why it was developed proved to be a difficulty in our inital analysis of the container runtime functionality. 

As of the end of the semester, the primary concerns are in defining how to establish a best practice for what the various container settings "should" be set to. This stems primarily from a lack of centralized recommendation as containers by nature are flexible, and recommended limits for one particular application and/or organization.

In addition, one late roadblock emerged from a follow up discussion where it was clarified what the intended user/use case should be for the application as developed. This in turn necessetates determining how to stand up the application as a service running on a Kubernetes cluster. Doing so unforutnately will need to be implemented as part of future work.

** **

## General comments

* This project has an oppurtinity to turn into a research paper that can be submitted to top-tier systems conference (based on usage of application in evaluating relative security of container images)
* Architecture diagram will be updated with the more information gained from implementations

** **

## References

[1]: Container Runtimes Part 1: An Introduction to Container Runtimes. https://www.ianlewis.org/en/container-runtimes-part-1-introduction-container-r

[2]: What’s the difference between runc, containerd, docker?. https://medium.com/@alenkacz/whats-the-difference-between-runc-containerd-docker-3fc8f79d4d6e

[3]: Docker components explained. http://alexander.holbreich.org/docker-components-explained/

[4]: opencontainers/runc github repo. https://github.com/opencontainers/runc


** **


