#!/usr/bin/env bash

declare -a musthave_containers=("apollo_paddlepaddle_volume_$USER" "apollo_localization_volume_$USER" "apollo_local_third_party_volume_$USER")

VERSION_X86_64="dev-18.04-x86_64-20191111_1530"

declare -a images_list=("apolloauto/apollo:paddlepaddle_volume-x86_64-2.0.0" "apolloauto/apollo:localization_volume-x86_64-latest" "apolloauto/apollo:local_third_party_volume-x86_64-latest")

dev_container="apollo_dev_oscar5_$USER"
apollo_base_image="apolloauto/apollo:$VERSION_X86_64"
dev_image="apollo_oscar_dev_$USER"

function show_usage()
{
cat <<EOF
Usage: bash docker/scripts/dev_launch.sh [options] ...
OPTIONS:
    -h, --help             Display this help and exit.
    -r, --refresh          Remove current containers and run it again.
    -p, --pull             Pull new Apollo images (if exists).
    stop                   Stop all running Apollo containers.
EOF
exit 0
}

function print_into_usage()
{
echo "READY TO RUN!"
}

function stop_containers()
{
running_containers=$(docker ps --format "{{.Names}}")

for i in ${running_containers[*]}
do
  if [[ "$i" =~ apollo_* ]];then
    printf %-*s 70 "stopping container: $i ..."
    docker stop $i > /dev/null
    if [ $? -eq 0 ];then
      printf "\033[32m[DONE]\033[0m\n"
    else
      printf "\033[31m[FAILED]\033[0m\n"
    fi
  fi
done
}

function local_volumes() {
    set +x
    # Apollo root and bazel cache dirs are required.
    volumes="-v $APOLLO_ROOT_DIR:/apollo \
             -v $HOME/.cache:${DOCKER_HOME}/.cache"
    APOLLO_TELEOP="${APOLLO_ROOT_DIR}/../apollo-teleop"
    if [ -d ${APOLLO_TELEOP} ]; then
        volumes="-v ${APOLLO_TELEOP}:/apollo/modules/teleop ${volumes}"
    fi
    case "$(uname -s)" in
        Linux)

            case "$(lsb_release -r | cut -f2)" in
                14.04)
                    volumes="${volumes} "
                    ;;
                *)
                    volumes="${volumes} -v /dev:/dev "
                    ;;
            esac
            volumes="${volumes} -v /media:/media \
                                -v /tmp/.X11-unix:/tmp/.X11-unix:rw \
                                -v /etc/localtime:/etc/localtime:ro \
                                -v /usr/src:/usr/src \
                                -v /lib/modules:/lib/modules"
            ;;
        Darwin)
            # MacOS has strict limitations on mapping volumes.
            chmod -R a+wr ~/.cache/bazel
            ;;
    esac
    echo "${volumes}"
}


