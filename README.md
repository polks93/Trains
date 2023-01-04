# Trains

*Trains* Simulate a train station with 8 tracks that must be allocated to the incoming trains. Trains can
have different priority and can be generated from the user or automatically. They can arrive and leave from
two tracks on the left and two on the right. The controller task has to manage track switches (also to
be animated) and traffic lights on each track.

## Install dependencies

This application requires Allegro4 to run. The suggested way to install is:

`sudo apt-get install liballegro4.4 liballegro4-4dev`

## Download

`git clone https://github.com/polks93/Trains.git`

## Compile

`make`

## Launch

`sudo bin/main`

## Clear history

`make clean`

*Note: superuser privileges are needed to create real-time threads.*
