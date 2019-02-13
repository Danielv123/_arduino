FROM node:latest
RUN apt-get update && curl -sL https://deb.nodesource.com/setup_8.x | bash - && apt-get install -y git nodejs
ADD /mcu8266_power_sensor /mcu8266_power_sensor
RUN cd mcu8266_power_sensor && npm install
WORKDIR /mcu8266_power_sensor
CMD ["node", "index.js"]

# sudo docker build -t esp8266_power_meter --no-cache --force-rm esp8266_power_meter
# sudo docker run -d --name esp8266_power_meter esp8266_power_meter
