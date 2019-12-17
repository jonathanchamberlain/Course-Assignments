From https://opensource.com/article/17/12/cri-o-all-runtime-kubernetes-needs:

CRI-O has origins in Red Hat work on tools related to containers, and started as a way to create minimal maintainable runtime dedicated to Kubernetes.

Is an implementation of Kubernetes CRI that allows Kubernetes to use any OCI-compliant runtime as container runtime for running pods. Explicitly supports runc and Clear Containers.

Supports OCI container images, can pull from any compliant container registry; is lightweight alternative to using Docker.

The scope is tied to the CRI.

Components (Go libraries):
Tested on opencontainers/runtime library - this library generates OCI configurations; requires Go v1.10.x or later
containers/storage - libary for managing layers and creating root filesystems 
containers/image - used to pull images from registries, supports Docker v2
containernetworking/cni - sets up networking, CNI plugins have been tested and work as expected
monitoring done using the conmon utilitiy
secuirty provided by series of tools such as SELinux, seccomp, and secuirty separation policies from OCI specification
