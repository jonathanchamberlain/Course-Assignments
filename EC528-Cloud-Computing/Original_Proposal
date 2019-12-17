# Interoperable_Container_Runtime
** **

## Abstract

Virtualization of resources has emerged in recent decades as a means to run multiple OSes on the same hardware. This particularly serves a useful function as this allows multiple applications to coexist on the same server, enabling efficencies in computing such as server consolidation.

Traditional VMs virtualize hardware resources, which results in the VMs taking up more resources. As such, OS-level virtualization, or Containers, have been developed. By sharing OS resources, containers are lightweight and can be spun up quickly while taking up fewer resources. Docker, introduced in 2013, is a popular runtime to manage containers as it addresses end-to-end management. However, Docker was initially a monolith with features not inherently dependent on each other being bundled together. As a result, alternative runtimes such as CRI-O and contianerd exist which implement container management at varying levels. [1]

The Open Container Initiative (OCI, https://www.opencontainers.org) has been established to create a open standard for container use regardless of the runtime being used to manage the container. However, the OCI only specifies downloading image then unpacking that image into an OCI Runtime filesystem bundle. It does not standardize lifecycle management of the containers, thus each container implements lifecycle functionality in a different manner. In this project, we will study the differences in popular runtimes Docker, containerd, and crio to implement a lifecycle mangement solution that enables a common management framework for controlling containers across enviornments.


** **

## 1.   Vision and Goals Of The Project:

Currently if someone wishes to launch an image in a container or perform any other lifecycle management functions on it, they must be sure that the scripts are configured correctly for the target container. For instance, launching images in Docker differs from doing so in CRI-O or containerd. This locks individuals and businesses into whichever container runtime they started with unless they invest the time required to edit the configuration and their scripts which holds the commands for target container. In order to address this and develop a means to enable users to perform lifecycle management in a consistent manner, we must begin with the following:

* Set up and study most common container run times. (e.g., Docker, cri-o)

* Study the mostly used lifecycle management functionalities for these runtimes. (e.g., start/stop execution, ps)

Our main goal is to build a framework which will be able to perform some of the mostly used container lifecycle management functions (e.g., start/stop execution, inspect, ps, etc) on the most popular container runtimes today. This framework will take the form of an executable binary that can be exported as a service accessable by the container runtimes. We will specifically focus on a framework compatible with the Docker, containerd, and CRI-O container runtimes.

While considered a stretch goal given the timeline of the project, the ultimate goal is to enable the set of Container Runtime tests run in the CIS Docker 1.13.0 Benchmark across any container runtime. These tests are specified in Chapter 5 of the document; example checks include restricting Linux Kernel capabilities within containers, limiting memory usage, and avoiding directly exposing the host devices to the containers. Publishing a minumum viable framework for this purpose will enable users to run their security checks using **a single script across the most popular containers.** 

Interoperable container runtime will be a tool that allows user to perform a few common container lifecycle management functions among different runtimes using a single framework.

## 2. Users/Personas Of The Project:

The intended user is a software developer who is developing, testing, and managing applications across containers running on different runtimes.
 
Example Use Case: A software developer would like to launch an image in CRI-O instead of Docker, because he realizes that CRI-O is more adaptable with Kubernetes, and using this capability will provide this application a lot more scalibility. Presently, he needs to deal with changing all the continous-integration scripts in order to be able to test and deploy his application on this new container runtime. With an interoperability framework in place, the developer is able to run a single set of commands which would work on these popular container runtimes.

Our aim is to provide this user a common management framework for controlling/managing containers in those different environments/runtimes.


## 3.   Scope and Features Of The Project:

The project aims to create a service that enables the use of the most used container lifecycle management functions across popular runtimes. The runtimes in scope for capatibility for this project will be Docker, and CRI-O. 

Initally, we plan to implement ps, inspect, and start/stop commands over Docker and CRI-O using our service. We intend to leverage the lower-level runtime runc, to expose lifecycle management functionalities to users so that such functionalities can be leveraged by our service regardless of which runtime the user is working with.

[Subject to change]

containerd is considered a runtime in scope as a stretch goal.

This project aims to esnure that the framework implements commands that satisfy the CIS Docker 1.13.0 Benchmark across our inscope runtimes. In doing so, users will be enabled to run their security checks with a single script rather than requiring separate suites for each runtime. This is considered a stretch goal and is subject to the ability to implement the basic lifecycle functionality. Individual benchmarks may be added into scope without including the full suite, depending on time permitting following completion of initial scope.


These benchmarks are specified in pp 126-180 of the Benchmark documentation, and consists of the following checks:

* Do not disable AppArmor Profile 
* Verify SELinux security options, if applicable 
* Restrict Linux Kernel Capabilities within containers 
* Do not use privileged containers 
* Do not mount sensitive host system directories on containers 
* Do not run ssh within containers 
* Do not map privileged ports within containers 
* Open only needed ports on container 
* Do not share the host's network namespace 
* Limit memory usage for container 
* Set container CPU priority appropriately 
* Mount container's root filesystem as read only 
* Bind incoming container traffic to a specific host interface 
* Set the 'on-failure' container restart policy to 5 
* Do not share the host's process namespace 
* Do not share the host's IPC namespace 
* Do not directly expose host devices to containers 
* Override default ulimit at runtime only if needed 
* Do not set mount propagation mode to shared 
* Do not share the host's UTS namespace 
* Do not disable default seccomp profile 
* Do not docker exec commands with privileged option 
* Do not docker exec commands with user option 
* Confirm cgroup usage 
* Restrict container from acquiring additional privileges 
* Check container health at runtime 
* Ensure docker commands always get the latest version of the image 
* Use PIDs cgroup limit 
* Do not use Docker's default bridge docker0 
* Do not share the host's user namespaces 
* Do not mount the Docker socket inside any containers 

[See https://www.cisecurity.org/benchmark/docker/]

** **

## 4. Solution Concept

Global Architectural Structure Of the Project:

![alt text](https://github.com/BU-NU-CLOUD-F19/Interoperable_Container_Runtime/blob/master/figures/cloud-architecture.png "Hover text")

Interoperable framework will be a service which exists between developer and container runtimes. It aims to provide a means of lifecycle management of container runtimes in interoperable way. That is, a single script would be able to execute lifecyle commands independent of underlying container runtime. 

Design Implications and Discussion:

* The implementation in the background for essential lifecycle functions will be examined, as well as the way containers interact with underlying Operating Systems.
* Any additional functionality to add into scope will be based upon our findings of the initial analysis.
* The interoperable framework will consist of scripts to be developed in Python. 
* The scripts serve as wrappers for functionality so that the end-user can run commands without caring which container runtime is the target container is running on.

Architecture of the Docker runtime:

![alt text](https://github.com/BU-NU-CLOUD-F19/Interoperable_Container_Runtime/blob/master/figures/Docker-architecture.png "Hover text")

Per the OCI standard, Docker, CRI-O, and containerd all use runc to implement low-level functionality. Thus, our intention is to leverage runc to create our interoperable framework. As seen in the above diagram, Docker implements its own image management and APIs for high level functionality on top of the container. Thus, understanding the interaction between the high level functionality and operations in runc is necessary in order to be able to implement the functionality in other frameworks.

## 5. Acceptance criteria

The mininum acceptance criteria is to enable 4 or 5 mostly used commands to execute on at least two different container runtimes. We will target Docker and CRI-O as our primary focus. Our stretch goal criteria include ensuring the commands also work with containerd and be able to run the CIS Docker Benchmark checks on those runtimes.


## 6.  Release Planning:

Release #1 (due by Week 5): 

* Set up at least two container run-time enviroments (i.e., Docker, Cri-o). 
* Start experimentation with life-cycle functions on runtimes
* Give a detailed report about implementation of requested functionalities (i.e., Start/Stop execution, ps, inspect)

Release #2 (due by Week 7): 

* Emprical analysis/evaluation of the overhead in high-level runtimes (e.g., Docker) as opposed to using only runc. 

Release #3 (due by Week 9): 

* Implement and demo two lifecycle functions over two or three (optional) container runtimes in interoperable framework

Release #4 (due by Week 11): 

* Implement and demo one more lifecycle functions over two or three (optional) container runtimes in interoperable framework

Release #5 (due by Week 12): 

* Implement and demo one more lifecycle functions over two or three (optional) container runtimes in interoperable framework

* Finalize report and demo end-to-end

Release #6 (due by Week 13) [Stretch goal]: 

* Demo with Docker CIS Benchmark if time permits


** **
## 7. Concerns

The main questions and concerns we have at this point are regarding understanding how the various runtimes operate. The differences in implementations were not necessarily clear. In particular, understanding how CRI-O works, and why it was developed proved to be a difficulty in our inital analysis of the container runtime functionality. 

** **

## General comments

* Detailed backlogs will take place on the Trello board [Link will be provided]
* This project has an oppurtinity to turn into a research paper that can be submitted to top-tier systems conference
* Architecture diagram will be updated with the more information gained from implementations

** **

## References

[1]: Container Runtimes Part 1: An Introduction to Container Runtimes. https://www.ianlewis.org/en/container-runtimes-part-1-introduction-container-r

[2]: What’s the difference between runc, containerd, docker?. https://medium.com/@alenkacz/whats-the-difference-between-runc-containerd-docker-3fc8f79d4d6e

[3]: Docker components explained. http://alexander.holbreich.org/docker-components-explained/

[4]: opencontainers/runc github repo. https://github.com/opencontainers/runc


** **
## Presentations

https://docs.google.com/presentation/d/1bMloLDt2xd2_FndwoQxTObwoqmAbbAUSy2fq8pvovYQ/edit#slide=id.g61835f440d_2_260