function run_new_containers()
{

current_containers=$(docker ps --format "{{.Names}}" | grep "apollo_")

images_iter=(0 1 2)
for i in ${images_iter[*]}
do
    if [[ ! " $current_containers " =~ .*\ "${musthave_containers[i]}"\ .* ]];then
        docker run -it -d --name ${musthave_containers[i]} ${images_list[i]}
        OTHER_VOLUME_CONF="${OTHER_VOLUME_CONF} --volumes-from ${musthave_containers[i]}"
    fi
done

local display=""
if [[ -z ${DISPLAY} ]];then
    display=":0"
else
    display="${DISPLAY}"
fi

USER_ID=$(id -u)
GRP=$(id -g -n)
GRP_ID=$(id -g)
LOCAL_HOST=`hostname`
DOCKER_HOME="/home/$USER"

if [ "$USER" == "root" ];then
    DOCKER_HOME="/root"
fi
if [ ! -d "$HOME/.cache" ];then
    mkdir "$HOME/.cache"
fi

USE_GPU=0
if [ -z "$(which nvidia-smi)" ]; then
    echo "No nvidia-driver found! Use CPU."
elif [ -z "$(nvidia-smi)" ]; then
    echo "No GPU device found! Use CPU."
else
    USE_GPU=1
fi

# Try to use GPU in container.
DOCKER_RUN="docker run"
NVIDIA_DOCKER_DOC="https://github.com/NVIDIA/nvidia-docker/blob/master/README.md"
if [ ${USE_GPU} -eq 1 ]; then
    DOCKER_VERSION=$(docker version --format '{{.Server.Version}}')
    if ! [ -z "$(which nvidia-docker)" ]; then
    DOCKER_RUN="nvidia-docker run"
    echo "nvidia-docker is in deprecation!"
    echo "Please install latest docker and nvidia-container-toolkit: ${NVIDIA_DOCKER_DOC}"
    elif ! [ -z "$(which nvidia-container-toolkit)" ]; then
    if dpkg --compare-versions "${DOCKER_VERSION}" "ge" "19.03"; then
        DOCKER_RUN="docker run --gpus all"
    else
        echo "You must upgrade to docker-ce 19.03+ to access GPU from container!"
        USE_GPU=0
    fi
    else
    USE_GPU=0
    echo "Cannot access GPU from container."
    echo "Please install latest docker and nvidia-container-toolkit: ${NVIDIA_DOCKER_DOC}"
    fi
fi


set -x

${DOCKER_RUN} -it \
    -d \
    --privileged \
    --name $dev_container \
    ${OTHER_VOLUME_CONF} \
    -e DISPLAY=$display \
    -e DOCKER_USER=$USER \
    -e USER=$USER \
    -e DOCKER_USER_ID=$USER_ID \
    -e DOCKER_GRP="$GRP" \
    -e DOCKER_GRP_ID=$GRP_ID \
    -e DOCKER_IMG=$IMG \
    -e USE_GPU=$USE_GPU \
    -e NVIDIA_VISIBLE_DEVICES=all \
    -e NVIDIA_DRIVER_CAPABILITIES=compute,video,utility \
    $(local_volumes) \
    --net host \
    -w /apollo \
    --add-host in_dev_docker:127.0.0.1 \
    --add-host ${LOCAL_HOST}:127.0.0.1 \
    --hostname in_dev_docker \
    --shm-size 6G \
    --pid=host \
    -v /dev/null:/dev/raw1394 \
    -v /pool/logs/data:/apollo/data \
    --device /dev/snd \
    -e PULSE_SERVER=unix:${XDG_RUNTIME_DIR}/pulse/native \
    -v ${XDG_RUNTIME_DIR}/pulse/native:${XDG_RUNTIME_DIR}/pulse/native \
    -v ~/.config/pulse/cookie:/root/.config/pulse/cookie \
    --group-add $(getent group audio | cut -d: -f3) \
    $dev_image \
    /bin/bash
if [ $? -ne 0 ];then
    echo "Failed to start docker container \"${dev_container}\" based on image: $dev_image"
    exit 1
fi

set +x

if [ "${USER}" != "root" ]; then
    docker exec $dev_container bash -c '/apollo/scripts/docker_adduser.sh'
fi
docker exec $dev_container bash -c '/apollo/docker/scripts/container_setup.sh'

# oscar_tools setup
docker exec $dev_container sh -c 'echo "source /apollo/scripts/oscar/oscar_tools/setup.sh" >> /home/${USER}/.bashrc'
}

function refresh_containers()
{
cat << EOF
Removing old containers...
EOF

#Firstly stopping running containers to prevent errors
stop_containers

#Removing containers
apollo_containers=$(docker ps -a --format "{{.Names}}" | grep "apollo_")
for i in ${apollo_containers[*]}
do
    printf %-*s 70 "removing container: $i ..."
    docker rm $i > /dev/null
    if [ $? -eq 0 ];then
      printf "\033[32m[DONE]\033[0m\n"
    else
      printf "\033[31m[FAILED]\033[0m\n"
    fi
done
exit 0
}

function pull_images()
{
    for i in ${images_list[*]}
    do
        docker pull $i
    done
    docker pull ${apollo_base_image}
    docker build -f docker/scripts/oscar_dockerfile -t ${dev_image} .
    exit 0
}

APOLLO_ROOT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )/../.." && pwd -P )"

if [ "$(readlink -f /apollo)" != "${APOLLO_ROOT_DIR}" ]; then
    sudo ln -snf ${APOLLO_ROOT_DIR} /apollo
fi

if [ -e /proc/sys/kernel ]; then
    echo "/apollo/data/core/core_%e.%p" | sudo tee /proc/sys/kernel/core_pattern > /dev/null
fi

source ${APOLLO_ROOT_DIR}/scripts/apollo_base.sh



while [ $# -gt 0 ]
do
    case "$1" in
    -r|--refresh)
        refresh_containers
        ;;
    -p|--pull)
        pull_images
        ;;
    -h|--help)
        show_usage
        ;;
    stop)
	    stop_containers
	    exit 0
	    ;;
    *)
        echo -e "\033[93mWarning\033[0m: Unknown option: $1"
        exit 2
        ;;
    esac
        shift
done

function main()
{
    #Try to start local containers
    failed_flag=0
    for i in ${musthave_containers[*]}
    do
        printf %-*s 70 "starting container: $i ..."
        docker start $i
        if [ $? -eq 0 ];then
            printf "\033[32m[DONE]\033[0m\n"
        else
            printf "\033[31m[FAILED]\033[0m\n"
            failed_flag=1
        fi
    done

    docker start $dev_container
    if [ $? -eq 0 ];then
        printf "\033[32m[DONE]\033[0m\n"
    else
        printf "\033[31m[FAILED]\033[0m\n"
        failed_flag=1
    fi
    
    if [ "$failed_flag" -eq 0 ];then
        print_into_usage
    else
        #Running new images due to failed ones"
        run_new_containers
    fi
}

main
