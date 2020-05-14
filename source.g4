# This is a comment and will be ignored by geant4

# Let's ask geant4 to report more details about the simulation
# by ajusting the verbosity level for each subsystem:

/run/verbose 1
/event/verbose 1
/tracking/verbose 1

# Now we define the source. Right now it is a mono-energetic beam of muons+
# We can set either energy or momentum, 
# but not both (since the mass is already defined).
# Between energy and momentum, geant4 will follow the last instruction before beamOn.

/gun/particle neutron
/gun/energy 10 MeV
/gun/position -10 0 0 m
/run/beamOn 1