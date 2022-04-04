#!/bin/bash

if [[ "$1" == "" ]]
then
	echo "Need to input the parameter"
	echo "You can input the parameters as below."
	echo "clean, distclean, build, run"
	echo "ex) ./docker.sh run"
	exit
fi

DOCKER_REPO="pycounter"
DOCKER_TAG="ci"

docker_clean()
{
	echo "Try to the ${FUNCNAME[0]}"

	# get the container id
	docker_ps=$(docker ps -a | \
		grep "${DOCKER_REPO}:${DOCKER_TAG}" | \
		awk '{printf $1}')

	# if exist the running container
	while [[ ${docker_ps} != "" ]]
	do
		docker kill "${docker_ps}"
		sleep 1

		docker_ps=$(docker ps -a | \
			grep "${DOCKER_REPO}:${DOCKER_TAG}" | \
			awk '{printf $1}')
	done
}

docker_distclean()
{
	echo "Try to the ${FUNCNAME[0]}"

	docker_clean

	# get the docker images information
	docker_image=$(docker images | grep ${DOCKER_REPO} | grep ${DOCKER_TAG})
	if [[ "${docker_image}" != "" ]]
	then
		docker rmi ${DOCKER_REPO}:${DOCKER_TAG}
	fi
}

docker_build()
{
	docker_image=$(docker images | grep ${DOCKER_REPO} | grep ${DOCKER_TAG})
	if [[ ${docker_image} != "" ]]
	then
		echo "Docker image(${DOCKER_REPO}:${DOCKER_TAG}) is exist already."
		exit
	fi

	if [[ ! -e "Dockerfile" ]]
	then
		echo "Not exist the Dockerfile!"
		exit
	fi

	# docker build
	docker build -t ${DOCKER_REPO}:${DOCKER_TAG} .
}

docker_run()
{
	docker_image=$(docker images | grep ${DOCKER_REPO} | grep ${DOCKER_TAG})
	if [[ ${docker_image} == "" ]]
	then
		echo "Docker image(${DOCKER_REPO}:${DOCKER_TAG}) is not exist."
		exit
	fi

	# docker run
	docker run \
		--rm \
		-it \
		${DOCKER_REPO}:${DOCKER_TAG} \
		/bin/bash
}

case "$1" in
	"clean")
		docker_clean
		;;
	"distclean")
		docker_distclean
		;;
	"build")
		docker_build
		;;
	"run")
		docker_run
		;;
	*)
		echo "Invalid parameter!"
		exit
		;;
esac

echo "Done"
