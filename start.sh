
#   navigate to backend source
cd ./Backend

#   implement a check to see if the docker service is currently running
#   if not, then start it
sudo service docker start
#   start and run each container in detached mode
docker-compose up -d

#   Navigate back up to project root
cd ..