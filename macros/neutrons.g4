# This is a comment and will be ignored by geant4

# Use this open statement to create an OpenGL view:
/vis/open OGL

# Draw geometry:
/vis/drawVolume

# Specify view angle:
/vis/viewer/set/viewpointThetaPhi 0. 0.

# Specify zoom value:
/vis/viewer/zoom 1.0

# Draw smooth trajectories at end of event, showing trajectory points
# as markers 2 pixels wide:
/vis/scene/add/trajectories smooth
/vis/modeling/trajectories/create/drawByCharge
/vis/modeling/trajectories/drawByCharge-0/default/setDrawStepPts true
/vis/modeling/trajectories/drawByCharge-0/default/setStepPtsSize 2

# To superimpose all of the events from a given run:
#/vis/scene/endOfEventAction accumulate

# Re-establish auto refreshing and verbosity:
/vis/viewer/set/autoRefresh true
/vis/viewer/set/rotationStyle freeRotation

# Now we define the source. Right now it is a mono-energetic beam of muons+
# We can set either energy or momentum, 
# but not both (since the mass is already defined).
# Between energy and momentum, geant4 will follow the last instruction before beamOn.

/gun/particle neutron
/gun/energy 10 MeV
/run/beamOn 1